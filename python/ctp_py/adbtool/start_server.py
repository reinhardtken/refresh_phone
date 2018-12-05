#!/usr/bin/env python
# encoding: utf-8

import subprocess
import re
import traceback

import util
import base


class Command(base.AdbCommandBase):
  cmd = 'start-server'
  re_port = re.compile(r'^[\*\.: A-Za-z]*([0-9]*) \*$')
  port_seed = 5037
  counter = 0
  MAX_TRY = 500

  zombie_ports = None
  zombie_ports_index = 0
  
  def __init__(self, suggest_port=None, callback_succ=None, callback_failed=None):
    super(Command, self).__init__(self._my_succ_callback, callback_failed)
    self.log = util.log.GetLogger(self.__class__.__name__)
    self.my_callback = callback_succ
    self.port = None
    self.zombie_mode = False
    if isinstance(suggest_port, list):
      #复用老port，这些都是牛逼软件，没法kill
      self.ZombieInit(suggest_port)
    else:
      self.NormalInit(suggest_port)
      # self.suggest_port = suggest_port
    # if self.suggest_port is None:
    #   self.suggest_port = Command.GenPort()


  def NormalInit(self, port):
    self.log.info('NormalInit  ' + str(port))
    self.suggest_port = port
    if self.suggest_port is None:
      self.suggest_port = Command.GenPort()

  def ZombieInit(self, ports):
    #在这种情况下，启动server其实已经没有任何意义了
    self.log.warning('ZombieInit  ')
    for one in ports:
      self.log.warning(str(one))
      
    self.zombie_mode = True
    Command.zombie_ports = ports
    # Command.zombie_ports_index = 0

  def NextZombiePort(self):
    #总是挨个遍历zombie_ports的数据
    if Command.zombie_ports_index + 1 < len(Command.zombie_ports):
      Command.zombie_ports_index += 1
    else:
      Command.zombie_ports_index = 0

    return Command.zombie_ports[Command.zombie_ports_index]



  def Execute(self):
    if self.zombie_mode:
      self.port = self.NextZombiePort()
      self.log.warning('zombie_mode ANDROID_ADB_SERVER_PORT = ' + str(self.port))
      base.AdbCommandBase.global_env['ANDROID_ADB_SERVER_PORT'] = str(self.port)
      return
    else:
      super(Command, self).Execute()
    pass


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
    1.0.39
     * daemon not running. starting it now at tcp:5038 *
     * daemon started successfully *
      1.0.32
     '* daemon not running. starting it now on port 5038 *'
    1.0.40
    * daemon not running; starting now at tcp:5678
    * daemon started successfully

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