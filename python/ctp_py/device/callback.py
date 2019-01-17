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


class CallbackInstallObject():
  def __init__(self, host):
    self.host = host

  
  # def Callback(self, apk, now, total):
  #
  #   progress = float(now) / total
  #   if (progress - self.last_progress) > 0.05 or now == total:
  #     self.last_progress = progress
  #     SendCommandResponse(self.queue, self.command, consts.ERROR_CODE_OK,
  #                         self.gen_func(progress=str(progress))['info'])
  
  def CallbackSucc(self, progress):
    stage = '进行中'
    if progress == 'Success':
      stage = '完成'
    elif progress == 'push over: ':
      stage = '安装中'
    SendInstallApkResponse(self.host.GenInstallApkResponse(error=consts.ERROR_CODE_OK, progress=progress, stage=stage))
  
  def CallbackFail(self, progress):
    SendInstallApkResponse(self.host.GenInstallApkResponse(error=consts.ERROR_CODE_PYADB_INSTALL_APK_FAILED,
                        stage='完成', progress=progress))


class CallbackUninstallObject():
  def __init__(self, host):
    self.host = host
  
  
  def CallbackSucc(self, progress):
    stage = '进行中'
    if progress == 'Success':
      stage = '完成删除老包'
      SendInstallApkResponse(self.host.GenInstallApkResponse(error=consts.ERROR_CODE_OK,
                        stage=stage, progress=progress))
  
  def CallbackFail(self, progress):
    SendInstallApkResponse(self.host.GenInstallApkResponse(error=consts.ERROR_CODE_PYADB_UNINSTALL_APK_FAILED,
                        stage='删除老包失败'))
    
    
    

def Send(queue, data):
  if isinstance(queue, list):
    for one in queue:
      one.put(data)
  else:
    queue.put(data)



def SendCommandResponse(queue, cmd, code, info=None):
  response = pb.apk_protomsg_pb2.CommandResponse()
  response.cmd = cmd.cmd
  response.cmd_no = cmd.cmd_no
  if cmd.sub_cmd_no is not None:
    response.sub_cmd_no = cmd.sub_cmd_no
  else:
    response.sub_cmd_no = 0
    
  start_timestamp = cmd.timestamp
  response.time_cost = util.utility.UTCTime2TimeInMicroseconds(int(time.time())) - start_timestamp
  response.code = code
  if info is not None:
    for one in info:
      response.info.append(one.decode('utf-8'))

  Send(queue, response)




def GenInstallApkResponse(current_response, target=None, command=None, error=consts.ERROR_CODE_OK, serial_number=None, stage='',
                          package_name='', progress='', time_max=0,
                          package_size=0, type='', adb_message='', info=None):

  if target is not None:
    current_response['target'] = target

  if command is not None:
    current_response['command'] = command

  if error is not None:
    current_response['error'] = error

  if serial_number is not None:
    current_response['serial_number'] = serial_number

  if stage is not None:
    current_response['stage'] = stage

  if package_name is not None:
    current_response['package_name'] = package_name

  if progress is not None:
    current_response['progress'] = progress

  if time_max is not None:
    current_response['time_max'] = time_max

  if package_size is not None:
    current_response['package_size'] = package_size

  if type is not None:
    current_response['type'] = type
    
  if adb_message is not None:
    current_response['adb_message'] = adb_message

  if info is not None:
    current_response['info'] = info

  return current_response
  
  
  
def SendInstallApkResponse(r):
  response = pb.apk_protomsg_pb2.CommandInstallApkResponse()
  response.cmd = r['command'].cmd
  response.cmd_no = r['command'].cmd_no
  response.sub_cmd_no = r['command'].sub_cmd_no
  response.code = r['error']
  start_timestamp = r['command'].timestamp
  response.time_cost = util.utility.UTCTime2TimeInMicroseconds(int(time.time())) - start_timestamp
  
  if 'package_name' in r and r['package_name'] is not None:
    response.package_name = r['package_name'].decode('utf-8')
  else:
    response.package_name = ''
    
  if 'progress' in r and r['progress'] is not None:
    response.progress = r['progress'].decode('utf-8')
  else:
    response.progress = ''

  if 'type' in r and r['type'] is not None:
    response.type = r['type'].decode('utf-8')
  else:
    response.type = ''
    

  response.serial_number = r['serial_number']
  
  if 'time_max' in r and r['time_max'] is not None:
    response.time_max = int(r['time_max'])
  else:
    response.time_max = int(0)

  if 'package_size' in r and r['package_size'] is not None:
    response.package_size = float(r['package_size'])
  else:
    response.package_size = float(0)
    
  if 'stage' in r and r['stage'] is not None:
    response.stage = r['stage'].decode('utf-8')
  else:
    response.stage = ''
    
  if 'adb_message' in r and r['adb_message'] is not None:
    response.adb_message = r['adb_message'].decode('utf-8')
  else:
    response.adb_message = ''

  
  if 'info' in r and r['info'] is not None:
    for one in r['info']:
      response.info.append(one.decode('utf-8'))

  Send(r['target'], response)


# def SendCommandResponse(queue, cmd, code, info=None):
#   response = pb.apk_protomsg_pb2.CommandResponse()
#   response.cmd = cmd.cmd
#   response.cmd_no = cmd.cmd_no
#   response.code = code
#   if info is not None:
#     for one in info:
#       response.info.append(one.decode('utf-8'))
#
#   Send(queue, response)



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
    response.head.sub_cmd_no = cmd.sub_cmd_no
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