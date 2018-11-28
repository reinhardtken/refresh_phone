#!/usr/bin/env python
# -*- coding: utf-8 -*-


import threading
import Queue
import asyncore
import logging
import socket
import time


import consts
import util.utility

import pb.apk_protomsg_pb2


class CallbackObject():
  def __init__(self, queue, command, serial_no, apk):
    self.queue = queue
    self.command = command
    self.serial_no = serial_no
    self.last_progress = 0
    self.apk = apk
  
  def Callback(self, apk, now, total):
    
    progress = float(now) / total
    if (progress - self.last_progress) > 0.05 or now == total:
      self.last_progress = progress
      SendCommandProgress(self.queue, self.command, consts.ERROR_CODE_OK,
                               [self.serial_no.encode('utf-8'), apk, str(progress), ])
  
  def CallbackSucc(self, progress):
    stage = '进行中'
    if progress == 'Success':
      stage = '完成'
    elif progress == 'push over: ':
      stage = '安装中'
    SendCommandProgress(self.queue, self.command, consts.ERROR_CODE_OK,
                                  [self.serial_no.encode('utf-8'), stage, self.apk, progress, ])
  
  def CallbackFail(self, progress):
    SendCommandProgress(self.queue, self.command, consts.ERROR_CODE_PYADB_INSTALL_APK_FAILED,
                                  [self.serial_no.encode('utf-8'), '完成', self.apk, progress, ])
    
    
    

def Send(queue, data):
  if isinstance(queue, list):
    for one in queue:
      one.put(data)
  else:
    queue.put(data)



def SendCommandProgress(queue, cmd, code, info=None):
  response = pb.apk_protomsg_pb2.CommandProgress()
  response.cmd = cmd.cmd
  response.cmd_no = cmd.cmd_no
  start_timestamp = cmd.timestamp
  response.time_cost = util.utility.UTCTime2TimeInMicroseconds(int(time.time())) - start_timestamp
  response.code = code
  if info is not None:
    for one in info:
      response.info.append(one.decode('utf-8'))

  Send(queue, response)


def SendCommandResponse(queue, cmd, code, info=None):
  response = pb.apk_protomsg_pb2.CommandResponse()
  response.cmd = cmd.cmd
  response.cmd_no = cmd.cmd_no
  response.code = code
  if info is not None:
    for one in info:
      response.info.append(one.decode('utf-8'))

  Send(queue, response)



# def SendApkList(queue, cmd, code, package_list=None):
#   response = pb.apk_protomsg_pb2.ApkList()
#   response.head.cmd = cmd.cmd
#   response.head.cmd_no = cmd.cmd_no
#   response.head.code = code
#   if package_list is not None:
#     for one in package_list:
#       one_apk = response.apk_list.add()
#       one_apk.md5 = one.md5
#       one_apk.name = one.name
#       one_apk.brief = one.brief
#       one_apk.apk_name = one.apk_name
#       one_apk.price = one.price
#       one_apk.type = one.type
#
#   queue.put(response)


def SendDevicesList(queue_master, cmd, code, device_list=None, info=None):
  try:
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
  
    queue_master.put(response)
    # queue_network.put(response)
  except Exception as e:
    pass