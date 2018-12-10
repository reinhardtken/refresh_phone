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
    SendInstallApkResponse(self.host.GenInstallApkResponse(error=consts.ERROR_CODE_OK, progress=progress))
  
  def CallbackFail(self, progress):
    SendInstallApkResponse(self.host.GenInstallApkResponse(error=consts.ERROR_CODE_PYADB_INSTALL_APK_FAILED,
                        stage='完成'.decode('utf-8'), progress=progress))


class CallbackUninstallObject():
  def __init__(self, host):
    self.host = host
  
  
  def CallbackSucc(self, progress):
    stage = '进行中'
    if progress == 'Success':
      stage = '完成删除老包'
      SendInstallApkResponse(self.host.GenInstallApkResponse(error=consts.ERROR_CODE_OK,
                        stage=stage.decode('utf-8')))
  
  def CallbackFail(self, progress):
    SendInstallApkResponse(self.host.GenInstallApkResponse(error=consts.ERROR_CODE_PYADB_UNINSTALL_APK_FAILED,
                        stage='删除老包失败'.decode('utf-8')))
    
    
    

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
  response.sub_cmd_no = cmd.sub_cmd_no
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
  response.package_name = r['package_name']
  response.progress = r['progress']
  response.type = r['type']
  response.serial_number = r['serial_number']
  response.time_max = r['time_max']
  response.package_size = r['package_size']
  response.stage = r['stage']
  response.adb_message = r['adb_message']
  
  if r['info'] is not None:
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