#!/usr/bin/env python
# encoding: utf-8
#
# Copyright (c) 2009 Doug Hellmann All rights reserved.
#
"""
"""

__version__ = "$Id$"
#end_pymotw_header

import asyncore
#import logging
import socket
from cStringIO import StringIO
import struct
import sys
#import urlparse


#========================================
import pb.cc_comunication_pb2
import pb.cp_comunication_pb2
import protobuf_codec
import config
import util.log
#=============================================
#config.params['address'] =config.params['port']7.0.0.1'
#PORT = 6789
#===========================================
class ProtobufClient(asyncore.dispatcher):

    def __init__(self, params):
        #self.url = url
        self.params = params
        self.codec = protobuf_codec.ProtobufCodec()
        #handle = protobuf_codec.PrintHandler('ctp.c2c.Command')
        #self.codec.RegisterMessageHandler(handle)
        handle = protobuf_codec.PrintHandler('ctp.cp')
        self.codec.RegisterModuleHandler(handle)

        self.has_send = False
        self.logger = util.log.GetLogger('ProtobufClient')
        self.logger.setLevel(util.log.WARN)
        asyncore.dispatcher.__init__(self)
        self.write_buffer = ''
        self.read_buffer = StringIO()
        self.create_socket(socket.AF_INET, socket.SOCK_STREAM)
        self.address = (self.params['address'], self.params['port'])
        self.logger.debug('connecting to %s', self.address)

        self.SendShakehand()

        self.shutdown_when_close = False
     
    """ 
    def __getattr__(self, attrname):
        if attrname == 'RegisterMessageHandler' or attrname == 'RegisterModuleHandler':
            print('ProtobufClient call delegate', attrname)
            return getattr(self.codec, attrname)"""

    def Connect(self):
        self.connect(self.address)    

    def handle_connect(self):
        #这个是服务器模式才到的。所以客户端模式永远到不了，所以guard那里的pid始终不对====
        #以上说法不对，如果writable返回true，就会到一次？？==
        self.logger.debug('handle_connect()===============================')
        print('handle_connect()===============================')
        #self.SendShakehand()

    def handle_close(self):
        self.logger.debug('handle_close()')
        self.close()

        if self.shutdown_when_close:
          sys.exit()

    def writable(self):
        #if self.has_send == True:
        #    return False
        #else:
        #    return True
        is_writable = (len(self.write_buffer) > 0)
        if is_writable:
            self.logger.debug('writable() -> %s', is_writable)
        #print('writable()=============================== ', is_writable)
        return is_writable
    
    def readable(self):
        self.logger.debug('readable() -> True')
        return True

    def handle_write(self):
        #self.send_test()
        
        sent = self.send(self.write_buffer)
        self.logger.debug('handle_write() -> "%s"',
                          self.write_buffer[:sent])
        self.write_buffer = self.write_buffer[sent:]


    def handle_read(self):
        data = self.recv(8192)
        self.codec.OnMessage(data)

    def send_test(self):
        self.has_send = True
        c2c = pb.cc_comunication_pb2.Command()
        c2c.cmd = "hello"
        c2c.params = "world"
        out = self.codec.PackMessage(c2c)
        self.send(out)

    def Send_cp_comunication_pb2_Command_Msg(self, msg):
        cp = pb.cp_comunication_pb2.Command()
        cp.cmd = msg['c']
        for v in msg['p']:
            cp.params.append(v)
        #c2c.params = msg['p'][0]
        out = self.codec.PackMessage(cp)
        self.send(out)

    def SendMsg(self, msg):
        out = self.codec.PackMessage(msg)
        #sent = self.send(out)
        #20140120 先发后进buf存在问题，如果这个时候buf里面有数据，buf的数据其实是要先于这个数据发送的，结果变成了比这个数据晚发送==
        #改成统一进buf，再发送buf。效率低就低吧==
        self.write_buffer = self.write_buffer + out
        sent = self.send(self.write_buffer)
        #print('sent number================', sent)
        if sent < len(self.write_buffer):
          #print('new self.write_buffer len ================================ ', len(self.write_buffer) - sent)
          pass
        
        self.write_buffer = self.write_buffer[sent:]

    def SendShakehand(self):
        shakehand_string = self.params['shakehand']

        #shakehand_string = 'protobuf://python/cmd+mysql'
        #shakehand_string = 'protobuf://python/mysql'
        shakehand_len = len(shakehand_string)
        format_string = 'I ' + str(shakehand_len) + 's '
        print(format_string)
        in_data = (shakehand_len, shakehand_string)
        codec = struct.Struct(format_string)
        out_data = codec.pack(*in_data)
        self.write_buffer = self.write_buffer + (out_data)
        #self.send(out_data)


    def RegisterMessageHandler(self, handler):
        self.codec.RegisterMessageHandler(handler)

    def RegisterMessageHandlerWithName(self, handler, name):
        self.codec.RegisterMessageHandlerWithName(handler, name)

    def RegisterModuleHandler(self, handler):
        self.codec.RegisterModuleHandler(handler)
#======================================================================
if __name__ == '__main__':
    logging.basicConfig(level=logging.DEBUG,
                        format='%(name)s: %(message)s',
                        )

    pc = ProtobufClient(config.params)

    logging.debug('LOOP STARTING')

    asyncore.loop()

    logging.debug('LOOP DONE')

