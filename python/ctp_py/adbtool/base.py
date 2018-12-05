#!/usr/bin/env python
# encoding: utf-8

import subprocess
import re
import traceback
import os
import util.log


import cmdtool.base

#这文章和我遇到的问题很像，重载流的时候，无法获得足够的数据
#https://www.jianshu.com/p/afdb55040b3f


class AdbCommandBase(cmdtool.base.CommandBase):
  adb_error_re = re.compile(r'error: (.*)')
  adb_install_failure = re.compile(r'Failure \[(.*)\]')
  
  #adb = r'C:\workspace\code\chromium24\src\build\Debug\adb\adb'
  adb = None
  
  def __init__(self, callback_succ=None, callback_fail=None, callback_exit=None, callback_exception=None):
    super(AdbCommandBase, self).__init__(callback_succ, callback_fail, callback_exit, callback_exception)
    self.log = util.log.GetLogger(self.__class__.__name__)
    if AdbCommandBase.adb is not None:
      self.log.info('adb path: ' + AdbCommandBase.adb)
      self.cmd_stack.append(AdbCommandBase.adb)
    else:
      self.log.error('adb path:  is null')
      
      
  
  def Execute(self):
    if 'ANDROID_ADB_SERVER_PORT' in self.global_env:
      self.log.info('ANDROID_ADB_SERVER_PORT  ' + self.global_env['ANDROID_ADB_SERVER_PORT'])
    else:
      self.log.error('ANDROID_ADB_SERVER_PORT  not exist')
    return super(AdbCommandBase, self).Execute()
  
  