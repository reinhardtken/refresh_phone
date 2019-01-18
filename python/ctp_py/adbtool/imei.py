#!/usr/bin/env python
# encoding: utf-8


import re
import traceback


#########################################################
if __name__ == '__main__':
  import sys
  sys.path.append(r'C:\workspace\code\chromium24\src\phone\python\ctp_py')
#########################################################

import util
import base



class Command(base.AdbCommandBase):
  cmd = ' shell service call iphonesubinfo 1'
  #pattern = re.compile(r'^[A-Za-z0-9 (:\\]*\'([0-9\. ]*)\\*\'*$')
  
  def __init__(self, serial_no, callback_succ=None, callback_fail=None):
    super(Command, self).__init__(callback_succ, callback_fail, None)
    self.log = util.log.GetLogger(self.__class__.__name__)
    
    self.serial_no = serial_no
    self.imei = ''
  
  
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
    Result: Parcel(
  0x00000000: 00000000 0000000f 00360038 00360039 '........8.6.9.6.'
  0x00000010: 00360033 00320030 00300030 00330034 '3.6.0.2.0.0.4.3.'
  0x00000020: 00320033 00000030                   '3.2.0...        ')
    '''
    try:
      print(line)
      self.log.info(line)
      if '0x000000' in line:
        index = line.find("'")
        index2 = line.find("'", index+1)
        #part = self.pattern.search(line).group(1)
        if index != -1 and index != -1:
          part = line[index+1 : index2]
          self.log.info(part)
          self.imei += part

      return (True, None)
    except Exception as e:
      exstr = traceback.format_exc()
      print(exstr)
      self.log.info(exstr)
      return (False, e)
    
    
  def GetIMEI(self):
    if len(self.imei):
      self.imei = self.imei.strip()
      self.imei = self.imei.replace(".", '')
      
    return self.imei
    
    
    
    
    
    
# ======================================================
if __name__ == '__main__':
  import start_server
  import base
  base.AdbCommandBase.adb = r'C:\workspace\code\chromium24\src\build\Debug\adb_1.0.39\adb'
  port = start_server.Command.GenPort()
  # 产生新server
  start_server = start_server.Command(port)
  start_server.Execute()
  succ, pid = start_server.GetReturnCode()
  #########################################
  one = Command('aa1ee7d1')
  one.Execute()
  imei = one.GetIMEI()
  print(imei)