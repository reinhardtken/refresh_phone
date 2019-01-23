#!/usr/bin/env python
# encoding: utf-8

import subprocess
import re
import traceback

import util
import base


class Command(base.AdbCommandBase):
  
  def __init__(self, serial_no, package_name, apk, callback_succ, callback_fail):
    super(Command, self).__init__(callback_succ, callback_fail, self.CallbackExit)
    self.log = util.log.GetLogger(self.__class__.__name__)
    self.serial_no = serial_no
    self.apk = apk
    self.package_name = package_name
    
    self.callback_succ = callback_succ
    self.callback_fail = callback_fail

    # 接收到某条信息后，就认为后续信息无意义，不再解析避免干扰
    self.stop_parser = False
    #1.0.39版本的adb，再小米等多个手机上，没有任何返回结果
    #简单的设计为如果没有明确返回错误，同时退出值为0，则认为成功
    self.happend_error = False
  
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
  
  
  def CallbackExit(self, exitcode):
    #正常不需要这个函数回报数据
    if self.stop_parser is False and self.happend_error is False:
      try:
        if exitcode == 0 and self.callback_succ is not None:
           self.callback_succ('Success')
        elif exitcode != 0 and self.callback_fail is not None:
          self.callback_fail('错误码 ： ' + str(exitcode))
      except Exception as e:
        pass
      pass
  
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
  
  签名无法通过
  Failed to install C:\workspace\code\chromium24\src\build\Release\ctp_data\apk\com.taobao.trip.apk:
  Failure [INSTALL_PARSE_FAILED_NO_CERTIFICATES: Failed to collect certificates from /data/app/vmdl617260616.tmp/base.apk using APK Signature Scheme v2: SHA-256 digest of contents did not verify]
  
  红米1s
  Permission denied
    '''
    
    if self.stop_parser == True:
      #某些手机出现了一个success，后面又跟着failue的情况，猜测第一个是推包成功，第二个是装包失败
      if 'Failure' in line:
        error = self.adb_install_failure.search(line).group(1)
        self.log.warning('Failure after success!!!!')
        self.log.warning(error)
        return (False, error)
      else:
        return (True, 'Success')
    
    
    try:
      print(line)
      self.log.info(line)
      if 'error' in line:
        error = self.adb_error_re.search(line).group(1)
        self.log.warning(error)
        self.happend_error = True
        return (False, error)
      elif 'Failure' in line:
        error = self.adb_install_failure.search(line).group(1)
        self.log.warning(error)
        self.happend_error = True
        return (False, error)
      elif line.startswith('pkg:') and self.package_name in line:
        return (True, 'push over: ')
      elif 'Failed' in line:
        self.log.warning(line)
        self.happend_error = True
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
      self.happend_error = True
      return (False, e)

  # def Execute(self):
  #   try:
  #     #cmd = r'C:\workspace\code\chromium24\src\build\out\adb_1.0.40\adb -s  aa1ee7d1 install -r   C:\workspace\code\chromium24\src\build\Release\ctp_data\apk\com.anroid.mylockscreen.apk'
  #     #cmd = cmd.decode('utf-8')
  #     cmd = self._BuildCmd()
  #     env = self._BuildEnv()
  #     self.log.info('CommandBase cmd: ' + cmd)
  #     p = subprocess.Popen(cmd, env=env, shell=self.shell, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
  #     while p.poll() is None:
  #       p.stdout.flush()
  #       line = p.stdout.readline()
  #       line = line.strip()
  #       if not len(line):
  #         p.stderr.flush()
  #         line = p.stderr.readline()
  #         line = line.strip()
  #
  #       succ, content = self.Parser(line)
  #       if succ:
  #         self.CallbackSucc(content)
  #       else:
  #         self.CallbackFail(content)
  #         # p.kill()
  #         break
  #
  #     line = p.stdout.readline()
  #     while len(line) > 0:
  #       line = line.strip()
  #       succ, content = self.Parser(line)
  #       if succ:
  #         self.CallbackSucc(content)
  #       else:
  #         self.CallbackFail(content)
  #         break
  #       line = p.stdout.readline()
  #
  #     self.returncode = p.returncode
  #     self.CallbackExit(p.returncode)
  #
  #   except Exception as e:
  #     exstr = traceback.format_exc()
  #     print(exstr)
  #     self.log.info(exstr)
  #     self.CallbackException(e)
  #     pass