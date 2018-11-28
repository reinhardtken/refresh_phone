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
  def __init__(self, queue_network, queue_master, serial_number):
    print("Proxy.init=======================================")
    self.queue_master = queue_master
    self.queue_in = Queue.Queue()
    self.serial_number = serial_number
    self._continue = True
    self.thread = threading.Thread(target=self.Work)
    self.thread.setDaemon(True)

    self.log = util.log.GetLogger(self.__class__.__name__)
    
    self.last_command = {}
    self.last_command['command'] = None
    self.last_command['timestamp'] = time.time()

    self.debug_once = True
  
  
  def Start(self):
    self.thread.start()
    
  
  def Stop(self):
    self._continue = False
    
  
  def ForceStop(self):
    stop_thread(self.thread.ident)
    
    
  def SendCommand(self, command):
    self.queue_in.put(command)
    
  
  def Process(self, command):
    self.last_command['command'] = command
    self.last_command['timestamp'] = time.time()
    
    if command.cmd == 'pyadb_scan_devices':
      self.ProcessScanDevices(command)
    elif command.cmd == 'pyadb_install_apk':
      self.ProcessInstallApk(command)
    pass
  
  
  def DealWithIncomeQueue(self):
    while not self.queue_in.empty():
      msg = self.queue_in.get()
      self.Process(msg)
      #报活
      # self.queue_master.put(('alive', self.serial_number))
      # self.queue_in.task_done()
  
  
  def Work(self):
    while self._continue:
      self.DealWithIncomeQueue()
      #跑到这里说明当前没有命令在执行，也就是不会处于卡主的状态
      self.last_command['command'] = None
      self.last_command['timestamp'] = time.time()
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



  def ProcessInstallApk(self, command):
    try:
      old_timestamp = command.timestamp
      command.timestamp = util.utility.UTCTime2TimeInMicroseconds(int(time.time()))
      
      apk_path = command.param[1].encode('utf-8')
      package_name = util.utility.GetPackageNameFromPath(apk_path)
      self.last_command['package_name'] = package_name
      self.log.info('command ' + command.cmd)
      self.log.info('command id ' + str(command.cmd_no))
      self.log.info('command apk_path ' + apk_path)
      self.log.info('command package_name ' + package_name)
      

      cb = callback.CallbackObject(self.queue_master, command, self.serial_number, package_name)
      install = adbtool.install.Command(self.serial_number, package_name, apk_path, cb.CallbackSucc,
                                        cb.CallbackFail)

      #    test code
      # if 'com.youku.phone' in package_name and self.debug_once:
      #   self.debug_once = False
      #   time.sleep(10000)

      self.log.info('before ProcessInstallApk2 ' + self.serial_number + ' : ' + apk_path)
      callback.SendCommandProgress(self.queue_master, command, consts.ERROR_CODE_OK,
                               [self.serial_number.encode('utf-8'), '开始', package_name, '', ])

      install.Execute()

      self.log.info('end ProcessInstallApk2')

    except Exception as e:
      pass



# ======================================
if __name__ == '__main__':
  pass