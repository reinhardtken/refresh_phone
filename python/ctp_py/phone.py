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
# ====================================
import util.thread_class
import pb.mq_protomsg_pb2
import pb.cp_comunication_pb2
import pb.apk_protomsg_pb2
# import util
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
import my_globals
import net_logic.master
import defer
import my_token
# =======================================================

# ======================================================================
class PhoneLogic(util.thread_class.ThreadClass):

  
  def InitGlobalLogByFile(self, prop_file):
    if os.path.exists(prop_file):
      prop = util.utility.ReadJsonProp(prop_file)
      self.InitGlobalLogByParam(prop)
  
  def InitGlobalLogByParam(self, prop):
    util.prop.Init(prop)
    util.log.Init(prop)
  
  def __init__(self, prop_file, queue_in, queue_out):
    self.log = None
    if prop_file is not None and os.path.exists(prop_file):
      self.InitGlobalLogByFile(prop_file)
      prop = util.utility.ReadJsonProp(prop_file)
      self.prop = prop
    else:
      # prop是none，说明启动参水里面只有一个port，要等c++把配置信息通过socket传过来
      self.prop = None
    
    util.thread_class.ThreadClass.__init__(self, queue_in)
    self.queue_out = queue_out
    my_globals.queue_network = queue_out
    
    self.local_prop = None
    self.apk_dir = None
    
    # self.device = adb_wrapper.ADBWrapper()
    # self.device2 = adb_wrapper2.AdbInstall()
    self.last_devices_list = None

    # 网络拉包
    self.check_update = net_logic.master.Master(queue_out)
    self.check_update.Start()
    
    
    #设备管理
    self.master = device.master.Master(queue_out, self.check_update)
    self.master.Start()
    

    self.sub_command_id = 0
    
    self.package_map = {}
    
    
    self.total_auto_defer = None




  def _GenSubCommandID(self):
    self.sub_command_id += 1
    return self.sub_command_id
    
    
    
    
  
  def Send(self, data):
    self.queue_out.put(data)
  
  def RegisterHandler(self, src):
    src.RegisterMessageHandlerWithName(self, 'apk.Command')
  
  def LoadLocalProp(self):
    try:
      self.check_update.json_path = self.prop['localPath']
      tmp = util.utility.ReadJsonProp(self.prop['localPath'])
      if self.check_update.ParseJson(tmp):
        self.local_prop = tmp
        self.check_update.local_prop = tmp
        return self.local_prop
    except Exception as e:
      return None
  
  def Init2(self, prop):
    self.prop = prop
    self.Init()
  
  def Init(self):
    if self.prop is not None:
      self.log = util.log.GetLogger(self.__class__.__name__)
      self.log.info('PhoneLogic init')
      self.log.info(str(self.prop))
      try:
        self.apk_dir = self.prop['apkPath']
        my_globals.apk_dir = self.apk_dir
        self.LoadLocalProp()
      except Exception as e:
        pass
      
      self.debugPath = self.prop['debugPath']
      util.utility.CreateDir(self.prop['apkPath'])

      # 设置adbexe文件
      adbtool.base.AdbCommandBase.adb = self.prop['adb_exe']
  
  def Work(self):
    time.sleep(1)
  
  def Handle(self, msg):
    # print(msg)
    self.queue.put(msg)
  
  def Name(self):
    return 'ctp.mq.QueryLevelMQ'
  
  def DealMsg(self, msg):
    if self.log:
      self.log.info('DealMsg  ' + msg.DESCRIPTOR.full_name)
      self.log.info(msg.cmd)
      
    if msg.DESCRIPTOR.full_name == 'apk.Command':
      if msg.cmd == 'py_config':
        # 完成初始化
        if self.prop is None:
          self.InitGlobalLogByParam(config.CreateProp(msg))
          self.Init2(config.CreateProp(msg))
          # 先尝试把自己的adb server启动起来
          init = pb.apk_protomsg_pb2.Command()
          init.cmd = consts.COMMAND_INIT
          init.cmd_no = -1
          self.ProcessStartServer(init)
          self.ProcessInitToken()
      elif self.prop is not None:
        # 只有完成了初始化才能响应业务请求
        if msg.cmd == consts.COMMAND_CHECK_UPDATE:
          msg.sub_cmd_no = self._GenSubCommandID()
          self.ProcessCheckUpdatePackageList(msg)
        elif msg.cmd == 'remove_local_package_list':
          msg.sub_cmd_no = self._GenSubCommandID()
          self.ProcessRemoveLocalPackageList(msg)
        elif msg.cmd == consts.COMMAND_GET_LOCAL_PACKAGELIST:
          msg.sub_cmd_no = self._GenSubCommandID()
          self.ProcessGetLocalPackageList(msg)
        elif msg.cmd == consts.COMMAND_INSTALL_APK:
          self.ProcessInstallApk3(msg)
        elif msg.cmd == consts.COMMAND_SCAN_DEVICES:
          self.ProcessScanDevices3(msg)
        elif msg.cmd == consts.COMMAND_GET_PACKAGE_LIST:
          self.ProcessGetPackageList(msg)
        elif msg.cmd == consts.COMMAND_REFRESH:
          self.ProcessRefresh(msg)
        elif msg.cmd == consts.COMMAND_AUTO_INSTALL:
          self.ProcessAutoInstall(msg)
        elif msg.cmd == consts.COMMAND_TOTAL_AUTO_INSTALL:
          self.ProcessTotalAutoInstall(msg)
        elif msg.cmd == consts.COMMAND_VERIFY_CODE:
          self.ProcessGetVerifyCode(msg)
        elif msg.cmd == consts.COMMAND_LOGIN:
          self.ProcessLogin(msg)
  
  
  
  
  def ProcessInitToken(self):
    my_token.token.Init(self.prop['token'])
    #如果有token
    if my_token.token.Get() is not None:
      #验证token有效性，有效则切换，告知界面切换
      if self.VerifyToken():
        command = pb.apk_protomsg_pb2.Command()
        command.cmd = consts.COMMAND_LOGIN
        command.cmd_no = -1
        self.SendCommandResponse(command,
                                 consts.ERROR_CODE_OK,
                                 [])
      else:
        my_token.token.Set(None)
    
  
  
  def VerifyToken(self):
    url = config.URL + '/api/applist'
  
    headers = {
      'User-Agent': 'Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.132 Safari/537.36',
      'X-atoken-Authorization': my_token.token.Get(),
    }


    try:
      timeout = (10, 180)
      try:
        response = requests.request("get", url, stream=True, data=None, headers=headers, timeout=timeout)
      except requests.exceptions.SSLError as e:
        response = requests.request("get", url, stream=True, data=None, headers=headers, verify=False, timeout=timeout)
    
      if response.status_code == 200:
        json_data = json.loads(response.content)
        if json_data['code'] == 200:
          return True
        else:
          my_token.token.Set(None)
    except requests.ConnectionError as e:
      print('Error', e.args)
    except Exception as e:
      print('Error', e.args)
  
    return False
    
    
  
  def ProcessLogin(self, command):
    url = config.URL + '/api/auth/login'
  
    headers = {
      'User-Agent': 'Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.132 Safari/537.36',
    }

    data = {
      'mobile': command.param[0].encode('utf-8'),
      'vcode': command.param[1].encode('utf-8'),
    }
    ret = False
    try:
      timeout = (10, 180)
      try:
        response = requests.request("post", url, stream=True, data=data, headers=headers, timeout=timeout)
      except requests.exceptions.SSLError as e:
        response = requests.request("post", url, stream=True, data=data, headers=headers, verify=False, timeout=timeout)
        
    
      if response.status_code == 200:
        json_data = json.loads(response.content)
        if json_data['code'] == 200:
          my_token.token.Set(json_data['data']['token'])
          self.SendCommandResponse(command,
                                   consts.ERROR_CODE_OK,
                                   [])
          return
        else:
          self.SendCommandResponse(command,
                                   consts.ERROR_CODE_LOGIN_FAILED,
                                   [json_data['msg'].encode('utf-8')])
          return
      else:
        self.SendCommandResponse(command,
                                 consts.ERROR_CODE_LOGIN_FAILED,
                                 [])
        print('net Error', response.status_code)
        return
  
  
    except requests.ConnectionError as e:
      print('Error', e.args)
    except Exception as e:
      print('Error', e.args)

    self.SendCommandResponse(command,
                             consts.ERROR_CODE_LOGIN_FAILED,
                             [])
  
  
  
  
  def ProcessGetVerifyCode(self, command):
    url = config.URL + '/api/auth/getcode'
    
    headers = {
      'User-Agent': 'Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.132 Safari/537.36',
    }
    params = {
      'mobile': command.param[0].encode('utf-8')
    }
    ret = False
    try:
      timeout = (10, 180)
      try:
        response = requests.request("GET", url, stream=True, data=None, headers=headers, params=params, timeout=timeout)
      except requests.exceptions.SSLError as e:
        response = requests.request("GET", url, stream=True, data=None, headers=headers, verify=False, params=params, timeout=timeout)

      if response.status_code == 200:
        json_data = json.loads(response.content)
        if json_data['code'] == 200:
          self.SendCommandResponse(command,
                                   consts.ERROR_CODE_OK,
                                   [])
      else:
        print('net Error', response.status_code)
      

    except requests.ConnectionError as e:
      print('Error', e.args)
    except Exception as e:
      print('Error', e.args)

  
  
  
  
  def ProcessStartServer(self, command):
    # 遍历所有的server，尝试kill，保留kill不掉的
    self.SendCommandResponse(command, consts.ERROR_CODE_OK, ['初始化运行环境...', ])
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

    self.SendCommandResponse(command, consts.ERROR_CODE_OK, ['初始化完成！', ])
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
    self.master.ProcessCommand('-', command)
    
    
  
  def ProcessAutoInstall(self, command):
    self.master.EnterAutoInstall(command)
    pass
  
  
  
  
  def ProcessTotalAutoInstall(self, command):
    if int(command.param[0]) == 1:
      self.log.info('ProcessTotalAutoInstall enter total auto')
      if self.total_auto_defer is None:
        self.log.info('ProcessTotalAutoInstall begin total auto')
        self.total_auto_defer = defer.Deferred()
        
        def step1(result, phone):
          #获取本地列表
          phone.log.info('ProcessTotalAutoInstall step1')
          get_local = pb.apk_protomsg_pb2.Command()
          get_local.cmd = consts.COMMAND_GET_LOCAL_PACKAGELIST
          get_local.cmd_no = -1
          phone.ProcessGetLocalPackageList(get_local)

          
        def step2(result, phone, command):
          #进入自动模式
          phone.log.info('ProcessTotalAutoInstall step2')
          phone.ProcessAutoInstall(command)
          #这是最后一步了，defer没事情做了，要把自己设置成None
          phone.total_auto_defer = None
        
        self.total_auto_defer.add_callback(step1, self)
        self.total_auto_defer.add_callback(step2, self, command)

        # 检查网络更新
        check = pb.apk_protomsg_pb2.Command()
        check.cmd = consts.COMMAND_CHECK_UPDATE
        check.cmd_no = -1
        self.ProcessCheckUpdatePackageList(check, self.total_auto_defer)
      else:
        self.log.info('ProcessTotalAutoInstall total_auto_defer not None, just wait')
    else:
      self.log.info('ProcessTotalAutoInstall leave total auto')
      self.ProcessAutoInstall(command)
  
  
  def ProcessRefresh(self, command):
    id = command.param[0].encode('utf-8')
    if id == 'all':
      for one in self.master.last_devices_list:
        self.master.ProcessCommand(one['serial_no'], command)
    elif id in self.master.last_devices_map:
      self.master.ProcessCommand(id, command)
    
  
  
  def ProcessGetPackageList(self, command):
    # 此处有并发隐患
    for one in self.master.last_devices_list:
      self.master.ProcessCommand(one['serial_no'], command)
  
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
  #     #self.SendDevicesList(command, PhoneLogic.ERROR_CODE_PYADB_SCAN_DEVICES_FAILED, None, str(device_list))
  #     pass
  
  def ProcessInstallApk3(self, command):
    #将包名换成路径
    package_name = command.param[1]
    apk_path = self.apk_dir + '\\' + package_name + '.apk'
    command.param[1] = apk_path
    # 此处有并发隐患
    # tmp = self.device.last_devices_list.copy()
    for one in self.master.last_devices_list:
      self.master.ProcessCommand(one['serial_no'], command)
  

  
  def ProcessGetLocalPackageList(self, command):
    try:
      local_prop = self.LoadLocalProp()
      #存储包信息

      apk_list = []
      if local_prop is not None:
        self.package_map.clear()
        for one in local_prop['data']:
          one_apk = pb.apk_protomsg_pb2.OneApk()
          one_apk.id = one['id']
          one_apk.md5 = one['md5']
          one_apk.name = one['apkName']
          one_apk.brief = one['introduction']
          one_apk.apk_name = one['packageName']
          one_apk.price = one['price']
          one_apk.type = consts.PACKAGE_BOTH
          one_apk.package_size = util.utility.GetFileSize(self.prop['apkPath'] + '/' + one['packageName'] + '.apk')
          self.package_map[one_apk.apk_name] = one
          apk_list.append(one_apk)
        
        # for one in local_prop['data']['remove']:
        #   one_apk = pb.apk_protomsg_pb2.OneApk()
        #   one_apk.md5 = ''.decode('utf-8')
        #   one_apk.name = one['packageName']
        #   one_apk.brief = ''.decode('utf-8')
        #   one_apk.apk_name = one['apkName']
        #   one_apk.price = one['price']
        #   one_apk.type = consts.PACKAGE_REMOVE
        #   one_apk.package_size = 0
        #   apk_list.append(one_apk)
        
        self.master.UpdateApkList(self.package_map)
        
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
      # 无论如何，最后要把目录再创建下
      util.utility.CreateDir(self.prop['apkPath'])
    except Exception as e:
      pass
  
  
  
  def ProcessCheckUpdatePackageList(self, command, deferred=None):
    self.check_update.apk_path = self.prop['apkPath']
    self.check_update.ProcessCheckUpdate(command, deferred)
  
  
  
  
  
  def SendCommandResponse(self, cmd, code, info=None):
    device.callback.SendCommandResponse(self.queue_out, cmd, code, info)
    
    
    
  
  def SendApkList(self, cmd, code, package_list=None):
    response = pb.apk_protomsg_pb2.ApkList()
    response.head.cmd = cmd.cmd
    response.head.cmd_no = cmd.cmd_no
    response.head.sub_cmd_no = cmd.sub_cmd_no
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
  
  


# ======================================================
if __name__ == '__main__':
  msg_queue = Queue.Queue()
  msg_queue2 = Queue.Queue()
  checkUpdate = PhoneLogic('ctp', msg_queue2, msg_queue)
  checkUpdate.CheckPackageList()