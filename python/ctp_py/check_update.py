#!/usr/bin/env python
# -*- coding: utf-8 -*-

import time
import datetime
import Queue
import os
import json
import sys
import traceback
import requests
import requests.exceptions
# HTTPSConnectionPool(host='s0qrdt.kdndj.com', port=443):
# Max retries exceeded with url: /files/0eccc965d5d8693b2e0cbc9fda97104c.apk
# (Caused by SSLError(CertificateError("hostname 's0qrdt.kdndj.com' doesn't match either of 'img.ucdl.pp.uc.cn', 'api.flash.cn', 'cdn.osupdateservice.yunos.com', 'static.flash.cn', 'www.flash.cn', 'dl6.ztems.com', 'slient.ucdl.pp.uc.cn', 'apimini.flash.2144.com', 'dl1.ztems.com', 'alissl.ucdl.pp.uc.cn', 'iscsi.ucdl.pp.uc.cn', 'stage.admin.flash.cn', 'mini.flash.2144.com', 'tongji.flash.cn', 'test.flash.cn', 'oss.ucdl.pp.uc.cn', 'stage-api.flash.cn', 'admin.flash.cn', 'tool.flash.cn', 'tongji.flash.2144.com', 'static-stage.flash.cn', 'stage.flash.cn'",),))
# 对于python自带的 urllib库  解决办法
# import ssl
# ssl.match_hostname = lambda cert, hostname: True
#====================================
import util.thread_class
import pb.mq_protomsg_pb2
import pb.cp_comunication_pb2
import pb.apk_protomsg_pb2
#import util
import tempfile
import config
import util.log
import util.utility
import consts
import adbtool.start_server
import adbtool.install
import adbtool.list_devices
import adbtool.find_server
import device.master
import device.callback
#=======================================================


# class CallbackObject():
#   def __init__(self, host, command, serial_no, apk):
#     self.host = host
#     self.command = command
#     self.serial_no = serial_no
#     self.last_progress = 0
#     self.apk = apk
#
#   def Callback(self, apk, now, total):
#
#     progress = float(now) / total
#     if (progress - self.last_progress) > 0.05 or now == total:
#       self.last_progress = progress
#       self.SendCommandProgress(self.command, consts.ERROR_CODE_OK,
#                                [self.serial_no.encode('utf-8'), apk, str(progress), ])
#
#
#   def CallbackSucc(self, progress):
#     stage = '进行中'
#     if progress == 'Success':
#       stage = '完成'
#     elif progress == 'push over: ':
#       stage = '安装中'
#     self.host.SendCommandProgress(self.command, consts.ERROR_CODE_OK,
#                              [self.serial_no.encode('utf-8'), stage, self.apk, progress, ])
#
#
#   def CallbackFail(self, progress):
#     self.host.SendCommandProgress(self.command, consts.ERROR_CODE_PYADB_INSTALL_APK_FAILED,
#                                   [self.serial_no.encode('utf-8'), '完成', self.apk, progress, ])
      


#======================================================================
class CheckUpdateApkList(util.thread_class.ThreadClass):

  def InitGlobalLogByFile(self, prop_file):
    if os.path.exists(prop_file):
      prop = util.utility.ReadJsonProp(prop_file)
      self.InitGlobalLogByParam(prop)
      
  def InitGlobalLogByParam(self, prop):
    util.prop.Init(prop)
    util.log.Init(prop)

  def __init__(self, prop_file, queue_in, queue_out):
    if prop_file is not None and os.path.exists(prop_file):
      self.InitGlobalLogByFile(prop_file)
      prop = util.utility.ReadJsonProp(prop_file)
      self.prop = prop
    else:
      #prop是none，说明启动参水里面只有一个port，要等c++把配置信息通过socket传过来
      self.prop = None
      
    util.thread_class.ThreadClass.__init__(self, queue_in)
    self.queue_out = queue_out
    
    
    self.local_prop = None
    self.apk_dir = None
    
    # self.device = adb_wrapper.ADBWrapper()
    # self.device2 = adb_wrapper2.AdbInstall()
    self.last_devices_list = None
    
    self.device = device.master.Master(queue_out)
    self.device.Start()
    
    

  def Send(self, data):
    self.queue_out.put(data)


  def RegisterHandler(self, src):
    src.RegisterMessageHandlerWithName(self, 'apk.Command')


  def LoadLocalProp(self):
    try:
      tmp = util.utility.ReadJsonProp(self.prop['localPath'])
      if self.ParseJson(tmp):
        self.local_prop = tmp
        return self.local_prop
    except Exception as e:
      return None



  def Init2(self, prop):
    self.prop = prop
    self.Init()

  def Init(self):
    if self.prop is not None:
      self.log = util.log.GetLogger(self.__class__.__name__)
      self.log.info('CheckUpdateApkList init')
      self.log.info(str(self.prop))
      try:
        self.apk_dir = self.prop['apkPath']
        self.LoadLocalProp()
      except Exception as e:
        pass
  
      self.debugPath = self.prop['debugPath']
      util.utility.CreateDir(self.prop['apkPath'])
      
      #设置adbexe文件
      adbtool.base.AdbCommandBase.adb = self.prop['adb_exe']


  def Work(self):
    time.sleep(1)


  def Handle(self, msg):
    #print(msg)
    self.queue.put(msg)

  def Name(self):
    return 'ctp.mq.QueryLevelMQ'




  def DealMsg(self, msg):
    if msg.DESCRIPTOR.full_name == 'apk.Command':
      if msg.cmd == 'py_config':
        #完成初始化
        if self.prop is None:
          self.InitGlobalLogByParam(config.CreateProp(msg))
          self.Init2(config.CreateProp(msg))
          #先尝试把自己的adb server启动起来
          self.ProcessStartServer(None)
      elif self.prop is not None:
        #只有完成了初始化才能响应业务请求
        if msg.cmd == 'check_net_package_list':
          self.ProcessCheckUpdatePackageList(msg)
        elif msg.cmd == 'remove_local_package_list':
          self.ProcessRemoveLocalPackageList(msg)
        elif msg.cmd == 'get_local_package_list':
          self.ProcessGetLocalPackageList(msg)
        elif msg.cmd == consts.COMMAND_INSTALL_APK:
          self.ProcessInstallApk3(msg)
        elif msg.cmd == consts.COMMAND_SCAN_DEVICES:
          self.ProcessScanDevices3(msg)
        elif msg.cmd == consts.COMMAND_GET_PACKAGE_LIST:
          self.ProcessGetPackageList(msg)



  def ProcessStartServer(self, command):
    #遍历所有的server，尝试kill，保留kill不掉的
    live = adbtool.find_server.FindAllServer()
    adbtool.find_server.KillAllServer(live)
    live = adbtool.find_server.FindAllServer()
    # 获取一个port
    if len(live) > 0:
      ports = adbtool.find_server.FindPort(live)
      # 复用老server
      start_server = adbtool.start_server.Command(ports)
      start_server.Execute()
      succ, pid = start_server.GetReturnCode()
    else:
      port = adbtool.start_server.Command.GenPort()
      # 产生新server
      start_server = adbtool.start_server.Command(port)
      start_server.Execute()
      succ, pid = start_server.GetReturnCode()
  
    # # 总是尝试先看看这个port有没有人用
    # netstat = cmdtool.netstat.Command(port)
    # netstat.Execute()
    #
    # # 有人用kill掉
    # succ, pid = netstat.GetReturnCode()
    # if succ:
    #   kill = cmdtool.taskkill.Command(pid)
    #   kill.Execute()
    
    
   
  def ProcessScanDevices3(self, command):
    self.device.ProcessCommand('-', command)
    
    
  
  def ProcessGetPackageList(self, command):
    # 此处有并发隐患
    for one in self.device.last_devices_list:
      self.device.ProcessCommand(one['serial_no'], command)

  
    

   
  # def ProcessScanDevices(self, command):
  #   succ, device_list = self.device.ListDevices()
  #   if succ:
  #     out = []
  #     for one in device_list:
  #       device = {}
  #       device['serial_no'] = one
  #       out.append(device)
  #
  #     self.SendDevicesList(command, consts.ERROR_CODE_OK, out)
  #   else:
  #     #self.SendDevicesList(command, CheckUpdateApkList.ERROR_CODE_PYADB_SCAN_DEVICES_FAILED, None, str(device_list))
  #     pass






  def ProcessInstallApk3(self, command):
    #此处有并发隐患
    # tmp = self.device.last_devices_list.copy()
    for one in self.device.last_devices_list:
      self.device.ProcessCommand(one['serial_no'], command)
      
      

  # def ProcessInstallApk2(self, command):
  #   try:
  #     apk_path = command.param[1].encode('utf-8')
  #     package_name = self.GetPackageNameFromPath(apk_path)
  #
  #     if command.param[0].encode('utf-8') == 'all':
  #       if self.last_devices_list is not None:
  #         for one in self.last_devices_list:
  #
  #           callback = CallbackObject(self, command, one['serial_no'], package_name)
  #           install = adbtool.install.Command(one['serial_no'], package_name, apk_path, callback.CallbackSucc, callback.CallbackFail)
  #
  #           self.log.info('before ProcessInstallApk2 ' + one['serial_no'] + ' : ' + apk_path)
  #           self.SendCommandProgress(command, consts.ERROR_CODE_OK,
  #                                         [one['serial_no'].encode('utf-8'), '开始', package_name, '', ])
  #
  #           install.Execute()
  #
  #           self.log.info('end ProcessInstallApk2')
  #
  #     elif command.param[0].encode('utf-8') == 'first':
  #       #apk = r'C:\workspace\code\chromium24\src\build\Debug\ctp_data\apk\com.tencent.android.qqdownloader.apk'
  #       if self.last_devices_list is not None:
  #         for one in self.last_devices_list:
  #           callback = CallbackObject(self, command, one['serial_no'], package_name)
  #           install = adbtool.install.Command(one['serial_no'], package_name, apk_path, callback.CallbackSucc, callback.CallbackFail)
  #           self.log.info('before ProcessInstallApk2 ' + one['serial_no'] + ' : ' + apk_path)
  #           self.SendCommandProgress(command, consts.ERROR_CODE_OK,
  #                                    [one['serial_no'].encode('utf-8'), '开始', package_name, '', ])
  #           install.Execute()
  #           self.log.info('end ProcessInstallApk2')
  #           break
  #     else:
  #       pass
  #
  #   except Exception as e:
  #     pass
    

  # def ProcessInstallApk(self, command):
  #   try:
  #     if command.param[0] == 'all':
  #       succ, device_list = self.device.ListDevices()
  #       if succ:
  #         for one in device_list:
  #           self.device.ConnectDevice(one)
  #           callback = CallbackObject(command, one)
  #           self.device.Install(command.param[1], callback.Callback)
  #     elif command.param[0] == 'default':
  #       self.device.ConnectDevice()
  #       callback = CallbackObject(command, 'default')
  #       self.device.Install(command.param[1], callback.Callback)
  #     else:
  #       self.device.ConnectDevice(command.param[0])
  #       callback = CallbackObject(command, command.param[0])
  #       self.device.Install(command.param[1], callback.Callback)
  #
  #   except Exception as e:
  #     pass



  def ProcessGetLocalPackageList(self, command):
    try:
      local_prop = self.LoadLocalProp()
      
      apk_list = []
      if local_prop is not None:
        for one in local_prop['data']:
          one_apk = pb.apk_protomsg_pb2.OneApk()
          one_apk.md5 = one['md5']
          #packagename和apkname互换了。。。
          one_apk.packageName = one['packageName']
          one_apk.brief = one['introduction']
          one_apk.apk_name = one['packageName']
          one_apk.price = one['price']
          one_apk.type = consts.PACKAGE_BOTH
          one_apk.package_size = util.utility.GetFileSize(self.prop['apkPath'] + '/' + one['packageName'] + '.apk')
          apk_list.append(one_apk)

        # for one in local_prop['data']['remove']:
        #   one_apk = pb.apk_protomsg_pb2.OneApk()
        #   one_apk.md5 = ''.decode('utf-8')
        #   one_apk.name = one['packageName']
        #   one_apk.brief = ''.decode('utf-8')
        #   one_apk.apk_name = one['packageName']
        #   one_apk.price = one['price']
        #   one_apk.type = consts.PACKAGE_REMOVE
        #   one_apk.package_size = 0
        #   apk_list.append(one_apk)


        # self.device.UpdateApkList(apk_list)

        self.SendApkList(command,
                         consts.ERROR_CODE_OK, apk_list)
      else:
        self.SendApkList(command,
                         consts.ERROR_CODE_LOAD_LOCAL_APKLIST_FAILED)
    except Exception as e:
      self.SendCommandResponse(command, consts.ERROR_CODE_UNKNOWN, ['获取本地包列表', ])


  def ProcessRemoveLocalPackageList(self, command):
    try:
      if os.path.exists(self.prop['localPath']):
        os.remove(self.prop['localPath'])
      self.SendCommandResponse(command,
                               consts.ERROR_CODE_OK,
                               ['删除配置文件成功', ])
    except Exception as e:
      self.SendCommandResponse(command,
                               consts.ERROR_CODE_REMOVE_JSON_FILE_FAILED,
                               [consts.error_string(consts.ERROR_CODE_REMOVE_JSON_FILE_FAILED), ])

    try:
      if os.path.isdir(self.prop['apkPath']):
        util.utility.RemoveDir(self.prop['apkPath'])
      self.SendCommandResponse(command,
                               consts.ERROR_CODE_OK,
                               ['删除包目录成功', ])
    except Exception as e:
      self.SendCommandResponse(command,
                               consts.ERROR_CODE_REMOVE_APK_DIR_FAILED,
                               [consts.error_string(
                                 consts.ERROR_CODE_REMOVE_APK_DIR_FAILED), ])

    try:
      #无论如何，最后要把目录再创建下
      util.utility.CreateDir(self.prop['apkPath'])
    except Exception as e:
      pass


  def ProcessCheckUpdatePackageList(self, command):
    re, data = self.PullJsonFile()
    if re == consts.ERROR_CODE_OK:
      self.CheckUpdate(data, command)
      pass
    else:
      #失败了，通知c++侧
      self.SendCommandResponse(command, re, [consts.error_string(re), ])


  
  def CheckUpdate(self, data, command):
    self.SendCommandResponse(command, consts.ERROR_CODE_OK, ['配置更新', '获取网络配置成功', ])
    try:
      util.utility.CreateDir(self.prop['apkPath'])
      if self.ParseJson(data):
        self.SendCommandResponse(command, consts.ERROR_CODE_OK, ['配置更新', '解析网络配置成功', ])
        need_save_json = False
        if self.local_prop is not None:
          #检查两个文件的区别，并下载最新apk，完成md5校验。之后，更新配置文件到本地
          old_package_name_set = set()
          for old_one in self.local_prop['data']:
            old_package_name_set.add(old_one['packageName'])
          new_package_name_set = set()
          for new_one in data['data']:
            new_package_name_set.add(new_one['packageName'])
            
          diff_set = new_package_name_set - old_package_name_set
          solved_package_name = set()
          for new_one in data['data']:
            for old_one in self.local_prop['data']:
              #1 如果包名在差集合，说明是新增，需要下载
              # 2 如果包名不在差集，说明不是新增，但md5不一致，说明换包了，需要下载
              #3 如果包名不在差集，说明不是新增，md5一致，但是本地没文件，需要下载
              file_path = self.prop['apkPath'] + '/' + new_one['packageName'] + '.apk'

              if new_one['packageName'] in diff_set or \
                  (new_one['packageName'] == old_one['packageName'] and new_one['md5'] != old_one['md5']) or \
                        (new_one['packageName'] == old_one['packageName'] and os.path.exists(file_path) == False):
                need_save_json = True
                if new_one['packageName'] not in solved_package_name and self.DownloadOneApk(new_one, command):
                  solved_package_name.add(new_one['packageName'])

          
        else:
          #不存在本地配置
          need_save_json = True
          for new_one in data['data']:
            self.DownloadOneApk(new_one, command)
            
        
        
        if need_save_json == True:
          #保存最新的json文件到本地
          if util.utility.WriteJsonFile(self.prop['localPath'], data):
            self.SendCommandResponse(command, consts.ERROR_CODE_OK, ['配置更新', '保存配置文件成功', ])
          else:
            self.SendCommandResponse(command, consts.ERROR_CODE_SAVE_JSON_FILE_FAILED, ['配置更新', consts.error_string(response.code), ])
        else:
          self.SendCommandResponse(command, consts.ERROR_CODE_OK, ['配置更新', '无需更新配置文件', ])
          pass

      else:
        self.SendCommandResponse(command, consts.ERROR_CODE_PARSE_JSON_FAILED, ['配置更新', '解析网络配置失败', ])
        return

    except Exception as e:
      self.SendCommandResponse(command, consts.ERROR_CODE_UNKNOWN, ['检查更新', ])
  
    return
    pass
  


  # def SendCommandProgress(self, cmd, code, info=None):
  #   response = pb.apk_protomsg_pb2.CommandProgress()
  #   response.cmd = cmd.cmd
  #   response.cmd_no = cmd.cmd_no
  #   response.code = code
  #   if info is not None:
  #     for one in info:
  #       response.info.append(one.decode('utf-8'))
  #
  #   self.Send(response)


  def SendCommandResponse(self, cmd, code, info=None):
    response = pb.apk_protomsg_pb2.CommandResponse()
    response.cmd = cmd.cmd
    response.cmd_no = cmd.cmd_no
    response.code = code
    if info is not None:
      for one in info:
        response.info.append(one.decode('utf-8'))

    self.Send(response)


  def SendApkList(self, cmd, code, package_list=None):
    response = pb.apk_protomsg_pb2.ApkList()
    response.head.cmd = cmd.cmd
    response.head.cmd_no = cmd.cmd_no
    response.head.code = code
    if package_list is not None:
      for one in package_list:
        one_apk = response.apk_list.add()
        one_apk.md5 = one.md5
        one_apk.name = one.name
        one_apk.brief = one.brief
        one_apk.apk_name = one.apk_name
        one_apk.price = one.price
        one_apk.type = one.type
        one_apk.package_size = one.package_size

    self.Send(response)
    
    
    
  def SendDevicesList(self, cmd, code, device_list=None, info=None):
    response = pb.apk_protomsg_pb2.DevicesList()
    response.head.cmd = cmd.cmd
    response.head.cmd_no = cmd.cmd_no
    response.head.code = code
    if device_list is not None:
      for one in device_list:
        one_device = response.devices_list.add()
        try:
          one_device.serial_no = one['serial_no']
          one_device.product = one['product']
          one_device.model = one['model']
          one_device.device = one['device']
        except Exception as e:
          pass

    if info is not None:
      response.head.info.append(info)

    self.Send(response)



  def DownloadFile(self, command, one, file_path, callback):
    url = one['downUrl']
    
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
                                one['packageName']])
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
        data = ("%.2f%%" % (percent *100))
        self.host.SendCommandResponse(self.command, consts.ERROR_CODE_OK, ['包更新', '下载中', self.one['packageName'], data])
      pass
 
  
  
  
  
  def DownloadOneApk(self, one, command):
    result = False
    file_path = self.prop['apkPath'] + '/' + one['packageName'] + '.apk'

    #检查文件是否存在
    if os.path.exists(file_path):
      #文件存在判断md5是否匹配
      md5 = util.utility.GetFileMD5(file_path)
      if md5 != one['md5']:
        os.remove(file_path)
      else:
        self.SendCommandResponse(command, consts.ERROR_CODE_OK, ['包更新', '存在，无需下载', one['packageName']])
        result = True
        return result


    #开始下载
    self.SendCommandResponse(command, consts.ERROR_CODE_OK, ['包更新', '开始下载', one['packageName']])
    
    
    
    
    try:
      dc = CheckUpdateApkList.DownloadCallbck(command, one, self)
      ret = self.DownloadFile(command, one, file_path, dc.callback_progress)
      if ret:
        md5 = util.utility.GetFileMD5(file_path)
        if md5 != one['md5']:
          # 删除文件
          os.remove(file_path)
          self.SendCommandResponse(command, consts.ERROR_CODE_MD5_APK_FAILED,
                                   ['包更新', consts.error_string(consts.ERROR_CODE_MD5_APK_FAILED),
                                    one['packageName']])
        else:
          self.SendCommandResponse(command, consts.ERROR_CODE_OK,
                                   ['包更新',
                                    '下载成功',
                                    one['packageName']])
          result = True
      
    except Exception as e:
      exstr = traceback.format_exc()
      print(exstr)
      self.log.info(exstr)
      
      self.SendCommandResponse(command, consts.ERROR_CODE_SAVE_APK_FILE_FAILED,
                               ['包更新',
                                consts.error_string(consts.ERROR_CODE_SAVE_APK_FILE_FAILED),
                                one['packageName']])
      
    
    return result
         
    
    
  
  #//检查json是否符合要求，该有的字段都有效，都存在
  def ParseJson(self, data):
    """
    {"code": 0,
     "msg": "",
     "data":
       {"install":
          [{"id": 2, "downUrl": "http:\/\/s0qrdt.kdndj.com\/files\/d5e4ce46458ce2c4940b8702cefa9e44.apk",
            "md5": "d3edb026aa97b5f429b6da1ebe746005",
            "packageName": "\u817e\u8baf\u5e94\u7528\u5b9d",
            "brief": "\u4e3a\u667a\u80fd\u624b\u673a\u7528\u6237\u6253\u9020\u7684\u4e00\u4e2a\u624b\u673a\u5e94\u7528\u83b7\u53d6\u5e73\u53f0",
            "orderid": 1,
            "updated_at": "2018-10-24 17:21:44",
            "created_at": "2018-10-24 11:38:28",
            "apkName": "com.tencent.android.qqdownloader",
            "price": "2.00"}
    """
    result = False
    try:
      if data['code'] == 0:
        for one in data['data']:
          if 'apkName' not in one or 'downUrl'  not in one or 'md5' not in one or 'packageName' not in one or 'price' not in one:
            return False

        # for one in data['data']['remove']:
        #   if 'apkName'  not in one:
        #     return False

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

#======================================================
if __name__ == '__main__':
  msg_queue = Queue.Queue()
  msg_queue2 = Queue.Queue()
  checkUpdate = CheckUpdateApkList('ctp', msg_queue2, msg_queue)
  checkUpdate.CheckPackageList()