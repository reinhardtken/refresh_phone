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
from cStringIO import StringIO
import struct
# import urlparse
import util.log
import config
import util.log


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
    self.read_buffer = StringIO()
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
  
  


# ======================================================================
if __name__ == '__main__':
  pc = ProtobufServer(config.ServerTest())
  asyncore.loop()


