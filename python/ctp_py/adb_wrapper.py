import os.path as op
import os


from adb import adb_commands
from adb import sign_m2crypto

# KitKat+ devices require authentication
# https://blog.csdn.net/caibaihui/article/details/46862591
# signer = sign_m2crypto.M2CryptoSigner(
#   op.expanduser(r'C:\Users\liuqingping\.android\adbkey'))
# # op.expanduser('~/.android/adbkey'))
# # Connect to the device
# device = adb_commands.AdbCommands()
# device.ConnectDevice(
#   rsa_keys=[signer])
# # Now we can use Shell, Pull, Push, etc!
# for i in xrange(10):
#   print device.Shell('echo %d' % i)
#
# print device.Shell('pm list packages -f')
#
#
# def callback(one, two, three):
#   print(one)
#   print(two)
#   print(three)
#
#
# print(device.Install(r'C:\workspace\code\chromium24\src\build\Debug\ctp_data\apk\com.tencent.android.qqdownloader.apk',
#                      transfer_progress_callback=callback))




class ADBWrapper():
  
  def __init__(self):
    self.device = None
    self.adbkey = os.environ['USERPROFILE'] + r'\.android\adbkey'
    self.Init()
    
  
  def Init(self):
    try:
      self.device = adb_commands.AdbCommands()
      self.signer = None
      if os.path.exists(self.adbkey):
        self.signer = sign_m2crypto.M2CryptoSigner(
          op.expanduser(self.adbkey))
          # op.expanduser(r'C:\Users\liuqingping\.android\adbkey'))
      
    except Exception as e:
      self.device = None
      pass
    
    
   
  def ConnectDevice(self):
    try:
      self.device.ConnectDevice(
        rsa_keys=[self.signer])
    except Exception as e:
      self.device = None
      pass
    
  
  def Install(self, apk, callback):
    try:
      self.device.Install(r'C:\workspace\code\chromium24\src\build\Debug\ctp_data\apk\com.tencent.android.qqdownloader.apk', transfer_progress_callback=callback)
    except Exception as e:
      pass
    
  def ListDevices(self):
    out = []
    try:
      device_list= adb_commands.AdbCommands().Devices()
    
      for one in device_list:
        out.append(one.serial_number)
      else:
        return (True, out)
    except Exception as e:
      return (False, e)