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
class OneDevice(object):
  def __init__(self, queue_master, serial_number):
    self.log = util.log.GetLogger(self.__class__.__name__)
    self.log.info('OneDevice ' + serial_number)
    print("OneDevice.init=======================================")
    self.queue_master = queue_master
    self.serial_number = serial_number
    self.proxy = None

    # 记录所有已经成功安装的包，除非强制清空，不然不再重复安装
    self.installed_set = set()

    # 记录所有超时的包，超时包会导致后续安装无法进行，除非强制，否转不再重复安装
    self.installed_timeout_set = set()
    
    
    #当发生过杀线程的事情，min id被设置成杀之前接收命令的最大id，确保如果无法正确杀死线程，也不会被线程后续回复干扰
    self.min_command_id = 0
    self.last_command_id = 0
    
   
  def _Start(self):
    self.proxy.Start()
      
      
  def Stop(self):
    self.min_command_id = self.last_command_id
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
    
      
      
  def IsInstalled(self, apk):
    return apk in self.installed_set
  
  
  def IsTimeOut(self, apk):
    return apk in self.installed_timeout_set
  
  
  def AddInstalled(self, apk):
    self.log.info('AddInstalled ' + self.serial_number + ' ' + apk)
    self.installed_set.add(apk)
    
  
  def AddTimeOut(self, apk):
    self.log.info('AddTimeOut ' + self.serial_number + ' ' + apk)
    self.installed_timeout_set.add(apk)
    
    
  def SendCommand(self, command):
    if self.proxy is not None:
      self.last_command_id = command.cmd_no
      self.proxy.SendCommand(command)



  def ProcessInstallApkResponse(self, command):
    if command.cmd_no > self.min_command_id:
      if len(command.info) >= 4:
        if command.info[1] == '完成'.decode('utf-8') and command.info[3] == 'Success'.decode('utf-8'):
          self.AddInstalled(command.info[2])
          
      return True
    
    else:
      self.log.warning('ProcessInstallApkResponse  the min: %d  the one: %d', self.min_command_id, command.cmd_no)
  

  


class Master(object):
  def __init__(self, queue_out):
    print("Master.init=======================================")
    self.queue_out = queue_out
    self.queue_in = Queue.Queue()
    self.devices_map = {}
    # self.last_alive = {}
    self.last_devices_list = []
    
    
    # self.apk_map = {}
    
    
    
    
    
    #超时map
    self.last_command_timeout = {}

    self.log = util.log.GetLogger(self.__class__.__name__)

    self.thread = threading.Thread(target=self.Work)
    self._continue = True
    self.thread.setDaemon(True)
    
    
  
  # def UpdateApkList(self, data):
  #   for one in data:
  #     self.apk_map[one.apk_name] = one
    
  
  def _Add(self, serial_number):
    if serial_number in self.devices_map:
      self.devices_map[serial_number].MaybeCreate()
      return self.devices_map[serial_number]
    else:
      one = OneDevice(self.queue_in, serial_number)
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
      type = command.param[0].encode('utf-8')
      if 'force' in type:
        self.log.info('ProcessInstallApk3  force ' + command.cmd)
        one.SendCommand(command)
      else:
        self.log.info('ProcessInstallApk3 ' + command.cmd)
        apk_path = command.param[1].encode('utf-8')
        package_name = util.utility.GetPackageNameFromPath(apk_path)
        if one.IsInstalled(package_name):
          callback.SendCommandProgress(self.queue_out, command, consts.ERROR_CODE_OK,
                                              [one.serial_number, '完成', package_name, '已经装过跳过安装', ''])
        elif one.IsTimeOut(package_name):
          callback.SendCommandProgress(self.queue_out, command, consts.ERROR_CODE_PYADB_OP_TIMEOUT_FAILED,
                                              [one.serial_number, '完成', package_name, '曾经超时跳过安装', ''])
        else:
          one.SendCommand(command)
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
          package_name = util.utility.GetPackageNameFromPath(apk_path)
          #超时的包，记录超时失败，不再继续
          self._Get(k).AddTimeOut(package_name)
          callback.SendCommandProgress(self.queue_out, v['c'],
                                       consts.ERROR_CODE_PYADB_OP_TIMEOUT_FAILED,
                                       [k.encode('utf-8'), '完成', package_name, '超时', str(tmp[k]['max'])])
          
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
      
    
    return True



  def ProcessInstallApkResponse(self, command):
    return self._Get(command.info[0]).ProcessInstallApkResponse(command)
  

  
    
  def ProcessIncome(self, command):
    forword = True
    if isinstance(command, pb.apk_protomsg_pb2.DevicesList):
      forword = self.ProcessScanDevicesResponse(command)
    elif isinstance(command, dict):
      if command['c'] == consts.COMMAND_INNER_START_CHECKOUT_TIMEOUT:
        forword = self.ProcessStartCheckTimeOut(command)
      elif command['c'] == consts.COMMAND_INNER_STOP_CHECKOUT_TIMEOUT:
        forword = self.ProcessStopCheckTimeOut(command)
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
      time.sleep(1)
  
  def Join(self):
    self.thread.join()


# ======================================
if __name__ == '__main__':
  pass