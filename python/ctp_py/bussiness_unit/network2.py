#!/usr/bin/env python
# encoding: utf-8


import threading
#py3
# import queue
#py2
import Queue
import asyncore
import logging
import socket
# ====================================


#参数是网络对象的list
#每个对象有一个SendMsg用于发送数据
#每个对象有一个BeginWork，用于在io线程的初始化
# ====================================
class Network(object):
  def __init__(self):
    print("Network.init=======================================")
    #send queue ,所有send sock共用这个queue
    #py3
    # self.queue_ = queue.Queue()
    #py2
    self.queue_ = Queue.Queue()
    self.work_list = set()
    self.to_work_list = set()
    self.thread = threading.Thread(target=self.Work)
    
  
  def queue(self):
    return self.queue_
  
  def Add(self, one):
    self.to_work_list.add(one)
  
  def Start(self):
    self.thread.start()

  
  
  def DealWithIncomeQueue(self):
    while not self.queue_.empty():
      msg = self.queue_.get()
      #self.socket_wrapper.SendMsg(msg)
      #msg[0]是套接字对象，[1]是数据
      msg[0].SendMessage(msg[1])
      
      # if msg[0] in self.work_list:
      #   msg[0].SendMessage(msg[1])
      # else:
      #   print('error!!!! unknown target')
      self.queue_.task_done()
  
  
  def Work(self):
    while True:  # self.asyn_pb_client.socket_map:
      for one in self.to_work_list:
        one.BeginWork()
      
      self.work_list.update(self.to_work_list)
      self.to_work_list.clear()
      
      asyncore.loop(timeout=0.001, count=1)
      self.DealWithIncomeQueue()
  
  def Join(self):
    self.thread.join()


# ======================================
if __name__ == '__main__':
  network = Network()
  network.Run()
  network.Join()