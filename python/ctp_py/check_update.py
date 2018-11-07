#!/usr/bin/env python
# -*- coding: utf-8 -*-

import time
import datetime
import Queue
import os
import json
import sys

import requests
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
import adb_wrapper
#=======================================================



#======================================================================
class CheckUpdateApkList(util.thread_class.ThreadClass):
  ERROR_CODE_OK = 0
  ERROR_CODE_PULL_JSON_FAILED = 1
  ERROR_CODE_PARSE_JSON_FAILED = 2
  ERROR_CODE_DOWNLOAD_APK_FAILED = 3
  ERROR_CODE_MD5_APK_FAILED = 4
  ERROR_CODE_SAVE_JSON_FILE_FAILED = 5
  ERROR_CODE_SAVE_APK_FILE_FAILED = 6

  ERROR_CODE_REMOVE_JSON_FILE_FAILED = 7
  ERROR_CODE_REMOVE_APK_DIR_FAILED = 8

  ERROR_CODE_LOAD_LOCAL_APKLIST_FAILED = 9


  ERROR_CODE_UNKNOWN = 100000
  

  ERROR_CODE_STRING = [
    '成功',#0
    '拉取配置文件失败',
    '解析配置文件失败',
    '下载apk文件失败',
    '校验apk文件失败',
    '保存配置文件失败',
    '保存apk文件失败',
    '删除配置文件失败',
    '删除apk目录失败',
    '加载本地配置文件失败'
  ]

  PACKAGE_INSTALL = 0#只安装
  PACKAGE_REMOVE = 1#只卸载
  PACKAGE_BOTH = 2#若存在，先卸载后安装

  @staticmethod
  def error_string(code):
    if len(CheckUpdateApkList.ERROR_CODE_STRING) > code:
      info = CheckUpdateApkList.ERROR_CODE_STRING[code]
    else:
      info = '未知错误'

    return info


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
    
    self.device = adb_wrapper.ADBWrapper()
    
    

  def Send(self, data):
    self.queue_out.put(data)


  def SendHandler(self, msg, result):
    data = pb.cp_comunication_pb2.BacktestingExpectAssetList()
    data.table_name = self.expectTableName
    data.type = msg.type
    data.level = msg.level
    data.total = self.expectCounter
    #data.index = index

    for v in result:
      try:
        data.data.append(v[2])
      except (TypeError):
        self.log.error('exception happen ========')
        pass
      
      
    #发送数据==
    self.log.info('before send================')
    self.Send(data)

  def RegisterHandler(self, src):
    # src.RegisterMessageHandlerWithName(self, 'ctp.mq.QueryLevelMQ')
    # src.RegisterMessageHandlerWithName(self, 'ctp.cp.BacktestingExpectAssetQuery')
    # src.RegisterMessageHandlerWithName(self, 'ctp.cp.BacktestingResultSave2DBRequery')
    src.RegisterMessageHandlerWithName(self, 'ctp.cp.Command')
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


  def Work(self):
    time.sleep(2)


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
      elif self.prop is not None:
        #只有完成了初始化才能响应业务请求
        if msg.cmd == 'check_net_package_list':
          self.ProcessCheckUpdatePackageList(msg)
        elif msg.cmd == 'remove_local_package_list':
          self.ProcessRemoveLocalPackageList(msg)
        elif msg.cmd == 'get_local_package_list':
          self.ProcessGetLocalPackageList(msg)
        elif msg.cmd == 'install_apk':
          self.ProcessInstallApk(msg)
        

  def GenInstallApkCallback(self, command):
    def InstallApkCallback(apk, now, total):
      progress = float(now) / total
      if (progress - self.last_progress) > 0.05 or now == total:
        self.last_progress = progress
        self.SendCommandProgress(command, CheckUpdateApkList.ERROR_CODE_OK, [apk, str(progress), ])
    
    return InstallApkCallback

  def ProcessInstallApk(self, command):
    self.last_progress = 0
    self.device.ConnectDevice()
    self.device.Install(command.param[0], self.GenInstallApkCallback(command))


  def ProcessGetLocalPackageList(self, command):
    try:
      local_prop = self.LoadLocalProp()
      apk_list = []
      if local_prop is not None:
        for one in local_prop['data']['install']:
          one_apk = pb.apk_protomsg_pb2.OneApk()
          one_apk.md5 = one['apkmd5']
          one_apk.name = one['name']
          one_apk.brief = one['brief']
          one_apk.apk_name = one['apkname']
          one_apk.price = one['price']
          one_apk.type = CheckUpdateApkList.PACKAGE_BOTH
          apk_list.append(one_apk)

        for one in local_prop['data']['remove']:
          one_apk = pb.apk_protomsg_pb2.OneApk()
          one_apk.md5 = ''.decode('utf-8')
          one_apk.name = one['name']
          one_apk.brief = ''.decode('utf-8')
          one_apk.apk_name = one['apkname']
          one_apk.price = one['price']
          one_apk.type = CheckUpdateApkList.PACKAGE_REMOVE
          apk_list.append(one_apk)

        self.SendApkList(command,
                         CheckUpdateApkList.ERROR_CODE_OK, apk_list)
      else:
        self.SendApkList(command,
                                 CheckUpdateApkList.ERROR_CODE_LOAD_LOCAL_APKLIST_FAILED)
    except Exception as e:
      self.SendCommandProgress(command, CheckUpdateApkList.ERROR_CODE_UNKNOWN, ['获取本地包列表', ])


  def ProcessRemoveLocalPackageList(self, command):
    try:
      if os.path.exists(self.prop['localPath']):
        os.remove(self.prop['localPath'])
      self.SendCommandProgress(command,
                               CheckUpdateApkList.ERROR_CODE_OK,
                               ['删除配置文件成功', ])
    except Exception as e:
      self.SendCommandProgress(command,
                               CheckUpdateApkList.ERROR_CODE_REMOVE_JSON_FILE_FAILED,
                               [CheckUpdateApkList.error_string(CheckUpdateApkList.ERROR_CODE_REMOVE_JSON_FILE_FAILED), ])

    try:
      if os.path.isdir(self.prop['apkPath']):
        util.utility.RemoveDir(self.prop['apkPath'])
      self.SendCommandProgress(command,
                               CheckUpdateApkList.ERROR_CODE_OK,
                               ['删除包目录成功', ])
    except Exception as e:
      self.SendCommandProgress(command,
                               CheckUpdateApkList.ERROR_CODE_REMOVE_APK_DIR_FAILED,
                               [CheckUpdateApkList.error_string(
                                 CheckUpdateApkList.ERROR_CODE_REMOVE_APK_DIR_FAILED), ])

    try:
      #无论如何，最后要把目录再创建下
      util.utility.CreateDir(self.prop['apkPath'])
    except Exception as e:
      pass


  def ProcessCheckUpdatePackageList(self, command):
    re, data = self.PullJsonFile()
    if re == CheckUpdateApkList.ERROR_CODE_OK:
      self.CheckUpdate(data, command)
      pass
    else:
      #失败了，通知c++侧
      self.SendCommandResponse(command, re, [CheckUpdateApkList.error_string(re), ])


  
  def CheckUpdate(self, data, command):
    self.SendCommandProgress(command, CheckUpdateApkList.ERROR_CODE_OK, ['配置更新', '获取网络配置成功', ])
    try:
      util.utility.CreateDir(self.prop['apkPath'])
      if self.ParseJson(data):
        self.SendCommandProgress(command, CheckUpdateApkList.ERROR_CODE_OK, ['配置更新', '解析网络配置成功', ])
        need_save_json = False
        if self.local_prop is not None:
          #检查两个文件的区别，并下载最新apk，完成md5校验。之后，更新配置文件到本地
          for new_one in data['data']['install']:
            for old_one in self.local_prop['data']['install']:
              if new_one['apkmd5'] != old_one['apkmd5']:
                need_save_json = True
                self.DownloadOneApk(new_one, command)

          
        else:
          #不存在本地配置
          need_save_json = True
          for new_one in data['data']['install']:
            self.DownloadOneApk(new_one, command)
            
        
        
        if need_save_json == True:
          #保存最新的json文件到本地
          if util.utility.WriteJsonFile(self.prop['localPath'], data):
            self.SendCommandProgress(command, CheckUpdateApkList.ERROR_CODE_OK, ['配置更新', '保存配置文件成功', ])
          else:
            self.SendCommandProgress(command, CheckUpdateApkList.ERROR_CODE_SAVE_JSON_FILE_FAILED, ['配置更新', CheckUpdateApkList.error_string(response.code), ])
        else:
          self.SendCommandProgress(command, CheckUpdateApkList.ERROR_CODE_OK, ['配置更新', '无需更新配置文件', ])
          pass

      else:
        self.SendCommandProgress(command, CheckUpdateApkList.ERROR_CODE_PARSE_JSON_FAILED, ['配置更新', '解析网络配置失败', ])
        return

    except Exception as e:
      self.SendCommandProgress(command, CheckUpdateApkList.ERROR_CODE_UNKNOWN, ['检查更新', ])
  
    return
    pass
  


  def SendCommandProgress(self, cmd, code, info=None):
    response = pb.apk_protomsg_pb2.CommandProgress()
    response.cmd = cmd.cmd
    response.cmd_no = cmd.cmd_no
    response.code = code
    if info is not None:
      for one in info:
        response.info.append(one.decode('utf-8'))

    self.Send(response)


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

    self.Send(response)


  
  def DownloadOneApk(self, one, command):
    file_path = self.prop['apkPath'] + '/' + one['apkname'] + '.apk'

    #检查文件是否存在
    if os.path.exists(file_path):
      #文件存在判断md5是否匹配
      md5 = util.utility.GetFileMd5(file_path)
      if md5 != one['apkmd5']:
        #理论上这种情况不应该存在
        os.remove(file_path)
      else:
        self.SendCommandProgress(command, CheckUpdateApkList.ERROR_CODE_OK, ['包更新', '存在，无需下载', one['apkname']])
        return


    #开始下载
    self.SendCommandProgress(command, CheckUpdateApkList.ERROR_CODE_OK, ['包更新', '开始下载', one['apkname']])

    
    URL = one['apkurl']
    headers = {
      'User-Agent': 'Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.132 Safari/537.36',
    }
    s = requests.Session()
    s.mount('http://', requests.adapters.HTTPAdapter(max_retries=5))
    s.mount('https://', requests.adapters.HTTPAdapter(max_retries=5))
    try:
      r = s.get(URL, headers=headers, timeout=10)
      if r.status_code == 200:
        try:
          with open(file_path, "wb") as code:
            code.write(r.content)
          # 检查md5
          md5 = util.utility.GetFileMd5(file_path)
          if md5 != one['apkmd5']:
            #删除文件
            os.remove(file_path)
            self.SendCommandProgress(command, CheckUpdateApkList.ERROR_CODE_MD5_APK_FAILED,
                                     ['包更新', CheckUpdateApkList.error_string(CheckUpdateApkList.ERROR_CODE_MD5_APK_FAILED), one['apkname']])
            return
          else:
            self.SendCommandProgress(command, CheckUpdateApkList.ERROR_CODE_OK,
                                     ['包更新',
                                      '下载成功',
                                      one['apkname']])
            return
        except Exception as e:
          self.SendCommandProgress(command, CheckUpdateApkList.ERROR_CODE_SAVE_APK_FILE_FAILED,
                                   ['包更新',
                                    CheckUpdateApkList.error_string(CheckUpdateApkList.ERROR_CODE_SAVE_APK_FILE_FAILED),
                                    one['apkname']])
          return
      else:
        self.SendCommandProgress(command, CheckUpdateApkList.ERROR_CODE_DOWNLOAD_APK_FAILED,
                                 ['包更新',
                                  CheckUpdateApkList.error_string(CheckUpdateApkList.ERROR_CODE_DOWNLOAD_APK_FAILED),
                                  one['apkname']])
        return
    except requests.ConnectionError as e:
      self.SendCommandProgress(command, CheckUpdateApkList.ERROR_CODE_DOWNLOAD_APK_FAILED,
                               ['包更新',
                                CheckUpdateApkList.error_string(CheckUpdateApkList.ERROR_CODE_DOWNLOAD_APK_FAILED),
                                one['apkname']])
      return

    
    
  
  #//检查json是否符合要求，该有的字段都有效，都存在
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
          if 'apkname' not in one or 'apkurl'  not in one or 'apkmd5' not in one or 'name' not in one or 'price' not in one:
            return False

        for one in data['data']['remove']:
          if 'apkname'  not in one:
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
          return (CheckUpdateApkList.ERROR_CODE_OK, json_data)
        except Exception as e:
          print(e)
          return (CheckUpdateApkList.ERROR_CODE_PARSE_JSON_FAILED, None)
      else:
        print('net Error', r.status_code)
    except requests.ConnectionError as e:
      print('Error', e.args)
    return (CheckUpdateApkList.ERROR_CODE_PULL_JSON_FAILED, None)

#======================================================
if __name__ == '__main__':
  msg_queue = Queue.Queue()
  msg_queue2 = Queue.Queue()
  checkUpdate = CheckUpdateApkList('ctp', msg_queue2, msg_queue)
  checkUpdate.CheckPackageList()