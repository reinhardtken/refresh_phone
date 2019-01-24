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
import functools
import json
import os
from concurrent import futures
#########################################################
if __name__ == '__main__':
  import sys
  sys.path.append(r'C:\workspace\code\chromium24\src\phone\python\ctp_py')
#########################################################
# ====================================


import util.log
import util.network
import device.callback
import consts
import pb.apk_protomsg_pb2
import my_globals
import my_token
import config
# ====================================

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
    self.tmp_defferd = None
    
    self.pool = futures.ThreadPoolExecutor(5, 'net')
  

  
    
  def ProcessIncome(self, command):
    if isinstance(command, util.utility.Task.CallObject):
      command.CallOnThisThread()
    elif isinstance(command, dict):
      if command['c'] == consts.COMMAND_NET_REPORT_DEVICE_INFO:
        self.ProcessReportDeviceInfo(command)
      elif command['c'] == consts.COMMAND_NET_REPORT_INSTALL_APK:
        self.ProcessReportInstallApk(command)
    elif command[0].cmd == consts.COMMAND_CHECK_UPDATE:
      self.tmp_defferd = command[1]
      self.ProcessInnerCheckUpdate(command[0])
    
        

    
      

        

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
      # self.CheckTimeout()
      # self.TriggerAutoInstall()
      # self.CheckPendingTask()
      time.sleep(1)
  
  
  def Join(self):
    self.thread.join()
  
      
      
#########################################################
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
        self.host.SendCommandResponse(self.command, consts.ERROR_CODE_OK, ['包更新', '下载中', self.one['packageName'], data])
      pass

  def DownloadOneApk(self, one, command):
    result = [False, one['packageName']]
    file_path = self.apk_path + '/' + one['packageName'] + '.apk'
  
    # 检查文件是否存在
    if os.path.exists(file_path):
      # 文件存在判断md5是否匹配
      md5 = util.utility.GetFileMD5(file_path)
      if md5.upper() != one['md5'].upper():
        os.remove(file_path)
      else:
        self.SendCommandResponse(command, consts.ERROR_CODE_OK, ['包更新', '存在，无需下载', one['packageName']])
        result[0] = True
        return result
  
    # 开始下载
    self.SendCommandResponse(command, consts.ERROR_CODE_OK, ['包更新', '开始下载', one['packageName']])
  
    try:
      dc = Master.DownloadCallbck(command, one, self)
      ret = util.network.DownloadFile(one['downUrl'], file_path, dc.callback_progress)
      if ret[0]:
        md5 = util.utility.GetFileMD5(file_path)
        if md5.upper() != one['md5'].upper():
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
          result[0] = True
  
    except Exception as e:
      exstr = traceback.format_exc()
      print(exstr)
      self.log.info(exstr)
    
      self.SendCommandResponse(command, consts.ERROR_CODE_SAVE_APK_FILE_FAILED,
                               ['包更新',
                                consts.error_string(consts.ERROR_CODE_SAVE_APK_FILE_FAILED),
                                one['packageName']])
  
    return result

  # //检查json是否符合要求，该有的字段都有效，都存在
  @staticmethod
  def ParseJson(data):
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
            
    2019-01-19
    u'apkName' (93001968) = {unicode} u'开心消消乐'
    u'createtime' (93002064) = {unicode} u'2018-11-30 17:25:46'
    u'downUrl' (93002088) = {unicode} u'https://s0qrdt.kdndj.com/files/com.happyelements.AndroidAnimal.ad.1.63.he_ad.apk'
    u'id' (92978736) = {int} 10
    u'introduction' (93002160) = {unicode} u'一款乐元素研发的一款三消类休闲游戏'
    u'md5' (93002256) = {unicode} u'86d42c65f3089f8c8892b2d321ab6596'
    u'packageName' (93001104) = {unicode} u'com.happyelements.AndroidAnimal.ad'
    u'price' (93002328) = {float} 1.8
    u'version' (93002304) = {unicode} u'5.0'
    """
    result = False
    try:
      if data['code'] == 200:
        for one in data['data']:
          if 'apkName' not in one or 'downUrl' not in one or 'md5' not in one or 'packageName' not in one or 'price' not in one or 'id' not in one or 'introduction' not in one:
            return False
      
        # for one in data['data']['remove']:
        #   if 'apkName' not in one:
        #     return False
      
        return True
  
    except Exception as e:
      print(e)
  
    return result
  
  
  # @staticmethod
  # def PullJsonFile():
  #   #URL = 'https://www.ppndj.com/jc/user/apkinstall?ch=1234'
  #   # URL = 'https://www.xppndj.com/jc/user/apkinstall?ch=1234'
  #   try:
  #     url = config.URL + '/api/applist'
  #
  #     headers = {
  #       'User-Agent': 'Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.132 Safari/537.36',
  #       'X-atoken-Authorization': my_token.token.Get(),
  #     }
  #
  #     try:
  #       timeout = (10, 180)
  #       response = requests.request("get", url, stream=True, data=None, headers=headers, timeout=timeout)
  #     except requests.exceptions.SSLError as e:
  #       response = requests.request("get", url, stream=True, data=None, headers=headers, verify=False, timeout=timeout)
  #
  #     if response.status_code == 200:
  #       try:
  #         json_data = json.loads(response.content)
  #         if json_data['code'] == 200:
  #           return (consts.ERROR_CODE_OK, json_data)
  #       except Exception as e:
  #         print(e)
  #         return (consts.ERROR_CODE_PARSE_JSON_FAILED, None)
  #     else:
  #       print('net Error', response.status_code)
  #   except requests.ConnectionError as e:
  #     print('Error', e.args)
  #   except Exception as e:
  #     print('Error', e.args)
  #   return (consts.ERROR_CODE_PULL_JSON_FAILED, None)



  def ProcessCheckUpdate(self, command, deferred=None):
    self.queue_in.put((command, deferred))
    
    
  def ProcessReport(self, command):
    self.queue_in.put(command)
    
  
  def CallbackDeferred(self):
    if self.tmp_defferd is not None:
      self.tmp_defferd.callback()
      self.tmp_defferd = None
    
  
  def ProcessInnerCheckUpdate(self, command):
    self.CheckUpdate2(command)
  
  
  

  # def CheckUpdate(self, data, command):
  #   self.SendCommandResponse(command, consts.ERROR_CODE_OK, ['配置更新', '获取网络配置成功', ])
  #   try:
  #     util.utility.CreateDir(self.apk_path)
  #     if self.ParseJson(data):
  #       self.SendCommandResponse(command, consts.ERROR_CODE_OK, ['配置更新', '解析网络配置成功', ])
  #       need_save_json = False
  #       if self.local_prop is not None:
  #         # 检查两个文件的区别，并下载最新apk，完成md5校验。之后，更新配置文件到本地
  #         old_package_name_set = set()
  #         for old_one in self.local_prop['data']:
  #           old_package_name_set.add(old_one['packageName'])
  #         new_package_name_set = set()
  #         for new_one in data['data']:
  #           new_package_name_set.add(new_one['packageName'])
  #
  #         diff_set = new_package_name_set - old_package_name_set
  #         solved_package_name = set()
  #         for new_one in data['data']:
  #           for old_one in self.local_prop['data']:
  #             # 1 如果包名在差集合，说明是新增，需要下载
  #             # 2 如果包名不在差集，说明不是新增，但md5不一致，说明换包了，需要下载
  #             # 3 如果包名不在差集，说明不是新增，md5一致，但是本地没文件，需要下载
  #             file_path = self.apk_path + '/' + new_one['packageName'] + '.apk'
  #
  #             if new_one['packageName'] in diff_set or \
  #                 (new_one['packageName'] == old_one['packageName'] and new_one['md5'] != old_one['md5']) or \
  #                 (new_one['packageName'] == old_one['packageName'] and os.path.exists(file_path) is False):
  #               need_save_json = True
  #               if new_one['packageName'] not in solved_package_name and self.DownloadOneApk(new_one, command):
  #                 solved_package_name.add(new_one['packageName'])
  #
  #
  #       else:
  #         # 不存在本地配置
  #         need_save_json = True
  #         for new_one in data['data']:
  #           self.DownloadOneApk(new_one, command)
  #
  #       if need_save_json == True:
  #         # 保存最新的json文件到本地
  #         if util.utility.WriteJsonFile(self.json_path, data):
  #           self.SendCommandResponse(command, consts.ERROR_CODE_OK, ['配置更新', '保存配置文件成功', ])
  #         else:
  #           self.SendCommandResponse(command, consts.ERROR_CODE_SAVE_JSON_FILE_FAILED,
  #                                    ['配置更新', '保存配置文件失败', ])
  #       else:
  #         self.SendCommandResponse(command, consts.ERROR_CODE_OK, ['配置更新', '无需更新配置文件', ])
  #         pass
  #
  #     else:
  #       self.SendCommandResponse(command, consts.ERROR_CODE_PARSE_JSON_FAILED, ['配置更新', '解析网络配置失败', ])
  #       return
  #
  #   except Exception as e:
  #     self.SendCommandResponse(command, consts.ERROR_CODE_UNKNOWN, ['检查更新', ])
  #     exstr = traceback.format_exc()
  #     print(exstr)
  #     self.log.info(exstr)
  #
  #   return
  #   pass


  def CheckUpdate2(self, command):
    self.SendCommandResponse(command, consts.ERROR_CODE_OK, ['配置更新', '获取网络配置成功', ])
    task = util.utility.Task(self.pool)
    
    # 1 获取配置文件
    future = self.pool.submit(Master.PullJsonFile2)
    
    #2 解析json，对没有的apk，安排下载任务
    def Step2(apkPath, task, result):
      try:
        util.utility.CreateDir(apkPath)
        if not result[0] and self.ParseJson(result[1]):
          data = result[1]
          self.SendCommandResponse(command, consts.ERROR_CODE_OK, ['配置更新', '解析网络配置成功', ])
          need_save_json = False
          solved_package_name = set()
          if self.local_prop is not None:
            # 检查两个文件的区别，并下载最新apk，完成md5校验。之后，更新配置文件到本地
            old_package_name_set = set()
            for old_one in self.local_prop['data']:
              old_package_name_set.add(old_one['packageName'])
            new_package_name_set = set()
            for new_one in data['data']:
              new_package_name_set.add(new_one['packageName'])
      
            diff_set = new_package_name_set - old_package_name_set
      
            for new_one in data['data']:
              for old_one in self.local_prop['data']:
                # 1 如果包名在差集合，说明是新增，需要下载
                # 2 如果包名不在差集，说明不是新增，但md5不一致，说明换包了，需要下载
                # 3 如果包名不在差集，说明不是新增，md5一致，但是本地没文件，需要下载
                file_path = self.apk_path + '/' + new_one['packageName'] + '.apk'
          
                if new_one['packageName'] in diff_set or \
                    (new_one['packageName'] == old_one['packageName'] and new_one['md5'].upper() != old_one['md5'].upper()) or \
                    (new_one['packageName'] == old_one['packageName'] and os.path.exists(file_path) is False):
                  need_save_json = True
                  if new_one['packageName'] not in solved_package_name:
                    solved_package_name.add(new_one['packageName'])
          else:
            for new_one in data['data']:
              solved_package_name.add(new_one['packageName'])
    

          def tmpCallback(task, result):
            print('tmpCallback', result)
    
          # def allDoneCallback(task, result):
          #   print('allDoneCallback', result)
          if len(solved_package_name):
            for new_one in data['data']:
              if new_one['packageName'] in solved_package_name:
                future = task.pool.submit(Master.DownloadOneApk, self, new_one, command)
                callback = task.GenGroupCallObject(self.queue_in, 'downloadApk', tmpCallback)
                future.add_done_callback(callback.Callback)
      
            task.AddGroupCallback(self.queue_in, 'downloadApk', functools.partial(Step3, self, data))
          else:
            #不需要下载包
            Step3(self, data, None, None)
        else:
          #没有成功解析网络配置
          self.CallbackDeferred()
          
      except Exception as e:
        exstr = traceback.format_exc()
        print(exstr)
        print(e)
          

    callback = task.GenCallObject(self.queue_in, functools.partial(Step2, self.apk_path))
    future.add_done_callback(callback.Callback)
    #
    def Step3(self, data, task, result):
      print('all file has download')
      #所有文件下载完成，存储配置文件
      # 保存最新的json文件到本地
      if util.utility.WriteJsonFile(self.json_path, data):
        self.SendCommandResponse(command, consts.ERROR_CODE_OK, ['配置更新', '保存配置文件成功', ])
      else:
        self.SendCommandResponse(command, consts.ERROR_CODE_SAVE_JSON_FILE_FAILED,
                                 ['配置更新', '保存配置文件失败', ])

      self.CallbackDeferred()
        
            
        
        
        
        


  
  

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
    
    
  @staticmethod
  def ProcessReportDeviceInfo(command):
    try:
      url = config.URL + '/api/add/equipment'
    
      headers = {
        'User-Agent': 'Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.132 Safari/537.36',
        'X-atoken-Authorization': my_token.token.Get(),
      }
      # params = {
      #   'mobile': command.param[0].encode('utf-8')
      # }
      data = {
        'product': command['product'].encode('utf-8'),
        'model': command['model'].encode('utf-8'),
        'device': command['device'].encode('utf-8'),
        'imei': command['imei'].encode('utf-8'),
        'serialNum': command['serial_number'].encode('utf-8'),
        'applist': '{}',
      }
    
      timeout = (10, 180)
      log = util.log.GetCurrentLogger()
      log.info('ProcessReportDeviceInfo')
      log.info(url)
      log.info(data)
      try:
        response = requests.request("post", url, stream=True, data=data, headers=headers, timeout=timeout)
      except requests.exceptions.SSLError as e:
        response = requests.request("post", url, stream=True, data=data, headers=headers, verify=False, timeout=timeout)
        
      try:
        # 打点到自己的服务器
        # http://60.205.224.225/api/add/equipment
        myURL = 'http://112.74.182.109/api/add/equipment'
        r = requests.request("post", myURL, stream=True, data=data, headers=headers, timeout=timeout)
        if r.status_code == 200:
          pass
      except Exception as e:
        pass
    
      if response.status_code == 200:
        json_data = json.loads(response.content)
        if json_data['code'] == 200:
          log.info('success')
          return
        else:
          #my_token.token.Set(None)
          log.warning('failed code not 200: %d', json_data['code'])
          return
      else:
        log.warning('status code not 200: %d', response.status_code)
        return
  
  
    except requests.ConnectionError as e:
      log.warning('ConnectionError %s', str(e.args))
    except Exception as e:
      log.warning('Exception %s', str(e.args))




  @staticmethod
  def ProcessReportInstallApk(command):
    try:
      url = config.URL + '/api/install/apk'
    
      headers = {
        'User-Agent': 'Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.132 Safari/537.36',
        'X-atoken-Authorization': my_token.token.Get(),
      }
      # params = {
      #   'mobile': command.param[0].encode('utf-8')
      # }
      data = {
        'id': command['id'],
        'status': command['status'],
        'reason': command['reason'].encode('utf-8'),
        'imei': command['imei'].encode('utf-8'),
        'serialNum': command['serial_number'].encode('utf-8'),
      }
    
      timeout = (10, 180)
      log = util.log.GetCurrentLogger()
      log.info('ProcessReportInstallApk')
      log.info(url)
      log.info(data)
      try:
        response = requests.request("post", url, stream=True, data=data, headers=headers, timeout=timeout)
      except requests.exceptions.SSLError as e:
        response = requests.request("post", url, stream=True, data=data, headers=headers, verify=False, timeout=timeout)
        
      
      try:
        # 打点到自己的服务器
        # http://10.58.72.57:8099/api/install/apk
        myURL = 'http://112.74.182.109/api/install/apk'
        r = requests.request("post", myURL, stream=True, data=data, headers=headers, timeout=timeout)
        if r.status_code == 200:
          pass
      except Exception as e:
        pass
      
    
      if response.status_code == 200:
        json_data = json.loads(response.content)
        if json_data['code'] == 200:
          log.info('success')
          return
        else:
          #my_token.token.Set(None)
          log.warning('failed code not 200: %d', json_data['code'])
          return
      else:
        log.warning('status code not 200: %d', response.status_code)
        return

    except requests.ConnectionError as e:
      log.warning('ConnectionError %s', str(e.args))
    except Exception as e:
      log.warning('Exception %s', str(e.args))


#########################################################
  @staticmethod
  def PullJsonFile2():
    url = config.URL + '/api/applist'
  
    headers = {
      'User-Agent': 'Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.132 Safari/537.36',
      'X-atoken-Authorization': my_token.token.Get(),
    }
    result = util.network.Get(url, headers)
    if result[0]:
      try:
        json_data = json.loads(result[1])
        if json_data['code'] == 200:
          return (consts.ERROR_CODE_OK, json_data)
        else:
          return (consts.ERROR_CODE_PARSE_JSON_FAILED, None)
      except Exception as e:
        print(e)
        return (consts.ERROR_CODE_PARSE_JSON_FAILED, None)
    else:
      print('net Error', result[1])
      return (consts.ERROR_CODE_PULL_JSON_FAILED, None)
      

# ======================================
if __name__ == '__main__':
  command = {}
  command['product'] = '123'
  command['model'] = '123'
  command['device'] = '123'
  command['imei'] = '123'
  command['serial_number'] = '123'
  my_token.token.Init(r'C:\workspace\code\chromium24\src\build\release\ctp_data\token')
  #Master.ProcessReportDeviceInfo(command)
  import Queue
  master = Master(Queue.Queue())
  master.apk_path = r'C:\workspace\code\chromium24\src\build\release\ctp_data\apk'
  config.URL = r'http://es.kdndj.com'
  config.URL = r'https://apkins.yfbro.com'
  check = pb.apk_protomsg_pb2.Command()
  check.cmd = consts.COMMAND_CHECK_UPDATE
  check.cmd_no = -1
  master.Start()
  master.CheckUpdate2(check)
  master.Join()
  pass