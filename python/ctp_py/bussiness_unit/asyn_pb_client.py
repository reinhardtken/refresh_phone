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
import traceback
#py3
# from io import BytesIO
#py2
from cStringIO import StringIO
import struct
# import urlparse
import bussiness_unit.protobuf_codec
import util.log
import config
# ========================================



# =============================================
# config.params['address'] =config.params['port']7.0.0.1'
# PORT = 6789
# ===========================================
class ProtobufClient(asyncore.dispatcher):
  
  def __init__(self, **kwargs):
    self.codec = bussiness_unit.protobuf_codec.ProtobufCodec()
    
    
    self.has_send = False
    self.logger = util.log.GetLogger('ProtobufClient')
    self.logger.setLevel(util.log.WARN)

    #py3
    # self.write_buffer = bytes()
    # self.read_buffer = BytesIO()
    #py2
    self.write_buffer = ''
    self.read_buffer = StringIO()
    
    
    if 'socket' in kwargs:
      self.need_connect = False
      asyncore.dispatcher.__init__(self, kwargs['socket'])
    elif 'params' in kwargs:
      self.need_connect = True
      asyncore.dispatcher.__init__(self)
      self.create_socket(socket.AF_INET, socket.SOCK_STREAM)
      self.params = kwargs['params']
      self.address = (self.params['address'], self.params['port'])
      self.logger.debug('connecting to %s', self.address)
      self.SendShakehand()
    
    
    
      
  
  def BeginWork(self):
    if self.need_connect:
      self.connect(self.address)
  
  
  def handle_connect(self):
    # 这个是服务器模式才到的。所以客户端模式永远到不了，所以guard那里的pid始终不对====
    # 以上说法不对，如果writable返回true，就会到一次？？==
    self.logger.debug('handle_connect()===============================')
    print('handle_connect()===============================')
    # self.SendShakehand()



  # def handle_read_event(self):
  #   try:
  #     if self.accepting:
  #       # accepting sockets are never connected, they "spawn" new
  #       # sockets that are connected
  #       self.handle_accept()
  #     elif not self.connected:
  #       self.handle_connect_event()
  #       self.handle_read()
  #     else:
  #       self.handle_read()
  #   except Exception as e:
  #     exstr = traceback.format_exc()
  #     print(exstr)
  
  def handle_close(self):
    self.logger.debug('handle_close()')
    self.close()
  
  def writable(self):
    is_writable = (len(self.write_buffer) > 0)
    if is_writable:
      self.logger.debug('writable() -> %s', is_writable)
    # print('writable()=============================== ', is_writable)
    return is_writable
  
  def readable(self):
    self.logger.debug('readable() -> True')
    return True
  
  def handle_write(self):
    sent = self.send(self.write_buffer)
    self.logger.debug('handle_write() -> "%s"',
                      self.write_buffer[:sent])
    self.write_buffer = self.write_buffer[sent:]
  
  def handle_read(self):
    data = self.recv(8192)
    self.codec.OnMessage(data)
  
  
  
  def SendMessage(self, msg):
    out = self.codec.PackMessage(msg)
    # sent = self.send(out)
    # 20140120 先发后进buf存在问题，如果这个时候buf里面有数据，buf的数据其实是要先于这个数据发送的，结果变成了比这个数据晚发送==
    # 改成统一进buf，再发送buf。效率低就低吧==
    self.write_buffer = self.write_buffer + out
    sent = self.send(self.write_buffer)
    # print('sent number================', sent)
    if sent < len(self.write_buffer):
      # print('new self.write_buffer len ================================ ', len(self.write_buffer) - sent)
      pass
    
    self.write_buffer = self.write_buffer[sent:]
  
  def SendShakehand(self):
    if 'shakehand' in self.params:
      shakehand_string = self.params['shakehand']
  
      # shakehand_string = 'protobuf://python/cmd+mysql'
      # shakehand_string = 'protobuf://python/mysql'
      shakehand_len = len(shakehand_string)
      format_string = 'I ' + str(shakehand_len) + 's '
      print(format_string)
      in_data = (shakehand_len, shakehand_string)
      codec = struct.Struct(format_string)
      out_data = codec.pack(*in_data)
      self.write_buffer = self.write_buffer + (out_data)
      # self.send(out_data)
  
  
  def RegisterMessageHandler(self, handler):
    self.codec.RegisterMessageHandler(handler)
  
  def RegisterMessageHandlerWithName(self, handler, name):
    self.codec.RegisterMessageHandlerWithName(handler, name)
  
  def RegisterModuleHandler(self, handler):
    self.codec.RegisterModuleHandler(handler)

  def RegisterParserHandler(self, name, handler):
    self.codec.RegisterParserHandler(name, handler)

# ======================================================================
if __name__ == '__main__':
  pc = ProtobufClient(params=config.params)
  asyncore.loop()


