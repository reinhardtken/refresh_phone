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



import util.log
import util.network
import util.utility
import device.callback
import consts
import pb.apk_protomsg_pb2
import my_globals
import my_token
import config


#########################################################
class Master(object):
  def __init__(self, queue_out, pool):
    self.queue_out = queue_out
    self.queue_in = Queue.Queue()

    self.log = util.log.GetLogger(self.__class__.__name__)

    self.thread = threading.Thread(target=self.Work)
    self._continue = True
    self.thread.setDaemon(True)

    # self.pool = futures.ThreadPoolExecutor(5, 'net')
    self.pool = pool
    
    
    self._apkPath = None
    self._jsonPath = None
    self._localProp = None


  @property
  def apkPath(self):
    return self._apkPath
  
  @apkPath.setter
  def apkPath(self, value):
    self._apkPath = value

  @property
  def jsonPath(self):
    return self._jsonPath

  @jsonPath.setter
  def jsonPath(self, value):
    self._jsonPath = value

  @property
  def localProp(self):
    return self._localProp

  @localProp.setter
  def localProp(self, value):
    self._localProp = value
    


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
      time.sleep(1)
  
  
  def Join(self):
    self.thread.join()

#########################################################
  def ProcessCheckUpdate(self, command, relay=None):
    util.utility.Task.Post(self.queue_in,
                           functools.partial(
                             self.CheckUpdate, command, relay))


  def ProcessReport(self, command):
    if command['c'] == consts.COMMAND_NET_REPORT_DEVICE_INFO:
      util.utility.Task.Post(self.queue_in,
                             functools.partial(
                               lambda command, task, result: self.pool.submit(
                                 Master.ProcessReportDeviceInfo, command),
                               command))
    elif command['c'] == consts.COMMAND_NET_REPORT_INSTALL_APK:
      util.utility.Task.Post(self.queue_in,
                             functools.partial(
                               lambda command, task, result: self.pool.submit(
                                 Master.ProcessReportInstallApk, command),
                               command))
#########################################################
  @util.utility.tryWrapper
  def ProcessIncome(self, command):
    if isinstance(command, util.utility.Task.CallObject):
      command.CallOnThisThread()
    elif isinstance(command, dict):
      pass
      
      
      
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
    file_path = self._apkPath + '/' + one['packageName'] + '.apk'
  
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
      
        return True
  
    except Exception as e:
      print(e)
  
    return result
      


#########################################################
  #检查网络配置，更新apk文件
  # 1 获取配置文件
  def CheckUpdateStep1(self, task, command):
    future = self.pool.submit(Master.PullJsonFile)
    task.AddDoneCallbackHelper(future, self.queue_in, functools.partial(self.CheckUpdateStep2, command))

  # 2 解析json，对没有的apk，安排下载任务
  @util.utility.tryWrapper
  def CheckUpdateStep2(self, command, task, result):
    util.utility.CreateDir(self.apkPath)
    if not result[0] and self.ParseJson(result[1]):
      #获取网络配置文件成功
      data = result[1]
      self.SendCommandResponse(command, consts.ERROR_CODE_OK, ['配置更新', '解析网络配置成功', ])
      solved_package_name = set()
      if self._localProp is not None:
        # 检查两个文件的区别，并下载最新apk，完成md5校验。之后，更新配置文件到本地
        old_package_name_set = set()
        for old_one in self._localProp['data']:
          old_package_name_set.add(old_one['packageName'])
        new_package_name_set = set()
        for new_one in data['data']:
          new_package_name_set.add(new_one['packageName'])
      
        diff_set = new_package_name_set - old_package_name_set
      
        for new_one in data['data']:
          for old_one in self._localProp['data']:
            # 1 如果包名在差集合，说明是新增，需要下载
            # 2 如果包名不在差集，说明不是新增，但md5不一致，说明换包了，需要下载
            # 3 如果包名不在差集，说明不是新增，md5一致，但是本地没文件，需要下载
            file_path = self._apkPath + '/' + new_one['packageName'] + '.apk'
          
            if new_one['packageName'] in diff_set or \
                (new_one['packageName'] == old_one['packageName'] and new_one['md5'].upper() != old_one[
                  'md5'].upper()) or \
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
            task.AddDoneCallbackGroupHelper(future, self.queue_in, 'downloadApk', tmpCallback)
 
      
        task.AddGroupCallback(self.queue_in, 'downloadApk', functools.partial(self.CheckUpdateStep3, command, data))
      else:
        # 不需要下载包
        self.CheckUpdateStep3(self, command, data, task, None)
    else:
      # 没有成功解析网络配置
      self.CheckUpdateStepFinal(command, task, False)

  # 3 所有文件下载完成，存储配置文件
  # 保存最新的json文件到本地
  def CheckUpdateStep3(self, command, data, task, result):
    print('all file has download')
    
    step4 = self.pool.submit(functools.partial(util.utility.WriteJsonFile, self.jsonPath, data))
    task.AddDoneCallbackHelper(step4, self.queue_in, functools.partial(Master.CheckUpdateStepFinal, self, command))




  def CheckUpdateStepFinal(self, command, task, result):
    if result:
      self.SendCommandResponse(command, consts.ERROR_CODE_OK, ['配置更新', '保存配置文件成功', ])
    else:
      self.SendCommandResponse(command, consts.ERROR_CODE_SAVE_JSON_FILE_FAILED,
                               ['配置更新', '保存配置文件失败', ])

    task.TriggerFinal(result)
    
    
  def CheckUpdate(self, command, future=None, task=None, result=None):
    assert util.utility.CheckThread.Valid(self.thread)
    self.SendCommandResponse(command, consts.ERROR_CODE_OK, ['配置更新', '开始获取网络配置', ])
    task = util.utility.Task(self.pool)
    self.CheckUpdateStep1(task, command)
    
    
    
    if future is not None:
      final = task.GenFinal()
      futures.wait([final], return_when=futures.ALL_COMPLETED)
      print('CheckUpdate finished...., call future now')
      future.set_result(final.result())
    
          

    
    
#########################################################
        
            

  

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
  def PullJsonFile():
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

  pool = futures.ThreadPoolExecutor(5, 'net')
  master = Master(Queue.Queue(), pool)

  command = {}
  command['c'] = consts.COMMAND_NET_REPORT_DEVICE_INFO
  master.ProcessReport(command)
  
  
  master.apkPath = r'C:\workspace\code\chromium24\src\build\release\ctp_data\apk'
  master.jsonPath = r'C:\workspace\code\chromium24\src\build\release\ctp_data\apk_local'
  config.URL = r'http://es.kdndj.com'
  #config.URL = r'https://apkins.yfbro.com'
  check = pb.apk_protomsg_pb2.Command()
  check.cmd = consts.COMMAND_CHECK_UPDATE
  check.cmd_no = -1
  master.Start()
  master.CheckUpdate(check, futures.Future())
  master.Join()
  pass