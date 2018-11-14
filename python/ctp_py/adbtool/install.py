#!/usr/bin/env python
# encoding: utf-8


import re
import traceback

import util
import base


class Command(base.AdbCommandBase):
  
  def __init__(self, serial_no, package_name, apk, callback_succ, callback_fail):
    super(Command, self).__init__(callback_succ, callback_fail)
    self.log = util.log.GetLogger(self.__class__.__name__)
    self.serial_no = serial_no
    self.apk = apk
    self.package_name = package_name

    # 接收到某条信息后，就认为后续信息无意义，不再解析避免干扰
    self.stop_parser = False
  
  def _BuildCmd(self):
    self.cmd_stack.append(self._BuildSerialNo())
    self.cmd_stack.append(self._BuildInstall())
    return super(Command, self)._BuildCmd()
  
  def _BuildSerialNo(self):
    if self.serial_no is not None:
      return ' -s {} '.format(self.serial_no)
    else:
      return ''
  
  def _BuildInstall(self):
    return ' install -r {}'.format(self.apk)
  
  # 这块后面可能需要根据不同手机设置parser
  # 可以通过装完后枚举package list的方式来判断是不是装成功了
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

  包无效：Invalid APK file: H:\refresh_phone_out_1.0.0.3\ctp_data\apk\com.youku.phone.apk
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
        # '[  0%] /data/local/tmp/com.tencent.android.qqdownloader.apk'
        left = line.find('[')
        right = line.find(']')
        if left != -1 and right != -1:
          return (True, line[left + 1: right])
        elif line == 'Success':
          self.stop_parser = True
          return (True, 'Success')
        else:
          return (True, line)
    except Exception as e:
      exstr = traceback.format_exc()
      print(exstr)
      self.log.info(exstr)
      return (False, e)