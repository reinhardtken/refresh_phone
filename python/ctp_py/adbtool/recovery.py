#!/usr/bin/env python
# encoding: utf-8


import re
import traceback

import util
import base


class Command(base.AdbCommandBase):
  cmd = 'reboot recovery'

  
  def __init__(self, serial_no, callback_succ, callback_fail):
    super(Command, self).__init__(callback_succ, callback_fail, None)
    self.log = util.log.GetLogger(self.__class__.__name__)
    
    self.serial_no = serial_no

  
  
  
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
    '''
    try:
      print(line)
      self.log.info(line)
      if 'error' in line:
        error = self.adb_error_re.search(line).group(1)
        self.log.warning(error)
        return (False, error)
      return (True, None)
    except Exception as e:
      exstr = traceback.format_exc()
      print(exstr)
      self.log.info(exstr)
      return (False, e)