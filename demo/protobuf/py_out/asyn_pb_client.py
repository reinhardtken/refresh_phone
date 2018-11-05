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
import logging
import socket
from cStringIO import StringIO
#import urlparse
import protobuf_codec

#========================================
import cc_comunication_pb2

class ProtobufClient(asyncore.dispatcher):

    def __init__(self):
        #self.url = url
        self.codec = protobuf_codec.ProtobufCodec()
        #handle = protobuf_codec.PrintHandler('ctp.c2c.Command')
        #self.codec.RegisterMessageHandler(handle)
        #handle = protobuf_codec.PrintHandler('ctp.c2c')
        #self.codec.RegisterModuleHandler(handle)

        self.has_send = False
        self.logger = logging.getLogger('ProtobufClient')
        asyncore.dispatcher.__init__(self)
        self.write_buffer = StringIO()
        self.read_buffer = StringIO()
        self.create_socket(socket.AF_INET, socket.SOCK_STREAM)
        self.address = ('localhost', 6789)
        self.logger.debug('connecting to %s', address)
     

    def DoConnect(self):
        self.connect(self.address)    

    def handle_connect(self):
        self.logger.debug('handle_connect()')
        

    def handle_close(self):
        self.logger.debug('handle_close()')
        self.close()

    def writable(self):
        if self.has_send == True:
            return False
        else:
            return True
        #is_writable = (len(self.write_buffer) > 0)
        #if is_writable:
        #    self.logger.debug('writable() -> %s', is_writable)
        #return is_writable
    
    def readable(self):
        self.logger.debug('readable() -> True')
        return True

    def handle_write(self):
        self.send_test()
        #sent = self.send(self.write_buffer)
        #self.logger.debug('handle_write() -> "%s"',
        #                  self.write_buffer[:sent])
        #self.write_buffer = self.write_buffer[sent:]

    def handle_read(self):
        data = self.recv(8192)
        self.codec.OnMessage(data)

    def send_test(self):
        self.has_send = True
        c2c = cc_comunication_pb2.Command()
        c2c.cmd = "hello"
        c2c.params = "world"
        out = self.codec.PackMessage(c2c)
        self.send(out)
#=========================================================

if __name__ == '__main__':
    logging.basicConfig(level=logging.DEBUG,
                        format='%(name)s: %(message)s',
                        )

    pc = ProtobufClient()

    logging.debug('LOOP STARTING')

    asyncore.loop()

    logging.debug('LOOP DONE')

