#!/usr/bin/env python
# -*- coding: utf-8 -*-


import threading
import Queue
import asyncore
import logging
import socket
# ====================================

import threading
import time
import inspect
import ctypes



import consts
import adbtool.list_devices
import callback

import util.log
import util.utility
import pb.apk_protomsg_pb2

def _async_raise(tid, exctype):
  """raises the exception, performs cleanup if needed"""
  tid = ctypes.c_long(tid)
  if not inspect.isclass(exctype):
    exctype = type(exctype)
  res = ctypes.pythonapi.PyThreadState_SetAsyncExc(tid, ctypes.py_object(exctype))
  if res == 0:
    raise ValueError("invalid thread id")
  elif res != 1:
    # """if it returns a number greater than one, you're in trouble,
    # and you should call it again with exc=NULL to revert the effect"""
    ctypes.pythonapi.PyThreadState_SetAsyncExc(tid, None)
    raise SystemError("PyThreadState_SetAsyncExc failed")


def stop_thread(id):
  try:
    _async_raise(id, SystemExit)
  except Exception as e:
    print(e)
    pass


# ====================================
class Proxy(object):
  def __init__(self, queue_master, serial_number):
    print("Proxy.init=======================================")
    self.queue_master = queue_master
    self.queue_in = Queue.Queue()
    self.serial_number = serial_number
    self._continue = True
    self.thread = threading.Thread(target=self.Work)
    self.thread.setDaemon(True)

    self.log = util.log.GetLogger(self.__class__.__name__)
    
    # self.last_command = {}
    # self.last_command['command'] = None
    # self.last_command['timestamp'] = time.time()
    
    #for install apk
    self.current_response = {}
    

    self.debug_once = True
  
  
  
  def _CleanCurrentResponse(self):
    self.current_response['info'] = ['' for x in range(10)]
  
  
  def Start(self):
    self.thread.start()
    
  
  def Stop(self):
    self._continue = False
    
  
  def ForceStop(self):
    #从日志看，这个方法并不能结束线程。。。
    self.log.info('ForceStop')
    self.log.info(self)
    # stop_thread(self.thread.ident)
    self.log.info('ForceStop over')
    
    
  def SendCommand(self, command):
    self.queue_in.put(command)
    
  
  def Process(self, command):
    self.BeforeCommand(command)
    
    if command.cmd == consts.COMMAND_SCAN_DEVICES:
      self.ProcessScanDevices(command)
    elif command.cmd == consts.COMMAND_INSTALL_APK:
      self.ProcessInstallApk(command)
    
    self.AfterCommand(command)
  
  
  
  def DealWithIncomeQueue(self):
    while not self.queue_in.empty() and self._continue:
      msg = self.queue_in.get()
      self.Process(msg)
      #报活
      # self.queue_master.put(('alive', self.serial_number))
      # self.queue_in.task_done()
  
  
  def Work(self):
    while self._continue:
      self.DealWithIncomeQueue()
      #跑到这里说明当前没有命令在执行，也就是不会处于卡主的状态
      # self.last_command['command'] = None
      # self.last_command['timestamp'] = time.time()
      time.sleep(1)
  
  
  def Join(self):
    self.thread.join()
    
    
#########################################################
  def ProcessScanDevices(self, command):
    def Callback(data):
      # 记录最新的设备列表
      self.last_devices_list = data
      callback.SendDevicesList(self.queue_master, command, consts.ERROR_CODE_OK, data)
  
    devices = adbtool.list_devices.Command(Callback)
    self.log.info('before ProcessScanDevices')
    devices.Execute()
    self.log.info('end ProcessScanDevices')


  
  def StartCheckTimeOut(self, command, timeout):
    response = {}
    response['c'] = consts.COMMAND_INNER_START_CHECKOUT_TIMEOUT
    response['s'] = self.serial_number
    response['command'] = command
    response['t'] = timeout
    self.queue_master.put(response)
    
    
    
  
  def StopCheckTimeOut(self, command):
    if command.cmd == consts.COMMAND_INSTALL_APK:
      response = {}
      response['c'] = consts.COMMAND_INNER_STOP_CHECKOUT_TIMEOUT
      response['s'] = self.serial_number
      response['command'] = command
      self.queue_master.put(response)
  
  
  
  def BeforeCommand(self, command):
    #基准值换算
    old_timestamp = command.timestamp
    command.timestamp = util.utility.UTCTime2TimeInMicroseconds(int(time.time()))
    
    self._CleanCurrentResponse()
    
    if command.cmd == consts.COMMAND_INSTALL_APK:
      apk_path = command.param[1].encode('utf-8')
      package_size = util.utility.GetFileSize(apk_path)
      package_name = util.utility.GetPackageNameFromPath(apk_path)
      #目前看，最慢速度是0.5mb/s，所以按这个数值+30s作为超时时长
      if 'air.tv.douyu' in package_name and self.debug_once and package_size > 30:
        self.debug_once = False
        time_max = package_size  - 30
      else:
        time_max = package_size * 2 + 30
      self.StartCheckTimeOut(command, time_max)
      response = self.GenCommandResponse(t=self.queue_master, c=command, s=self.serial_number.encode('utf-8'),
                                         p=package_name, time_max=time_max, size=package_size)
      callback.SendCommandProgress(self.queue_master, command, consts.ERROR_CODE_OK,
                                   response['info'])
      pass
    
  
  
  
  def AfterCommand(self, command):
    if command.cmd == consts.COMMAND_INSTALL_APK:
      self.StopCheckTimeOut(command)
  
  
  
  
  
  def GenCommandResponse(self, t=None, c=None, err=None, s=None, stage=None, p=None, progress=None, time_max=None,
                         size=None):
    
    if t is not None:
      self.current_response['target'] = t
      
    if c is not None:
      self.current_response['command'] = c
      
    if err is not None:
      self.current_response['error'] = err
      
      
    if s is not None:
      self.current_response['info'][0] = s
      
    if stage is not None:
      self.current_response['info'][1] = stage
      
    if p is not None:
      self.current_response['info'][2] = p
      
    if progress is not None:
      self.current_response['info'][3] = str(progress)
      
    if time_max is not None:
      self.current_response['info'][4] = str(time_max)
      
    if size is not None:
      self.current_response['info'][5] = str(size)
      
    
    return self.current_response
  
  

  def ProcessInstallApk(self, command):
    try:
      
      apk_path = command.param[1].encode('utf-8')
      package_name = util.utility.GetPackageNameFromPath(apk_path)
      # self.last_command['package_name'] = package_name
      self.log.info(self)
      self.log.info('command ' + command.cmd)
      self.log.info('command id ' + str(command.cmd_no))
      self.log.info('command apk_path ' + apk_path)
      self.log.info('command package_name ' + package_name)
      

      cb = callback.CallbackObject(self.queue_master, command, self.serial_number, self.GenCommandResponse)
      install = adbtool.install.Command(self.serial_number, package_name, apk_path, cb.CallbackSucc,
                                        cb.CallbackFail)

      #    test code
      # if 'com.youku.phone' in package_name and self.debug_once:
      #   self.debug_once = False
      #   time.sleep(10000)

      self.log.info('before ProcessInstallApk ' + self.serial_number + ' : ' + apk_path)
      callback.SendCommandProgress(self.queue_master, command, consts.ERROR_CODE_OK,
                                   self.GenCommandResponse(stage='开始')['info'])

      install.Execute()

      self.log.info('end ProcessInstallApk')

    except Exception as e:
      pass



# ======================================
if __name__ == '__main__':
  pass