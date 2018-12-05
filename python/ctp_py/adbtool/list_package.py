#!/usr/bin/env python
# encoding: utf-8


import re
import traceback

import util
import base


class Command(base.AdbCommandBase):
  cmd = 'shell pm list packages'
  re_package = re.compile(r'package:(.*)')
  
  def __init__(self, serial_no, callback_exit):
    super(Command, self).__init__(None, None, self._callback_wrapper)
    self.log = util.log.GetLogger(self.__class__.__name__)

    self.serial_no = serial_no
    self.step = 0
    self.package_list = []
    self.my_callback_exit = callback_exit
  
  def _callback_wrapper(self, code):
    if self.my_callback_exit is not None:
      try:
        self.my_callback_exit(self.package_list)
      except Exception as e:
        pass
  
  def _BuildCmd(self):
    self.cmd_stack.append(self._BuildSerialNo())
    self.cmd_stack.append(Command.cmd)
    return super(Command, self)._BuildCmd()
  
  
  def _BuildSerialNo(self):
    if self.serial_no is not None:
      return ' -s {} '.format(self.serial_no)
    else:
      return ''
  
  def Parser(self, line):
    '''
    package:com.letv.android.FuseWire
    package:com.qualcomm.qti.auth.sampleextauthservice
    package:com.uei.quicksetsdk.letv
    package:com.letv.android.quicksearchbox
    '''
    if len(line):
      print(line)
      self.log.info(line)
    
      if 'package:' in line:
        package = self.re_package.search(line).group(1)
        self.package_list.append(package)
        return (True, None)
      
    
    return (True, None)