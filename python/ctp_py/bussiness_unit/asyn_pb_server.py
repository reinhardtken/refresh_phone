#!/usr/bin/env python
# encoding: utf-8
#
# Copyright (c) 2009 Doug Hellmann All rights reserved.
#
"""
"""

__version__ = "$Id$"
# end_pymotw_header

import asyncore
# import logging
import socket
from io import BytesIO
import struct
# import urlparse
import util.log
import config
# ========================================


# =============================================
# config.params['address'] =config.params['port']7.0.0.1'
# PORT = 6789
# ===========================================
class ProtobufServer(asyncore.dispatcher):
  
  #服务端接收请求的处理器，可能彼此没关联，也可能有关联。看具体业务逻辑
  #所以接受一个工厂对象，工厂决定如何处理
  def __init__(self, params, handlerFactory):
    self.params = params
    self.handlerFactory = handlerFactory

    
    self.has_send = False
    self.logger = util.log.GetLogger('ProtobufServer')
    self.logger.setLevel(util.log.WARN)
    asyncore.dispatcher.__init__(self)
    self.write_buffer = ''
    self.read_buffer = BytesIO()
    self.create_socket(socket.AF_INET, socket.SOCK_STREAM)
    self.address = (self.params['address'], self.params['port'])
    self.logger.debug('connecting to %s', self.address)
    

  def BeginWork(self):
    self.bind(self.address)
    self.listen(10)

    
  def handle_accept(self):
    (conn_sock, client_address) = self.accept()
    # if self.verify_request(conn_sock, client_address):
    self.ProcessRequest(conn_sock, client_address)
    
  
  def ProcessRequest(self, conn_sock, client_address):
    self.logger.info("conn_made: client_address=%s:%s" % \
             (client_address[0],
              client_address[1]))
    self.handlerFactory.Create(conn_sock, client_address)
  
  # def handle_connect(self):
  #   # 这个是服务器模式才到的。所以客户端模式永远到不了，所以guard那里的pid始终不对====
  #   # 以上说法不对，如果writable返回true，就会到一次？？==
  #   self.logger.debug('handle_connect()===============================')
  #   print('handle_connect()===============================')
  #   # self.SendShakehand()
  
  def handle_close(self):
    self.logger.debug('handle_close()')
    self.close()
  
  def writable(self):
    # if self.has_send == True:
    #    return False
    # else:
    #    return True
    is_writable = (len(self.write_buffer) > 0)
    if is_writable:
      self.logger.debug('writable() -> %s', is_writable)
    # print('writable()=============================== ', is_writable)
    return is_writable
  
  def readable(self):
    self.logger.debug('readable() -> True')
    return True
  
  def handle_write(self):
    # self.send_test()
    
    sent = self.send(self.write_buffer)
    self.logger.debug('handle_write() -> "%s"',
                      self.write_buffer[:sent])
    self.write_buffer = self.write_buffer[sent:]
  
  def handle_read(self):
    data = self.recv(8192)
    self.codec.OnMessage(data)
  
  # def send_test(self):
  #   self.has_send = True
  #   c2c = pb.cc_comunication_pb2.Command()
  #   c2c.cmd = "hello"
  #   c2c.params = "world"
  #   out = self.codec.PackMessage(c2c)
  #   self.send(out)
  
  # def Send_cp_comunication_pb2_Command_Msg(self, msg):
  #   cp = pb.cp_comunication_pb2.Command()
  #   cp.cmd = msg['c']
  #   for v in msg['p']:
  #     cp.params.append(v)
  #   # c2c.params = msg['p'][0]
  #   out = self.codec.PackMessage(cp)
  #   self.send(out)
  
  # def SendMsg(self, msg):
  #   out = self.codec.PackMessage(msg)
  #   # sent = self.send(out)
  #   # 20140120 先发后进buf存在问题，如果这个时候buf里面有数据，buf的数据其实是要先于这个数据发送的，结果变成了比这个数据晚发送==
  #   # 改成统一进buf，再发送buf。效率低就低吧==
  #   self.write_buffer = self.write_buffer + out
  #   sent = self.send(self.write_buffer)
  #   # print('sent number================', sent)
  #   if sent < len(self.write_buffer):
  #     # print('new self.write_buffer len ================================ ', len(self.write_buffer) - sent)
  #     pass
  #
  #   self.write_buffer = self.write_buffer[sent:]
  
  # def SendShakehand(self):
  #   shakehand_string = self.params['shakehand']
  #
  #   # shakehand_string = 'protobuf://python/cmd+mysql'
  #   # shakehand_string = 'protobuf://python/mysql'
  #   shakehand_len = len(shakehand_string)
  #   format_string = 'I ' + str(shakehand_len) + 's '
  #   print(format_string)
  #   in_data = (shakehand_len, shakehand_string)
  #   codec = struct.Struct(format_string)
  #   out_data = codec.pack(*in_data)
  #   self.write_buffer = self.write_buffer + (out_data)
  #   # self.send(out_data)
  #
  # def RegisterMessageHandler(self, handler):
  #   self.codec.RegisterMessageHandler(handler)
  #
  # def RegisterMessageHandlerWithName(self, handler, name):
  #   self.codec.RegisterMessageHandlerWithName(handler, name)
  #
  # def RegisterModuleHandler(self, handler):
  #   self.codec.RegisterModuleHandler(handler)


# ======================================================================
if __name__ == '__main__':
  pc = ProtobufServer(config.ServerTest())
  asyncore.loop()


