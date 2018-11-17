#!/usr/bin/env python
# -*- coding: utf-8 -*-


import threading
import Queue
import asyncore
import logging
import socket
import time
# ====================================

import proxy
import util.log



# ====================================
class Master(object):
  def __init__(self, queue_out):
    print("Master.init=======================================")
    self.queue_out = queue_out
    self.queue_in = Queue.Queue()
    self.devices_map = {}
    self.last_alive = {}
    self.last_devices_list = None

    self.log = util.log.GetLogger(self.__class__.__name__)

    self.thread = threading.Thread(target=self.Work)
    self._continue = True
    self.thread.setDaemon(True)
    
  
  def _Add(self, serial_number):
    if serial_number in self.devices_map:
      return self.devices_map[serial_number]
    else:
      one = proxy.Proxy(self.queue_out, self.queue_in, serial_number)
      self.devices_map[serial_number] = one
      one.Start()
      return one
  
  def _Remove(self, serial_number):
    if serial_number in self.devices_map:
      self.devices_map[serial_number].ForceStop()
      self.devices_map.remove(serial_number)
      
  
  def ProcessCommand(self, serial_number, command):
    one = self._Add(serial_number)
    one.SendCommand(command)
    
    
  def ProcessIncome(self, msg):
    K_TIMEOUT = 60
    if msg[0] == 'alive':
      self.last_alive[msg[1]] = time.time()
    elif msg[0] == 'device_list':
      self.last_devices_list = msg[1]
      
    #检查所有，干掉超时
    tmp = self.last_alive.copy()
    for key, value in tmp.items():
      if time.time() - value > 60:
        self.last_alive.remove(key)
        self._Remove(key)
        

  def Start(self):
    self.thread.start()
  
  
  def DealWithIncomeQueue(self):
    while not self.queue_in.empty():
      msg = self.queue_in.get()
      self.ProcessIncome(msg)
      self.queue_in.task_done()
  
  def Work(self):
    
    while self._continue:
      self.DealWithIncomeQueue()
      time.sleep(1)
  
  def Join(self):
    self.thread.join()


# ======================================
if __name__ == '__main__':
  pass