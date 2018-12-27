#!/usr/bin/env python
# -*- coding: utf-8 -*-


import threading
import Queue
import asyncore
import logging
import socket
import time
from collections import deque
import traceback
import requests
import requests.exceptions
import json
import os
# ====================================

import proxy
import util.log
import device.callback
import consts
import pb.apk_protomsg_pb2
import my_globals
# ====================================
class FailedItem(object):
  MAX_TRY = 3
  ERROR_TIMEOUT = 1
  ERROR_ADB = 2
  def __init__(self):
    self.package_name = None
    self.try_times = 0
    self.error_code = None
    self.error_message = None
    #根据error_message产生的内部错误码，比如如果文件不存在则没必要重试，
    #INSTALL_FAILED_VERSION_DOWNGRADE则可以尝试先卸载后安装
    #0表示默认重试
    self.inner_error_code = 0
    
  
  def TryAgain(self):
    self._GenInnerErrorCode()
    if self.try_times < FailedItem.MAX_TRY and self.inner_error_code < 1000:
      return (True, self.inner_error_code)
    else:
      self.try_times = FailedItem.MAX_TRY
      return (False, self.inner_error_code)
  
  
  
  def _GenInnerErrorCode(self):
    allow_try = {
      u'INSTALL_FAILED_VERSION_DOWNGRADE': 1,
      
    }
  
    no_need_try = {
      u'No such file or directory': 1000,
      u'INSTALL_FAILED_INSUFFICIENT_STORAGE': 1001,
    }
    if self.error_message is not None:
      for k, v in allow_try.items():
        if k in self.error_message:
          self.inner_error_code = v
          return
        
      
      for k, v in no_need_try.items():
        if k in self.error_message:
          self.inner_error_code = v
          return
      
        
      
  
  
  
class OneDevice(object):
  def __init__(self, queue_master, serial_number):
    self.log = util.log.GetLogger(self.__class__.__name__)
    self.log.info('OneDevice ' + serial_number)
    print("OneDevice.init=======================================")
    self.queue_master = queue_master
    self.serial_number = serial_number
    self.proxy = None
    
    self.model = None
    # 记录所有已经成功安装的包，除非强制清空，不然不再重复安装
    self.installed_set = set()

    # 记录所有超时的包，超时包会导致后续安装无法进行，除非强制，否转不再重复安装
    # self.installed_timeout_set = set()
    
    
    #记录所有失败的包，adb返回的失败，超时失败，重试次数
    self.installed_failed = {}
    
    #摘要
    self.digest = {
      'total_number': 0,
      'success_number': 0,
      'failed_number': 0,
      'time_cost': 0,
      'fail_list': [],
    }
    
    
    #当发生过杀线程的事情，sub_min id被设置成杀之前接收命令的最大id，确保如果无法正确杀死线程，也不会被线程后续回复干扰
    self.min_sub_command_id = 0
    self.last_sub_command_id = 0

    self.package_list = []
    self.package_set = set()
    
    
    self.current_installapk_response = {}
    
    self.sub_command_id = 0
    
    #需要进行安装的apk集合
    self.todo_install_apk_set = set()
    
    
  
  
  def _GenSubCommandID(self):
    self.sub_command_id += 1
    return self.sub_command_id
  
  
  def CopyCommand(self, command):
    new_one = pb.apk_protomsg_pb2.Command()
    new_one.cmd = command.cmd
    new_one.cmd_no = command.cmd_no
    new_one.sub_cmd_no = self._GenSubCommandID()
    for one in command.param:
      new_one.param.append(one)
      
    return new_one
   
   
  def _Start(self):
    self.proxy.Start()
      
      
  def Stop(self):
    self.min_sub_command_id = self.last_sub_command_id
    if self.proxy is not None:
      self.proxy.Stop()
      self.proxy.ForceStop()
      
  
  def Destroy(self):
    self.proxy = None
      
      
  
  def _Create(self):
      self.proxy = proxy.Proxy(self.queue_master, self.serial_number)
      
      
  
  def MaybeCreate(self):
    if self.proxy is None:
      self._Create()
      self._Start()
      
      
  
  def Empty(self):
    if self.proxy is not None:
      return self.proxy.Empty()
    
    #如果proxy不存在，也就没有任务队列，那默认非empty
    return False


  def UpdateApkList(self, data):
    self.todo_install_apk_set = data
    
    
      
  def IsInstalled(self, apk):
    return apk in self.installed_set
  
  
  def IsInstallFailed(self, apk):
    return apk in self.installed_failed
  
  
  
  def IsTimeOut(self, apk):
    return apk in self.installed_failed and self.installed_failed[apk].error_code == FailedItem.ERROR_TIMEOUT
  
  
  def AddInstalled(self, command):
    self.log.info('AddInstalled ' + self.serial_number + ' ' + command.package_name)
    self.installed_set.add(command.package_name)
    #耗时记录
    self.digest['time_cost'] += command.time_cost
    
  
  def AddTimeOut(self, apk, time_cost):
    self.log.info('AddTimeOut ' + self.serial_number + ' ' + apk)
    self.AddInstallFailed(apk, FailedItem.ERROR_TIMEOUT, 'timeout', time_cost)
    
  
  
  def AddInstallFailed(self, package_name, reason, progress, time_cost):
    self.log.info('AddInstallFailed  %s  %s %d %s', self.serial_number, package_name, reason, progress)
    apk = package_name
    if apk in self.installed_failed:
      self.installed_failed[apk].try_times += 1
      self.installed_failed[apk].error_code = reason
      self.installed_failed[apk].error_message = progress
    else:
      item = FailedItem()
      item.package_name = apk
      item.try_times = 1
      item.error_code = reason
      item.error_message = progress
      self.installed_failed[apk] = item

    # 耗时记录
    self.digest['time_cost'] += time_cost
  
    
    
  def SendCommand(self, command):
    if self.proxy is not None:
      # 添加sub_command_id
      command.sub_cmd_no = self._GenSubCommandID()
      self.last_sub_command_id = command.sub_cmd_no
      self.proxy.SendCommand(command)



  def NotifyInstallApkDigest(self, command):
    out = pb.apk_protomsg_pb2.CommandInstallApkDigest()
    out.cmd = consts.COMMAND_NOTIFY_INSTALL_APK_DIGEST
    out.total_number = len(self.todo_install_apk_set)
    out.success_number = len(self.installed_set)
    out.failed_number = len(self.installed_failed)
    out.time_cost = self.digest['time_cost']
    out.serial_number = self.serial_number
    out.model = self.model
    for one in self.installed_failed.values():
      failed = out.fail_list.add()
      failed.package_name = one.package_name
      failed.adb_message = one.error_message
      failed.user_message = consts.AdbMessage2UserMessage(one.error_message)
      failed.try_times = one.try_times
      
    return out
    


  def ProcessInstallApkResponse(self, command):
    if command.sub_cmd_no > self.min_sub_command_id:
      if command.stage == '完成'.decode('utf-8'):
        if command.progress == 'Success'.decode('utf-8'):
          self.AddInstalled(command)
        else:
          self.AddInstallFailed(command.package_name, FailedItem.ERROR_ADB, command.progress, command.time_cost)

        # 发送装包摘要
        notify = self.NotifyInstallApkDigest(command)
        my_globals.queue_network.put(notify)
      
        
          
      return True
    
    else:
      self.log.warning('ProcessInstallApkResponse  the min: %d  the one: %d', self.min_sub_command_id, command.cmd_no)
  

  
  def ProcessPackageListResponse(self, command):
    self.package_list = command['package_list']
    self.package_set = set(self.package_list)
    self.log.info(self.serial_number + ' package list:')
    for one in self.package_list:
      self.log.info(one)
      
      
      
  def IsInPackageList(self, package):
    return package in self.package_set
  
  
  
  def TriggerProcessPackageList(self):
    pass



  def ProcessInstallApk(self, command):
    # 添加sub_command_id
    command.sub_cmd_no = self._GenSubCommandID()
    type = command.param[0].encode('utf-8')
    apk_path = command.param[1].encode('utf-8')
    package_name = util.utility.GetPackageNameNoApkExt(apk_path)
    self.GenInstallApkResponse(target=self.queue_master, command=command, serial_number=self.serial_number, package_name=package_name,
                               time_max=0, package_size=0, type=type)

    if package_name in self.package_set:
      self.log.info('ProcessInstallApk delete first')
      command.param.append(consts.INSTALL_TYPE_DELTE_FIRST.decode('utf-8'))
    else:
      self.log.info('ProcessInstallApk new install')
      command.param.append(consts.INSTALL_TYPE_JUST_INSTALL.decode('utf-8'))
  
    if 'force' in type:
      self.log.info('ProcessInstallApk  force ' + command.cmd)
      self.SendCommand(command)
    else:
      self.log.info('ProcessInstallApk ' + command.cmd)
    
      if self.IsInstalled(package_name):
        callback.SendInstallApkResponse(self.GenInstallApkResponse(error=consts.ERROR_CODE_OK,
                                                                   stage='完成', progress='已经装过跳过安装'))
      elif self.IsTimeOut(package_name):
        callback.SendInstallApkResponse(self.GenInstallApkResponse(error=consts.ERROR_CODE_PYADB_OP_TIMEOUT_FAILED,
                                     stage='完成', progress='曾经超时跳过安装'))
      else:
        self.SendCommand(command)



  def GenInstallApkResponse(self, target=None, command=None, error=consts.ERROR_CODE_OK, serial_number=None, stage=None,
                            package_name=None, progress=None, time_max=None,
                            package_size=None, type=None, adb_message=None, info=None):
    return callback.GenInstallApkResponse(self.current_installapk_response, target=target, command=command, error=error, serial_number=serial_number, stage=stage,
                            package_name=package_name, progress=progress, time_max=time_max, package_size=package_size, type=type, adb_message=adb_message, info=info)

  
  def _GenInstallApkCommand(self, package_name):
    command = pb.apk_protomsg_pb2.Command()
    command.cmd = consts.COMMAND_INSTALL_APK
    command.cmd_no = -1
    command.sub_cmd_no = self._GenSubCommandID()

    command.param.append(u'all')
    apk_path = my_globals.apk_dir + '\\' + package_name + '.apk'
    command.param.append(apk_path)
    return command
#########################################################
class Master(object):
  def __init__(self, queue_out):
    print("Master.init=======================================")
    # self.host = host
    self.queue_out = queue_out
    self.queue_in = Queue.Queue()
    self.devices_map = {}
    # self.last_alive = {}
    self.last_devices_list = []
    self.last_devices_map = {}
    
    self.pending_task_list = deque()
    
    
    # self.apk_map = {}
    self.todo_install_apk_set = set()
    
    
    
    
    #超时map
    self.last_command_timeout = {}

    self.log = util.log.GetLogger(self.__class__.__name__)

    self.thread = threading.Thread(target=self.Work)
    self._continue = True
    self.thread.setDaemon(True)

    self.auto_install_mode = False
    
    
    self.apk_path = None
    self.json_path = None
    self.local_prop = None



  def EnterAutoInstall(self, command):
    if int(command.param[0]) == 1:
      self.auto_install_mode = True
      self.TriggerAutoInstall()
    else:
      self.auto_install_mode = False
      
      
  
  def MayTryAgain(self, device, apk):
    # 检查所有设备，如果要安装包数大于已经成功包数，且失败次数位超过阈值，重新把安装请求塞到队列中
    # 没有失败过或者失败重试没有到达上限
    if not device.IsInstallFailed(apk):
      command = device._GenInstallApkCommand(apk)
      device.ProcessInstallApk(command)

    else:
      one = device.installed_failed[apk]
      try_again, way = one.TryAgain()
      if try_again:
        goon = False
        command = device._GenInstallApkCommand(apk)
        if way == 1:
          # 需要先卸载后安装
          command.param.append(consts.INSTALL_TYPE_DELTE_FIRST.decode('utf-8'))
          goon = True
    
        # if goon:
        device.ProcessInstallApk(command)
      
      



  def TriggerAutoInstall(self):
    # 自动安装逻辑
    if self.auto_install_mode:
      for one in self.last_devices_list:
        device = self._Add(one['serial_no'])
        if device.Empty():
          for apk in device.todo_install_apk_set:
            # 没有安装成功
            if not device.IsInstalled(apk):
              self.MayTryAgain(device, apk)
      
    
    
  
  def UpdateApkList(self, data):
    self.todo_install_apk_set = data
    for one in self.last_devices_list:
      device = self._Add(one['serial_no'])
      device.UpdateApkList(data)
      

    
  
  def _Add(self, serial_number):
    if serial_number in self.devices_map:
      self.devices_map[serial_number].MaybeCreate()
      return self.devices_map[serial_number]
    else:
      one = OneDevice(self.queue_in, serial_number)
      if serial_number != '-':
        one.model = self.last_devices_map[serial_number]['model']
      #把要装的包集合设置好
      one.UpdateApkList(self.todo_install_apk_set)
      self.devices_map[serial_number] = one
      one.MaybeCreate()
      return one
  
  
  def _Remove(self, serial_number):
    if serial_number in self.devices_map:
      self.devices_map[serial_number].Stop()
      self.devices_map[serial_number].Destroy()
      
  
  def _Get(self, serial_number):
    return self.devices_map[serial_number]
      
  
  def ProcessCommand(self, serial_number, command):
    #每次执行新命令前，看看旧有设备有无超时的
    self.CheckTimeout()
    
    
    one = self._Add(serial_number)
    if command.cmd == consts.COMMAND_INSTALL_APK:
      one.ProcessInstallApk(command)
    else:
      one.SendCommand(command)
    
    
  
  def CheckTimeout(self):
    # K_TIMEOUT = 120
    # 检查所有，干掉超时
    tmp = self.last_command_timeout.copy()
    for k, v in tmp.items():
      if time.time() - tmp[k]['t'] > 0:
        # 目前应该只有装包会超时
        if v['c'].cmd == consts.COMMAND_INSTALL_APK:
          self.last_command_timeout.pop(k)
          apk_path =v['c'].param[1].encode('utf-8')
          package_name = util.utility.GetPackageNameNoApkExt(apk_path)
          #超时的包，记录超时失败，不再继续
          self._Get(k).AddTimeOut(package_name, v['max'])
          # 发送装包摘要
          notify = self._Get(k).NotifyInstallApkDigest(None)
          self.queue_out.put(notify)
          #发送超时通知
          device.callback.SendCommandResponse(self.queue_out, v['c'],
                                       consts.ERROR_CODE_PYADB_OP_TIMEOUT_FAILED,
                                       [k.encode('utf-8'), '完成', package_name, '超时', str(tmp[k]['max'])])
          
        self._Remove(k)
      
        
        
    
    
  
  def ProcessScanDevicesResponse(self, command):
    self.last_devices_list = []
    self.last_devices_map = {}
    for one_device in command.devices_list:
      one = {}
      one['serial_no'] = one_device.serial_no
      one['product'] = one_device.product
      one['model'] = one_device.model
      one['device'] = one_device.device
      self.last_devices_list.append(one)
      self.last_devices_map[one['serial_no']] = one
      
    
    #对于所有手机，如果自动模式，开始装包
    self.TriggerAutoInstall()
      
    
    return True



  def ProcessInstallApkResponse(self, command):
    return self._Get(command.serial_number).ProcessInstallApkResponse(command)
  

  
    
  def ProcessIncome(self, command):
    forword = True
    if isinstance(command, pb.apk_protomsg_pb2.DevicesList):
      forword = self.ProcessScanDevicesResponse(command)
    elif isinstance(command, dict):
      if command['c'] == consts.COMMAND_INNER_START_CHECKOUT_TIMEOUT:
        forword = self.ProcessStartCheckTimeOut(command)
      elif command['c'] == consts.COMMAND_INNER_STOP_CHECKOUT_TIMEOUT:
        forword = self.ProcessStopCheckTimeOut(command)
      elif command['c'] == consts.COMMAND_INNER_PACKAGE_LIST:
        forword = self.ProcessPackageListResponse(command)
    elif command.cmd == consts.COMMAND_INSTALL_APK:
      forword = self.ProcessInstallApkResponse(command)
    
      
    if forword is not None and forword == True:
      self.queue_out.put(command)
      
      
  
  def ProcessStartCheckTimeOut(self, command):
    serial_number = command['s']
    self.last_command_timeout[serial_number] = {}
    self.last_command_timeout[serial_number]['c'] = command['command']
    self.last_command_timeout[serial_number]['max'] = command['t']
    self.last_command_timeout[serial_number]['t'] = time.time() + command['t']
    
    self.log.info('ProcessStartCheckTimeOut ' + serial_number + ' ' + str(command['t']))
    
    
    
  def ProcessPackageListResponse(self, command):
    serial_number = command['s']
    return self._Get(serial_number).ProcessPackageListResponse(command)
    pass
    
  
  def ProcessStopCheckTimeOut(self, command):
    try:
      serial_number = command['s']
      self.last_command_timeout.pop(serial_number)
      self.log.info('ProcessStopCheckTimeOut ' + serial_number)
    except Exception as e:
      pass
        

  def Start(self):
    self.thread.start()
  
  
  def DealWithIncomeQueue(self):
    while not self.queue_in.empty():
      msg = self.queue_in.get()
      self.ProcessIncome(msg)
      self.queue_in.task_done()
  
  
  def Work(self):
    
    while self._continue:
      self.DealWithIncomeQueue()
      self.CheckTimeout()
      self.TriggerAutoInstall()
      self.CheckPendingTask()
      time.sleep(1)
  
  
  def Join(self):
    self.thread.join()
    
    
  
  def CheckPendingTask(self):
    while len(self.pending_task_list):
      if self.pending_task_list[0][0] < time.time():
        pass
      
      
      
      
      
      
#########################################################
  def DownloadFile(self, command, one, file_path, callback):
    url = one['apkurl']
  
    headers = {
      'User-Agent': 'Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.132 Safari/537.36',
    }
    ret = False
    try:
      try:
        response = requests.request("GET", url, stream=True, data=None, headers=headers)
      except requests.exceptions.SSLError as e:
        response = requests.request("GET", url, stream=True, data=None, headers=headers, verify=False)
    
      total_length = int(response.headers.get("Content-Length"))
      now = 0
      with open(file_path, 'wb') as f:
        for chunk in response.iter_content(chunk_size=8096):
          if chunk:
            f.write(chunk)
            f.flush()
            now += len(chunk)
            callback(now, total_length)
        else:
          callback(total_length, total_length)
          return True
    except Exception as e:
      exstr = traceback.format_exc()
      print(exstr)
      self.log.info(exstr)
      self.SendCommandResponse(command, consts.ERROR_CODE_DOWNLOAD_APK_FAILED,
                               ['包更新',
                                consts.error_string(consts.ERROR_CODE_DOWNLOAD_APK_FAILED),
                                one['apkname']])
    return ret

  class DownloadCallbck(object):
    def __init__(self, command, one, host):
      self.host = host
      self.last_report = 0
      self.command = command
      self.one = one
  
    def callback_progress(self, now, total):
      percent = float(now) / total
      if percent - self.last_report > 0.05 or now == total:
        self.last_report = percent
        data = ("%.2f%%" % (percent * 100))
        self.host.SendCommandResponse(self.command, consts.ERROR_CODE_OK, ['包更新', '下载中', self.one['apkname'], data])
      pass

  def DownloadOneApk(self, one, command):
    result = False
    file_path = self.apk_path + '/' + one['apkname'] + '.apk'
  
    # 检查文件是否存在
    if os.path.exists(file_path):
      # 文件存在判断md5是否匹配
      md5 = util.utility.GetFileMD5(file_path)
      if md5 != one['apkmd5']:
        os.remove(file_path)
      else:
        self.SendCommandResponse(command, consts.ERROR_CODE_OK, ['包更新', '存在，无需下载', one['apkname']])
        result = True
        return result
  
    # 开始下载
    self.SendCommandResponse(command, consts.ERROR_CODE_OK, ['包更新', '开始下载', one['apkname']])
  
    try:
      dc = Master.DownloadCallbck(command, one, self)
      ret = self.DownloadFile(command, one, file_path, dc.callback_progress)
      if ret:
        md5 = util.utility.GetFileMD5(file_path)
        if md5 != one['apkmd5']:
          # 删除文件
          os.remove(file_path)
          self.SendCommandResponse(command, consts.ERROR_CODE_MD5_APK_FAILED,
                                   ['包更新', consts.error_string(consts.ERROR_CODE_MD5_APK_FAILED),
                                    one['apkname']])
        else:
          self.SendCommandResponse(command, consts.ERROR_CODE_OK,
                                   ['包更新',
                                    '下载成功',
                                    one['apkname']])
          result = True
  
    except Exception as e:
      exstr = traceback.format_exc()
      print(exstr)
      self.log.info(exstr)
    
      self.SendCommandResponse(command, consts.ERROR_CODE_SAVE_APK_FILE_FAILED,
                               ['包更新',
                                consts.error_string(consts.ERROR_CODE_SAVE_APK_FILE_FAILED),
                                one['apkname']])
  
    return result

  # //检查json是否符合要求，该有的字段都有效，都存在
  def ParseJson(self, data):
    """
    {"code": 0,
     "msg": "",
     "data":
       {"install":
          [{"id": 2, "apkurl": "http:\/\/s0qrdt.kdndj.com\/files\/d5e4ce46458ce2c4940b8702cefa9e44.apk",
            "apkmd5": "d3edb026aa97b5f429b6da1ebe746005",
            "name": "\u817e\u8baf\u5e94\u7528\u5b9d",
            "brief": "\u4e3a\u667a\u80fd\u624b\u673a\u7528\u6237\u6253\u9020\u7684\u4e00\u4e2a\u624b\u673a\u5e94\u7528\u83b7\u53d6\u5e73\u53f0",
            "orderid": 1,
            "updated_at": "2018-10-24 17:21:44",
            "created_at": "2018-10-24 11:38:28",
            "apkname": "com.tencent.android.qqdownloader",
            "price": "2.00"}
    """
    result = False
    try:
      if data['code'] == 0:
        for one in data['data']['install']:
          if 'apkname' not in one or 'apkurl' not in one or 'apkmd5' not in one or 'name' not in one or 'price' not in one:
            return False
      
        for one in data['data']['remove']:
          if 'apkname' not in one:
            return False
      
        return True
  
    except Exception as e:
      print(e)
  
    return result

  def PullJsonFile(self):
    URL = 'https://www.ppndj.com/jc/user/apkinstall?ch=1234'
    # URL = 'https://www.xppndj.com/jc/user/apkinstall?ch=1234'
    headers = {
      'User-Agent': 'Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.132 Safari/537.36',
    }
  
    s = requests.Session()
    s.mount('http://', requests.adapters.HTTPAdapter(max_retries=5))
    s.mount('https://', requests.adapters.HTTPAdapter(max_retries=5))
    try:
      r = s.get(URL, headers=headers, timeout=5)
      if r.status_code == 200:
        try:
          json_data = json.loads(r.content)
          return (consts.ERROR_CODE_OK, json_data)
        except Exception as e:
          print(e)
          return (consts.ERROR_CODE_PARSE_JSON_FAILED, None)
      else:
        print('net Error', r.status_code)
    except requests.ConnectionError as e:
      print('Error', e.args)
    return (consts.ERROR_CODE_PULL_JSON_FAILED, None)


  def CheckUpdate(self, data, command):
    self.SendCommandResponse(command, consts.ERROR_CODE_OK, ['配置更新', '获取网络配置成功', ])
    try:
      util.utility.CreateDir(self.apk_path)
      if self.ParseJson(data):
        self.SendCommandResponse(command, consts.ERROR_CODE_OK, ['配置更新', '解析网络配置成功', ])
        need_save_json = False
        if self.local_prop is not None:
          # 检查两个文件的区别，并下载最新apk，完成md5校验。之后，更新配置文件到本地
          old_package_name_set = set()
          for old_one in self.local_prop['data']['install']:
            old_package_name_set.add(old_one['apkname'])
          new_package_name_set = set()
          for new_one in data['data']['install']:
            new_package_name_set.add(new_one['apkname'])
        
          diff_set = new_package_name_set - old_package_name_set
          solved_package_name = set()
          for new_one in data['data']['install']:
            for old_one in self.local_prop['data']['install']:
              # 1 如果包名在差集合，说明是新增，需要下载
              # 2 如果包名不在差集，说明不是新增，但md5不一致，说明换包了，需要下载
              # 3 如果包名不在差集，说明不是新增，md5一致，但是本地没文件，需要下载
              file_path = self.apk_path + '/' + new_one['apkname'] + '.apk'
            
              if new_one['apkname'] in diff_set or \
                  (new_one['apkname'] == old_one['apkname'] and new_one['apkmd5'] != old_one['apkmd5']) or \
                  (new_one['apkname'] == old_one['apkname'] and os.path.exists(file_path) is False):
                need_save_json = True
                if new_one['apkname'] not in solved_package_name and self.DownloadOneApk(new_one, command):
                  solved_package_name.add(new_one['apkname'])
      
      
        else:
          # 不存在本地配置
          need_save_json = True
          for new_one in data['data']['install']:
            self.DownloadOneApk(new_one, command)
      
        if need_save_json == True:
          # 保存最新的json文件到本地
          if util.utility.WriteJsonFile(self.json_path, data):
            self.SendCommandResponse(command, consts.ERROR_CODE_OK, ['配置更新', '保存配置文件成功', ])
          else:
            self.SendCommandResponse(command, consts.ERROR_CODE_SAVE_JSON_FILE_FAILED,
                                     ['配置更新', '保存配置文件失败', ])
        else:
          self.SendCommandResponse(command, consts.ERROR_CODE_OK, ['配置更新', '无需更新配置文件', ])
          pass
    
      else:
        self.SendCommandResponse(command, consts.ERROR_CODE_PARSE_JSON_FAILED, ['配置更新', '解析网络配置失败', ])
        return
  
    except Exception as e:
      self.SendCommandResponse(command, consts.ERROR_CODE_UNKNOWN, ['检查更新', ])
      exstr = traceback.format_exc()
      print(exstr)
      self.log.info(exstr)
  
    return
    pass



  def SendCommandResponse(self, cmd, code, info=None):
    response = pb.apk_protomsg_pb2.CommandResponse()
    response.cmd = cmd.cmd
    response.cmd_no = cmd.cmd_no
    response.sub_cmd_no = cmd.sub_cmd_no
    start_timestamp = cmd.timestamp
    response.time_cost = util.utility.UTCTime2TimeInMicroseconds(int(time.time())) - start_timestamp
    response.code = code
    if info is not None:
      for one in info:
        response.info.append(one.decode('utf-8'))

    self.queue_out.put(response)
  
  

# ======================================
if __name__ == '__main__':
  pass