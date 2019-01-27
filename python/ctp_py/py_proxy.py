#!/usr/bin/env python
# encoding: utf-8


import Queue
import sys
import util
import time
import traceback
import asyncore
import logging
#########################################################
import config
import util.log
import bussiness_unit.asyn_pb_server
import bussiness_unit.network2
import bussiness_unit.asyn_pb_client
import bussiness_unit.base
import util.exception_handler
#https://pypi.org/project/python3-protobuf/#description
#这个包是google官方的，没道理不好使啊。。。
import pb.apk_protomsg_pb2

#py3
_b = (lambda x:x.encode('latin1'))
_s = (lambda x:x.decode('latin1'))
#py2
_b = (lambda x:x)
_s = (lambda x:x)
#########################################################
# 主要是用来处理业务逻辑的，理论上是会有多个的
class PyProxyOne(bussiness_unit.base.Base):
  data_map = {
    'apk.CommandResponse': pb.apk_protomsg_pb2.CommandResponse,
    'market_strategy.Head': None,
  }
  
  def __init__(self, queue):
    super(PyProxyOne, self).__init__(queue)
    self.handler_list.extend(PyProxyOne.data_map.keys())
    self.parser_list.extend(PyProxyOne.data_map.keys())
    
    #发送配置信息
    command = pb.apk_protomsg_pb2.Command()
    command.cmd = 'py_config'
    command.cmd_no = -1
    command.timestamp = 0
    command.param.append(r'C:\workspace\code\chromium24\src\build\Release')
    command.param.append('1')
    self.SendMessage(command)


  
  def ParserMessage(self, name, data):
    if _s(name) in PyProxyOne.data_map and PyProxyOne.data_map[_s(name)] is not None:
      msg = PyProxyOne.data_map[_s(name)]()
      msg.ParseFromString(data)
      return msg
  
  
  def ProcessMessage(self, data):
    # msg[0] socket, msg[1] real data
    msg = data[1]
    if msg.DESCRIPTOR.full_name == 'apk.CommandResponse':
      print(msg)
      # head = pb.market_strategy_pb2.Head()
      # head.serial_number = 1
      # command = pb.market_strategy_pb2.Command()
      # command.head.serial_number = 1
      #
      # command.ms_name = 'test'
      # command.os_name = 'test2'
      # command.type = 'if'
      # command.level = 1
      # command.price = 123.4
      # command.limit_percent = 0.2
      # command.position_cash = 1000
      # command.action = 1
      # command.buy_clean_sell = 1
      # master.py_proxy_one.SendMessage(command)
    

  


    
    
#########################################################
class PyProxyMaster(object):
  
  def __init__(self, handle=PyProxyOne):
    self.network = bussiness_unit.network2.Network()
    self.handle = handle

    params = {}
    params["port"] = 7789
    params['address'] = '127.0.0.1'
    
    # params = config.PyProxyParams()
    server = bussiness_unit.asyn_pb_server.ProtobufServer(params, self)
    
    self.network.Add(server)
    self.network.Start()
    
    self.py_proxy_one = None
    
    
  def Create(self, socket, address):
    try:
      if self.py_proxy_one is None:
        self.py_proxy_one = self.handle(self.network.queue())
        self.py_proxy_one.Start()

      socket_wrapper = bussiness_unit.asyn_pb_client.ProtobufClient(serverClient=True, socket=socket)
      self.py_proxy_one.ConnectMultiple(socket_wrapper)
      self.network.Add(socket_wrapper)
    except Exception as e:
      exstr = traceback.format_exc()
      print(exstr)



    
    

#########################################################



if __name__ == '__main__':
  # from google.protobuf import descriptor_pb2
  # test = descriptor_pb2.DescriptorProto()
  # msg = pb.market_strategy_pb2.Command()
  # # msg.head.serial_number = 1
  exh = util.exception_handler.ExceptHookHandler()
  master = PyProxyMaster()