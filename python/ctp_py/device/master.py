#!/usr/bin/env python
# -*- coding: utf-8 -*-


import threading
import Queue
import asyncore
import logging
import socket
import time
# ====================================

import proxy
import util.log
import callback
import consts
import pb.apk_protomsg_pb2
# ====================================
class Master(object):
  def __init__(self, queue_out):
    print("Master.init=======================================")
    self.queue_out = queue_out
    self.queue_in = Queue.Queue()
    self.devices_map = {}
    # self.last_alive = {}
    self.last_devices_list = []
    #记录所有已经成功安装的包，除非强制清空，不然不再重复安装
    self.installed_map = {}

    self.log = util.log.GetLogger(self.__class__.__name__)

    self.thread = threading.Thread(target=self.Work)
    self._continue = True
    self.thread.setDaemon(True)
    
  
  def _Add(self, serial_number):
    if serial_number in self.devices_map:
      return self.devices_map[serial_number]
    else:
      one = proxy.Proxy(self.queue_out, self.queue_in, serial_number)
      self.devices_map[serial_number] = one
      one.Start()
      return one
  
  def _Remove(self, serial_number):
    if serial_number in self.devices_map:
      self.devices_map[serial_number].ForceStop()
      self.devices_map.pop(serial_number)
      
  
  def ProcessCommand(self, serial_number, command):
    #每次执行新命令前，看看旧有设备有无超时的
    self.CheckTimeout()
    
    one = self._Add(serial_number)
    one.SendCommand(command)
    
    
  
  def CheckTimeout(self):
    K_TIMEOUT = 120
    # 检查所有，干掉超时
    tmp = self.devices_map.copy()
    for k, v in tmp.items():
      if v.last_command['command'] is not None and time.time() - v.last_command['timestamp'] > K_TIMEOUT:
        #目前应该只有装包会超时
        if v.last_command['command'].cmd == 'pyadb_install_apk':
          package_name = v.last_command['package_name']
          callback.SendCommandProgress(self. queue_out, v.last_command['command'], consts.ERROR_CODE_PYADB_OP_TIMEOUT_FAILED,
                                 [k.encode('utf-8'), '完成', package_name, '超时', ])
        self._Remove(k)
        
        
    
    
  
  def ProcessScanDevicesResponse(self, command):
    self.last_devices_list = []
    for one_device in command.devices_list:
      one = {}
      one['serial_no'] = one_device.serial_no
      one['product'] = one_device.product
      one['model'] = one_device.model
      one['device'] = one_device.device
      self.last_devices_list.append(one)
      if one_device.serial_no not in self.installed_map:
        self.installed_map[one_device.serial_no] = set()
      
    
    return True



  def ProcessInstallApkResponse(self, command):
    if len(command.info)  == 4:
      if command.info[1] == '完成'.decode('utf-8') and command.info[3] == 'Success'.decode('utf-8'):
        if command.info[0] in self.installed_map:
          self.installed_map[command.info[0]].add(command.info[2])
  
    return True

  
    
  def ProcessIncome(self, command):
    forword = True
    if isinstance(command, pb.apk_protomsg_pb2.DevicesList):
      forword = self.ProcessScanDevicesResponse(command)
    elif command.cmd == 'pyadb_install_apk':
      forword = self.ProcessInstallApkResponse(command)
      
      
    
    if forword:
      self.queue_out.put(command)
    
        

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
      time.sleep(1)
  
  def Join(self):
    self.thread.join()


# ======================================
if __name__ == '__main__':
  pass