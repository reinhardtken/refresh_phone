#!/usr/bin/env python
# -*- coding: utf-8 -*-


import threading
import Queue
import asyncore
import logging
import socket
#====================================
import asyn_pb_client
import asyn_pb_client_keepalive

#====================================
class Network:
  def __init__(self, queue, p1, p2):
    print("Network.init=======================================")
    self.queue = queue

    self.asyn_pb_client = None
    self.asyn_pb_client_keepalive = None

    if p1 is not None:
      self.asyn_pb_client = asyn_pb_client.ProtobufClient(p1)
      #如果连接断开，直接退出
      self.asyn_pb_client.shutdown_when_close = True
    if p2 is not None:
      self.asyn_pb_client_keepalive = asyn_pb_client_keepalive.KeepAliveProtobufClient(p2)

    self.thread = threading.Thread(target=self.Work)

  def Run(self):
    
    self.thread.start()
    print('after start')

  def DealWithIncomeQueue(self):
    #print('DealWithIncomeQueue')
    while not self.queue.empty():
      msg = self.queue.get()
      self.asyn_pb_client.SendMsg(msg)
      self.queue.task_done()


  def Work(self):
    if self.asyn_pb_client is not None:
      self.asyn_pb_client.Connect()
    if self.asyn_pb_client_keepalive is not None:
      self.asyn_pb_client_keepalive.Connect()

    while True:#self.asyn_pb_client.socket_map:
      asyncore.loop(timeout = 1, count = 1)
      self.DealWithIncomeQueue()


  def Join(self):
    self.thread.join()
#======================================    
if __name__ == '__main__':
  network = Network()
  network.Run()
  network.Join()