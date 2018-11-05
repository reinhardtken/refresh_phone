import threading
import Queue
import asyncore
import logging

#================================================================
class ThreadClass:
  def __init__(self, queue):
    self.queue = queue


  def Run(self):
    self.thread = threading.Thread(target = self.WorkProxy)
    self.thread.start()
    print('after start')

  def DealWithIncomeQueue(self):
    while not self.queue.empty():
      msg = self.queue.get()
      self.DealMsg(msg)
      self.queue.task_done()


  def DealMsg(self, msg):
    pass

  def Work(self):
    pass

  def BeforeLoop(self):
    pass

  def Continue(self):
    return True

  def WorkProxy(self):
    self.BeforeLoop()
    while self.Continue():
      self.Work()
      self.DealWithIncomeQueue()


  def Join(self):
    self.thread.join()

#==========================================================    