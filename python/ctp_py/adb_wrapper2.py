#!/usr/bin/env python
# encoding: utf-8

import subprocess
import re


import util


class AdbCommandBase(object):
  adb_error_re = re.compile(r'error: (.*)')
  adb_install_failure = re.compile(r'Failure \[(.*)\]')

  # adb = r'C:\workspace\code\chromium24\src\build\Debug\adb\adb'
  adb = None
  
  
  def __init__(self, callback_succ=None, callback_fail=None, callback_exit=None, callback_exception=None):
    self.log = util.log.GetLogger(self.__class__.__name__)

    self.callback_succ = callback_succ
    self.callback_fail = callback_fail
    self.callback_exit = callback_exit
    self.callback_exception = callback_exception
    self.cmd_stack = []
    self.cmd_stack.append(AdbCommandBase.adb)
  
  def Parser(self):
    return (False, None)

  def CallbackSucc(self, content):
    if self.callback_succ is not None:
      try:
        self.callback_succ(content)
      except Exception as e:
        pass

  def CallbackFail(self, content):
    if self.callback_fail is not None:
      try:
        self.callback_fail(content)
      except Exception as e:
        pass
      
  
  def CallbackExit(self, code):
    if self.callback_exit is not None:
      try:
        self.callback_exit(code)
      except Exception as e:
        pass
  
  def CallbackException(self, e):
    if self.callback_exception is not None:
      try:
        self.callback_exception(e)
      except Exception as e:
        pass
  
  def _BuildCmd(self):
    out = ''
    for one in self.cmd_stack:
      out += one + ' '
    
    return out
  
  def Execute(self):
    try:
      cmd = self._BuildCmd()
      self.log.info('AdbCommandBase cmd: ' + cmd)
      p = subprocess.Popen(cmd, shell=False, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
      while p.poll() is None:
        line = p.stdout.readline()
        line = line.strip()
        succ, content = self.Parser(line)
        if succ:
          self.CallbackSucc(content)
        else:
          self.CallbackFail(content)
          p.kill()
          break
    
      
      
      line = p.stdout.readline()
      while len(line) > 0:
        line = line.strip()
        succ, content = self.Parser(line)
        if succ:
          self.CallbackSucc(content)
        else:
          self.CallbackFail(content)
          break
        line = p.stdout.readline()


      self.CallbackExit(p.returncode)
  
    except Exception as e:
      self.CallbackException(e)
      pass
  
  

class AdbListDevices(AdbCommandBase):
  cmd = 'devices -l'
  
  def __init__(self, callback_exit):
    super(AdbListDevices, self).__init__(None, None, self._callback_wrapper)
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
    self.cmd_stack.append(AdbListDevices.cmd)
    return super(AdbListDevices, self)._BuildCmd()
  
  
  
  def Parser(self, line):
    '''
    List of devices attached
    
    
    List of devices attached
    aa1ee7d1               device product:LeMax2_CN model:Le_X820 device:le_x2
    '''
    print(line)

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
  
  

class AdbInstallApk(AdbCommandBase):
  
  def __init__(self, serial_no, package_name, apk, callback_succ, callback_fail):
    super(AdbInstallApk, self).__init__(callback_succ, callback_fail)
    self.log = util.log.GetLogger(self.__class__.__name__)
    self.serial_no = serial_no
    self.apk = apk
    self.package_name = package_name

    #接收到某条信息后，就认为后续信息无意义，不再解析避免干扰
    self.stop_parser = False
  
    
    
  def _BuildCmd(self):
    self.cmd_stack.append(self._BuildSerialNo())
    self.cmd_stack.append(self._BuildInstall())
    return super(AdbInstallApk, self)._BuildCmd()
  
  def _BuildSerialNo(self):
    if self.serial_no is not None:
      return ' -s {} '.format(self.serial_no)
    else:
      return ''
    
  
  def _BuildInstall(self):
    return ' install -r {}'.format(self.apk)
    
  
  
  #这块后面可能需要根据不同手机设置parser
  def Parser(self, line):
    '''
    letv##################################################
    [  0%] /data/local/tmp/com.tencent.android.qqdownloader.apk
    adb: error: failed to get feature set: no devices/emulators found
    C:\workspace\code\chromium24\src\build\Debug\ctp_data\apk\com.tencent.android.qqdownloader.apk: 1 file pushed. 2.3 MB/s (9099747 bytes in 3.744s)]
    Please select on your phone whether can install the app by The ADB command?
    pkg: /data/local/tmp/com.tencent.android.qqdownloader.apk
    Success
    #xiaomi###############################################
    Failure [INSTALL_FAILED_USER_RESTRICTED: Install canceled by user]
  1115 KB/s (9099747 bytes in 7.965s)
  Please select on your phone whether can install the app by The ADB command?
  Failure [INSTALL_FAILED_USER_RESTRICTED: Install canceled by user]
  3406 KB/s (14642800 bytes in 4.197s)
  Please select on your phone whether can install the app by The ADB command?
  Failure [INSTALL_FAILED_VERSION_DOWNGRADE]
  6531 KB/s (21376547 bytes in 3.196s)
  Please select on your phone whether can install the app by The ADB command?
  
  拔线：Failed to install
    '''

    if self.stop_parser == True:
      return (True, 'Success')

    try:
      print(line)
      self.log.info(line)
      if 'error' in line:
        error = self.adb_error_re.search(line).group(1)
        self.log.info(error)
        return (False, error)
      # if line.startswith('adb: error: '):
      #   return (False, line)
      elif 'Failure' in line:
        error = self.adb_install_failure.search(line).group(1)
        self.log.info(error)
        return (False, error)
      elif line.startswith('pkg:') and self.package_name in line:
        return (True, 'push over: ')
      elif 'Failed' in line:
        return (False, line)
      else:
        #'[  0%] /data/local/tmp/com.tencent.android.qqdownloader.apk'
        left = line.find('[')
        right = line.find(']')
        if left != -1 and right != -1:
          return (True, line[left+1: right])
        elif line == 'Success':
          self.stop_parser = True
          return (True, 'Success')
        else:
          return (True, line)
    except Exception as e:
      return (False, e)
      
      
      
#======================================================
if __name__ == '__main__':
  re = AdbCommandBase.adb_install_failure.search('Failure [INSTALL_FAILED_VERSION_DOWNGRADE]').group(1)
  one = AdbListDevices(None)
  one.Execute()
  apk = r'C:\workspace\code\chromium24\src\build\Debug\ctp_data\apk\com.tencent.android.qqdownloader.apk'
  two = AdbInstallApk('aa1ee7d1', apk, None, None)
  two.Execute()