#!/usr/bin/env python
# -*- coding: utf-8 -*-


import threading
import Queue
import asyncore
import logging
import socket
import time
from collections import deque
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

    self.package_list = []
    self.package_set = set()
    
    
    self.current_installapk_response = {}
    
    self.sub_command_id = 0
    
   
  
  
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
      # 添加sub_command_id
      command.sub_cmd_no = self._GenSubCommandID()
      self.proxy.SendCommand(command)



  def ProcessInstallApkResponse(self, command):
    if command.cmd_no > self.min_command_id:
      if command.stage == '完成'.decode('utf-8') and command.progress == 'Success'.decode('utf-8'):
        self.AddInstalled(command.package_name)
        
          
      return True
    
    else:
      self.log.warning('ProcessInstallApkResponse  the min: %d  the one: %d', self.min_command_id, command.cmd_no)
  

  
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
    # MIN_INTERVAL = 5 * 60
    # if len(self.package_list) == 0:
    #   self.ProcessPackageList()
    # elif self.last_get_package_list is None or time.time() - self.last_get_package_list > MIN_INTERVAL:
    #   self.ProcessPackageList()


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
    # if target is not None:
    #   self.current_response['target'] = target
    #
    # if command is not None:
    #   self.current_response['command'] = command
    #
    # if error is not None:
    #   self.current_response['error'] = error
    #
    # if serial_number is not None:
    #   self.current_response['serial_number'] = serial_number
    #
    # if stage is not None:
    #   self.current_response['stage'] = stage
    #
    # if package_name is not None:
    #   self.current_response['package_name'] = package_name
    #
    # if progress is not None:
    #   self.current_response['progress'] = str(progress)
    #
    # if time_max is not None:
    #   self.current_response['time_max'] = str(time_max)
    #
    # if size is not None:
    #   self.current_response['package_size'] = str(size)
    #
    # if type is not None:
    #   self.current_response['type'] = str(type)
    #
    # if info is not None:
    #   self.current_response['info'] = info
    #
    # return self.current_response
    
#########################################################
class Master(object):
  def __init__(self, queue_out):
    print("Master.init=======================================")
    self.queue_out = queue_out
    self.queue_in = Queue.Queue()
    self.devices_map = {}
    # self.last_alive = {}
    self.last_devices_list = []
    self.last_devices_set = set()
    
    self.pending_task_list = deque()
    
    
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
          self._Get(k).AddTimeOut(package_name)
          callback.SendCommandResponse(self.queue_out, v['c'],
                                       consts.ERROR_CODE_PYADB_OP_TIMEOUT_FAILED,
                                       [k.encode('utf-8'), '完成', package_name, '超时', str(tmp[k]['max'])])
          
        self._Remove(k)
      
        
        
    
    
  
  def ProcessScanDevicesResponse(self, command):
    self.last_devices_list = []
    self.last_devices_set = set()
    for one_device in command.devices_list:
      one = {}
      one['serial_no'] = one_device.serial_no
      one['product'] = one_device.product
      one['model'] = one_device.model
      one['device'] = one_device.device
      self.last_devices_list.append(one)
      self.last_devices_set.add(one['serial_no'])
      
    
    #对于所有手机，枚举已经安装的包
    # for one in self.last_devices_list:
    #   d = self._Add(one['serial_no'])
    #   d.TriggerProcessPackageList()
      
    
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
      self.CheckPendingTask()
      time.sleep(1)
  
  def Join(self):
    self.thread.join()
    
    
  
  def CheckPendingTask(self):
    while len(self.pending_task_list):
      if self.pending_task_list[0][0] < time.time():
        pass

# ======================================
if __name__ == '__main__':
  pass