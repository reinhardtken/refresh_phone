#!/usr/bin/env python
# encoding: utf-8

import subprocess

class AdbInstall():
  
  def __init__(self):
    self.adb = r'C:\workspace\code\chromium24\src\build\Debug\adb\adb '
    
  
  def BuildSerialNo(self, serial_no):
    if serial_no is not None:
      return ' -s {} '.format(serial_no)
    else:
      return ''
    
  
  def BuildInstall(self, apk):
    return ' install -r {}'.format(apk)
    
    
  
  def Install(self, serial_no, apk, callback):
    try:
      cmd = self.adb + self.BuildSerialNo(serial_no) + self.BuildInstall(apk)
      p = subprocess.Popen(cmd, shell=False, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
      while p.poll() is None:
        line = p.stdout.readline()
        line = line.strip()
        succ, content = self.Parser(line)
        if succ:
          callback(succ, content)
        else:
          callback(succ, content)
          p.kill()
          break
          
      # if p.returncode == 0:
      #   print('Subprogram success')
      # else:
      #   print('Subprogram failed')
      
    except Exception as e:
      pass
  
  
  
  def Parser(self, line):
    '''
    [  0%] /data/local/tmp/com.tencent.android.qqdownloader.apk
    adb: error: failed to get feature set: no devices/emulators found
    C:\workspace\code\chromium24\src\build\Debug\ctp_data\apk\com.tencent.android.qqdownloader.apk: 1 file pushed. 2.3 MB/s (9099747 bytes in 3.744s)]
    Please select on your phone whether can install the app by The ADB command?
    pkg: /data/local/tmp/com.tencent.android.qqdownloader.apk
    Success
    :param line:
    :return:
    '''
    if line.startswith('adb: error: '):
      return (False, line)
    else:
      '[  0%] /data/local/tmp/com.tencent.android.qqdownloader.apk'
      left = line.find('[')
      right = line.find(']')
      if left != -1 and right != -1:
        return (True, line[left+1: right])
      elif line == 'Success':
        return (True, 'Success')
      else:
        pass