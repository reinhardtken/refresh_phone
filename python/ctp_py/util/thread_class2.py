import threading
import queue
import asyncore
import logging
import time
#================================================================
class ThreadClass(object):
  def __init__(self, processMessage, work=None):
    self.queue_ = queue.Queue()
    self.processMessage = processMessage
    self.work = work
    if self.work is None:
      self.work = self.EmptyWork

    self.thread = threading.Thread(target=self.WorkProxy)
    
  
  def queue(self):
    return self.queue_
  
  
  def EmptyWork(self):
    time.sleep(0.001)


  def Start(self):
    # self.thread = threading.Thread(target=self.WorkProxy)
    self.thread.start()
    # print('after start')

  def ProcessIncomeQueue(self):
    while not self.queue_.empty():
      msg = self.queue_.get()
      self.processMessage(msg)
      self.queue_.task_done()



  def BeforeLoop(self):
    pass

  def Continue(self):
    return True

  def WorkProxy(self):
    self.BeforeLoop()
    while self.Continue():
      self.work()
      self.ProcessIncomeQueue()


  def Join(self):
    self.thread.join()

#==========================================================