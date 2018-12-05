#!/usr/bin/env python
# encoding: utf-8

import subprocess
import re
import traceback
import os
import util.log


class CommandBase(object):
  SUCCESS = 'SUCCESS'
  # 如果成功通过ANDROID_ADB_SERVER_PORT设置了自己的端口，则下面的类变量被设置
  global_env = os.environ.copy()
  #for test
  global_env['ANDROID_ADB_SERVER_PORT'] = '5678'
  
  # adb = None
  
  def __init__(self, callback_succ=None, callback_fail=None, callback_exit=None, callback_exception=None):
    self.log = util.log.GetLogger(self.__class__.__name__)
    
    self.__callback_succ = callback_succ
    self.__callback_fail = callback_fail
    self.__callback_exit = callback_exit
    self.__callback_exception = callback_exception
    self.cmd_stack = []
    self.env = None
    self.use_global_env = True
    self.shell = False
    self.returncode = None
  
  def Parser(self, line):
    return (False, None)
  
  
  def GetReturnCode(self):
    return self.returncode
  
  
  def CallbackSucc(self, content):
    if self.__callback_succ is not None:
      try:
        self.__callback_succ(content)
      except Exception as e:
        pass
  
  def CallbackFail(self, content):
    if self.__callback_fail is not None:
      try:
        self.__callback_fail(content)
      except Exception as e:
        pass
  
  def CallbackExit(self, code):
    if self.__callback_exit is not None:
      try:
        self.__callback_exit(code)
      except Exception as e:
        pass
  
  def CallbackException(self, e):
    if self.__callback_exception is not None:
      try:
        self.__callback_exception(e)
      except Exception as e:
        pass
  
  def _BuildCmd(self):
    out = ''
    for one in self.cmd_stack:
      out += one + ' '
    
    return out
  
  def _BuildEnv(self):
    if self.env is not None:
      if self.use_global_env and CommandBase.global_env is not None:
        tmp = CommandBase.global_env.copy()
        tmp.update(self.env)
        return tmp
      else:
        return self.env
    
    elif self.use_global_env and CommandBase.global_env is not None:
      return CommandBase.global_env.copy()
    else:
      return None
    
   
  @staticmethod
  def __ReadLine(p):
    p.stdout.flush()
    line = p.stdout.readline()
    line = line.strip()
    if not len(line):
      p.stderr.flush()
      line = p.stderr.readline()
      line = line.strip()
    
    return line
    
    
  
  def Execute(self):
    try:
      cmd = self._BuildCmd()
      env = self._BuildEnv()
      self.log.info('CommandBase cmd: ' + cmd)
      p = subprocess.Popen(cmd, env=env, shell=self.shell, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
      while p.poll() is None:
  
        line = CommandBase.__ReadLine(p)
        
        succ, content = self.Parser(line)
        if succ:
          self.CallbackSucc(content)
        else:
          self.CallbackFail(content)
          self.log.warning('CommandBase kill happen: ' + cmd)
          p.kill()
          break

      line = CommandBase.__ReadLine(p)
      while len(line) > 0:
        line = line.strip()
        succ, content = self.Parser(line)
        if succ:
          self.CallbackSucc(content)
        else:
          self.CallbackFail(content)
          break
        line = CommandBase.__ReadLine(p)
      
      self.returncode = p.returncode
      self.CallbackExit(p.returncode)
    
    except Exception as e:
      exstr = traceback.format_exc()
      print(exstr)
      self.log.info(exstr)
      self.CallbackException(e)
      pass