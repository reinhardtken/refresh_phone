#!/usr/bin/env python
# encoding: utf-8


import re
import traceback

import util
import base


class Command(base.AdbCommandBase):
  
  def __init__(self, serial_no, package_name, callback_succ, callback_fail):
    super(Command, self).__init__(callback_succ, callback_fail)
    self.log = util.log.GetLogger(self.__class__.__name__)
    self.serial_no = serial_no
    self.package_name = package_name
    
    # 接收到某条信息后，就认为后续信息无意义，不再解析避免干扰
    self.stop_parser = False
  
  def _BuildCmd(self):
    self.cmd_stack.append(self._BuildSerialNo())
    self.cmd_stack.append(self._BuildUninstall())
    return super(Command, self)._BuildCmd()
  
  def _BuildSerialNo(self):
    if self.serial_no is not None:
      return ' -s {} '.format(self.serial_no)
    else:
      return ''
  
  def _BuildUninstall(self):
    return ' uninstall  {}'.format(self.package_name)
  
  # 这块后面可能需要根据不同手机设置parser
  # 可以通过装完后枚举package list的方式来判断是不是装成功了
  def Parser(self, line):
    '''
    letv##################################################
    Success
    Failure [DELETE_FAILED_INTERNAL_ERROR]
    '''

    
    try:
      if len(line):
        print(line)
        self.log.info(line)
        if line == 'Success':
          self.stop_parser = True
          return (True, 'Success')
        elif 'Failure' in line:
          error = self.adb_install_failure.search(line).group(1)
          self.log.warning(error)
          return (False, error)
        else:
          return (False, None)
    except Exception as e:
      exstr = traceback.format_exc()
      print(exstr)
      self.log.info(exstr)
      return (False, e)