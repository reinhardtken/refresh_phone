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
import struct
#import urlparse
import os

#========================================
import pb.guard_comunication_pb2
import config
import asyn_pb_client
#=============================================
#config.params['address'] =config.params['port']7.0.0.1'
#PORT = 6789
#===========================================
class KeepAliveProtobufClient(asyn_pb_client.ProtobufClient):

    def __init__(self, params):
        asyn_pb_client.ProtobufClient.__init__(self, params)          

    def SendShakehand(self):
        #follow can't work
        #argument 1 must be type, not classobj
        #super(KeepAliveProtobufClient, self).SendShakehand()

        asyn_pb_client.ProtobufClient.SendShakehand(self)
        self.SendKeepAlive()

    def SendKeepAlive(self):
        self.has_send = True
        report = pb.guard_comunication_pb2.ReportPid()
        report.process_id = int(os.getpid())
        print("the pid ========", report.process_id)
        out = self.codec.PackMessage(report)
        self.write_buffer = self.write_buffer + (out)
        #self.send(out)
#======================================================================
if __name__ == '__main__':
    #logging.basicConfig(level=logging.DEBUG,
    #                    format='%(name)s: %(message)s',
    #                    )

    pc = KeepAliveProtobufClient(config.params)

    logging.debug('LOOP STARTING')

    asyncore.loop()

    logging.debug('LOOP DONE')

