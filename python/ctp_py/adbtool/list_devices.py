#!/usr/bin/env python
# encoding: utf-8


import re
import traceback

import util
import base


class Command(base.AdbCommandBase):
  cmd = 'devices -l'
  
  def __init__(self, callback_exit):
    super(Command, self).__init__(None, None, self._callback_wrapper)
    self.log = util.log.GetLogger(self.__class__.__name__)
    
    self.step = 0
    self.devices_list = []
    self.my_callback_exit = callback_exit
  
  def _callback_wrapper(self, code):
    if self.my_callback_exit is not None:
      try:
        self.my_callback_exit(self.devices_list)
      except Exception as e:
        pass
  
  def _BuildCmd(self):
    self.cmd_stack.append(Command.cmd)
    return super(Command, self)._BuildCmd()
  
  def Parser(self, line):
    '''
    List of devices attached


    List of devices attached
    aa1ee7d1               device product:LeMax2_CN model:Le_X820 device:le_x2
    '''
    if len(line):
      print(line)
      self.log.info(line)
      
    if 'error' in line:
      error = self.adb_error_re.search(line).group(1)
      return (False, error)
    # if line.startswith('adb: error: '):
    #   return (False, line)
    elif 'List of devices attached' in line:
      self.step = 1
    elif self.step == 1:
      one_device = {}
      first_space = line.find(' ')
      if first_space != -1:
        one_device['serial_no'] = line[:first_space]
        rest = line[first_space:].strip()
        part = rest.split(' ')
        if len(part) >= 4:
          one_device['product'] = part[1].split(':')[1]
          one_device['model'] = part[2].split(':')[1]
          one_device['device'] = part[3].split(':')[1]
          self.devices_list.append(one_device)
          return (True, None)
    
    return (True, None)