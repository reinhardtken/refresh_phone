#!/usr/bin/env python
# encoding: utf-8

import subprocess
import re
import traceback

import util
import base


class Command(base.AdbCommandBase):
  cmd = 'start-server'
  re_port = re.compile(r'tcp:(.*)\*')
  port_seed = 5037
  counter = 0
  MAX_TRY = 500
  
  def __init__(self, suggest_port=None, callback_succ=None, callback_failed=None):
    super(Command, self).__init__(self._my_succ_callback, callback_failed)
    self.log = util.log.GetLogger(self.__class__.__name__)
    self.my_callback = callback_succ
    self.port = None
    self.suggest_port = suggest_port
    if self.suggest_port is None:
      self.suggest_port = Command.GenPort()
      
      
  @staticmethod
  def GenPort():
    Command.counter += 1
    if Command.counter > Command.MAX_TRY:
      Command.counter = 0
    return Command.port_seed + Command.counter
  
  
  def _my_succ_callback(self, content):
    if content == base.AdbCommandBase.SUCCESS:
      if self.port is not None:
        self.log.info('ANDROID_ADB_SERVER_PORT = ' + str(self.port))
        base.AdbCommandBase.global_env['ANDROID_ADB_SERVER_PORT'] = str(self.port)
    
    if self.my_callback is not None:
      self.my_callback(content)
      
      
  def GetReturnCode(self):
    if self.port is not None:
      return (True, self.port)
    else:
      #永远假设不会失败？？？
      return (True, None)
    
  
  def _BuildCmd(self):
    self.cmd_stack.append(Command.cmd)
    return super(Command, self)._BuildCmd()


  def _BuildEnv(self):
    self.env = {'ANDROID_ADB_SERVER_PORT': str(self.suggest_port)}
    return super(Command, self)._BuildEnv()
  
  
  def Parser(self, line):
    '''
    * daemon not running. starting it now at tcp:5038 *
    * daemon started successfully *
    '''
    try:
      print(line)
      self.log.info(line)
      if 'daemon not running' in line:
        self.port = int(self.re_port.search(line).group(1))
        return (True, self.port)
      elif 'daemon started successfully' in line:
        return (True, base.AdbCommandBase.SUCCESS)
      
      return (False, None)
    except Exception as e:
      exstr = traceback.format_exc()
      print(exstr)
      self.log.info(exstr)
      return (False, e)
    
    
 
def test():
  one = Command(5041)
  one.Execute()
 
   
#======================================================
if __name__ == '__main__':
  one = Command(5039)
  one.Execute()