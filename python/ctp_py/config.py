#-*- coding: UTF-8 -*-

import os
import util.utility
#===========================================
__MQ_PORT = 6788
__TU_PORT = 6789
__BT_PORT = 6791

__CHECK_UPDATE_PORT = 7900
# __KEEPALIVE_CHECK_UPDATE_PORT = 6801
# __KEEPALIVE_CHECK_UPDATE_SHAKEHAND = 'protobuf://python/update-apk-alive'
__CHECK_UPDATE_SHAKEHAND = 'protobuf://python/update-apk'




__KEEPALIVE_MQ_PORT = 6787
__KEEPALIVE_TU_PORT = 6787
__KEEPALIVE_BT_PORT = 6787

__MQ_SHAKEHAND = 'protobuf://python/mysql'
__TU_SHAKEHAND = 'protobuf://python/cmd+mysql'
__BT_SHAKEHAND = 'protobuf://python/backtesting'



__KEEPALIVE_MQ_SHAKEHAND = 'protobuf://python/mysql-alive'
__KEEPALIVE_TU_SHAKEHAND = 'protobuf://python/cmd-mysql-alive'


__ADDRESS = '127.0.0.1'

#params = {}


def MQParams():
  params = {}
  params["port"] = __MQ_PORT
  params["shakehand"] = __MQ_SHAKEHAND
  params['address'] = __ADDRESS
  return params


def TUParams():
  params = {}
  params["port"] = __TU_PORT
  params["shakehand"] = __TU_SHAKEHAND
  params['address'] = __ADDRESS
  return params

def KeepAliveTUParams():
  params = {}
  params["port"] = __KEEPALIVE_TU_PORT
  params["shakehand"] = __KEEPALIVE_TU_SHAKEHAND
  params['address'] = __ADDRESS
  return params

def KeepAliveMQParams():
  params = {}
  params["port"] = __KEEPALIVE_MQ_PORT
  params["shakehand"] = __KEEPALIVE_MQ_SHAKEHAND
  params['address'] = __ADDRESS
  return params  


def BTParams():
  params = {}
  params["port"] = __BT_PORT
  params["shakehand"] = __BT_SHAKEHAND
  params['address'] = __ADDRESS
  return params

def KeepAliveBTParams():
  params = {}
  params["port"] = __KEEPALIVE_BT_PORT
  params["shakehand"] = __KEEPALIVE_BT_SHAKEHAND
  params['address'] = __ADDRESS
  return params 



def CheckUpdateParams(param):
  params = {}
  
  if os.path.exists(param):
    params["port"] = __CHECK_UPDATE_PORT
  else:
    params["port"] = int(param)
  params["shakehand"] = __CHECK_UPDATE_SHAKEHAND
  params['address'] = __ADDRESS
  return params



# def KeepAliveCheckUpdateParams():
#   params = {}
#   params["port"] = __KEEPALIVE_CHECK_UPDATE_PORT
#   params["shakehand"] = __KEEPALIVE_CHECK_UPDATE_SHAKEHAND
#   params['address'] = __ADDRESS
#   return params


def ServerTest():
  params = {}
  params["port"] = 7788
  params["shakehand"] = __KEEPALIVE_TU_SHAKEHAND
  params['address'] = '127.0.0.1'
  return params



def CreateProp(msg):
  prop = {}
  
  prop['apkPath'] = msg.param[0] + '\\ctp_data\\apk'
  prop['localPath'] = msg.param[0] + '\\ctp_data\\apk_local'
  prop['logPath'] = msg.param[0] + '\\ctp_data\\log'
  prop['debugPath'] = msg.param[0] + '\\ctp_data\\log'

  # 判断操作系统版本，根据版本选择adb版本
  if util.utility.XP:
    prop['adb_exe'] = msg.param[0] + '\\adb_1.0.32\\adb.exe'
  else:
    prop['adb_exe'] = msg.param[0] + '\\adb_1.0.39\\adb.exe'
  
  return prop


def Version():
  return '1.0.0.1'