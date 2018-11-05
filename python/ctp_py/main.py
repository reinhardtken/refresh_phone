#!/usr/bin/env python
# -*- coding: utf-8 -*-

import Queue
import sys

import time
import os
#=====================================
import network
import check_update
import config
import util.log



import util.exception_handler
import util.prop




def CheckUpdateMode(param):
  #param 可能是两种情况
  # 1 一个配置文件路径
  # 2 一个port ，用于建立socket链接
  
    
  # used from cmd to network
  msg_queue = Queue.Queue()
  # used from network to mysql
  msg_queue2 = Queue.Queue()

  network_object = network.Network(msg_queue, config.CheckUpdateParams(param), None)
  checkUpdate = check_update.CheckUpdateApkList(param, msg_queue2, msg_queue)
  checkUpdate.Init()
  checkUpdate.RegisterHandler(network_object.asyn_pb_client)
  checkUpdate.Run()

  network_object.Run()
  print('after network_object.Run()========================')

  


#=======================================================



if __name__ == '__main__':
  print('<=============(version=' + config.Version() + ')==================>')
  print(sys.argv)
  if len(sys.argv) >= 2:
    param = sys.argv[1]
    exh = util.exception_handler.ExceptHookHandler()

    CheckUpdateMode(param)
    


  