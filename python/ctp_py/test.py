# encoding: utf-8


import subprocess
import adbtool.start_server
import cmdtool.netstat
import cmdtool.taskkill
import cmdtool.base
import adbtool.uninstall
import adbtool.install
import adbtool.base
import adbtool.start_server
import os
import psutil
import re
import util.utility
import adbtool.find_server

import requests
# import progressbar
import requests.packages.urllib3

requests.packages.urllib3.disable_warnings()

import defer



def test_psutil():
  out = []
  print u"系统全部进程 %s" % psutil.pids()
  for pnum in psutil.pids():
    p = psutil.Process(pnum)
    # print u"进程名 %-20s  内存利用率 %-18s 进程状态 %-10s 创建时间 %-10s " % (p.name(), p.memory_percent(), p.status(), p.create_time())
    # print p.io_counters()    #进程读写信息
    try:
      print('==================')
      cmd = p.cmdline()
      for one in cmd:
        if 'fork-server' in one:
          one_result = {}
          one_result['cmd'] = cmd
          one_result['cwd'] = p.cwd()
          one_result['status'] = p.status()
          one_result['create_time'] = p.create_time()
          out.append(one_result)
          break

    except Exception as e:
      print(e)

  for one in out:
    print(one)


def test_down():
  url = "http://s0qrdt.kdndj.com/files/7c8cf81d5b7edc5c3daa33b4d97bc400.apk"
  
  response = requests.request("GET", url, stream=True, data=None, headers=None)
  
  save_path = r'C:\workspace\tmp\123.apk'
  
  total_length = int(response.headers.get("Content-Length"))
  now = 0
  with open(save_path, 'wb') as f:
    # widgets = ['Progress: ', progressbar.Percentage(), ' ',
    #            progressbar.Bar(marker='#', left='[', right=']'),
    #            ' ', progressbar.ETA(), ' ', progressbar.FileTransferSpeed()]
    # pbar = progressbar.ProgressBar(widgets=widgets, maxval=total_length).start()
    for chunk in response.iter_content(chunk_size=8096):
      if chunk:
        f.write(chunk)
        f.flush()
        now += len(chunk)
      print(float(now)/total_length)
      
      
      
      
def b_callback(arg):
  print(  "b_callback called with arg =", arg)
  return b


def on_done(arg):
  print( "on_done called with arg =", arg)
  return arg



a = defer.Deferred()
b = defer.Deferred()


def testa():
  print(dir(a))
  a.add_callback(b_callback)
  a.add_callback(on_done)
  a.callback(3)
  b.callback(4)

def testb():
  a.add_callback(b_callback)
  a.add_callback(on_done)
  b.callback(4)
  a.callback(3)


def test_defer():
  pass



if __name__ == '__main__':
  testa()
  live = adbtool.find_server.FindAllServer()
  adbtool.find_server.KillAllServer(live)
  live = adbtool.find_server.FindAllServer()
  adbtool.base.AdbCommandBase.adb = r'C:\workspace\code\chromium24\src\build\out\adb_1.0.39\adb'

  port = adbtool.start_server.Command.GenPort()
  # 产生新server
  start_server = adbtool.start_server.Command(port)
  start_server.Execute()
  
  def CallbackSucc(progress):
    pass


  def CallbackFail(progress):
    pass

  # cmdtool.base.CommandBase.global_env['ANDROID_ADB_SERVER_PORT'] = '5038'
  device = 'b6341ede'.decode('utf-8')
  # device = 'aa1ee7d1'
  uninstall = adbtool.install.Command(device, 'com.anroid.mylockscreen', r'C:\workspace\code\chromium24\src\build\Release\ctp_data\apk\com.taobao.trip.apk', CallbackSucc,
                                        CallbackFail)


  uninstall.Execute()
  
  #Android Debug Bridge version 1.0.32
  #'d06664ec2c78e6532f53e654b424cab4'
  
  #以后版本不支持xp
  #Android Debug Bridge version 1.0.36
  #'03596ed3833e08880030e1e1a0136e88'
  #Android Debug Bridge version 1.0.39
  # 'dec7e5e645aded51f9d7f02d60e9ed41'
  #Android Debug Bridge version 1.0.40
  #'c94a562536ca23a1936d4892b0d9d735'
  path = r'C:\prog\adb\adb 1.0.36 and fastboot\adb.exe'
  # path = r'C:\Users\liuqingping\AppData\Local\Android\Sdk\platform-tools\adb.exe'
  #path = r'C:\workspace\code\chromium24\src\build\release\adb\adb.exe'
  md5 = util.utility.GetFileMD5(path)
  test_psutil()
  line = '* daemon not running. starting it now at tcp:5038 *'
  re_port = re.compile(r'^[\*\.: A-Za-z]*([0-9]*) \*$')
  result = re_port.search(line).group(1)
  line = '* daemon not running. starting it now on port 5038 *'
  result = re_port.search(line).group(1)
  test_down()
  a = r'C:\Users\liuqingping\Desktop\7c8cf81d5b7edc5c3daa33b4d97bc400.apk'
  out = util.utility.GetFileMD5(a)
  # 获取一个port
  port = adbtool.start_server.Command.GenPort()
  
  # 总是尝试先看看找个port有没有人用
  netstat = cmdtool.netstat.Command(port)
  netstat.Execute()
  # 有人用kill掉
  succ, pid = netstat.GetReturnCode()
  if succ:
    kill = cmdtool.taskkill.Command(pid)
    kill.Execute()
  
    # 产生新server
    start_server = adbtool.start_server.Command(port)
    start_server.Execute()
    succ, pid = start_server.GetReturnCode()
   
   
   
   # # adbtool.start_server.test()
   # cmdtool.netstat.test()
   # out = os.system('netstat -aon|findstr "5040"')  # 25端口号
   # print(out)  # 输出进程
   # out = os.system('tasklist|findstr "139056"')  # 3316进是程
   # print(out)
   #
   # p = psutil.Process(139056)
   # print('Process name : %s' % p.name())
   #
   # # 获取进程bin路径
   # print('Process bin  path : %s' % p.exe())
   #
   # # 获取pid对应的路径
   # print('Process path : %s' % p.cwd())
   
   # adbtool.start_server.test()
     # shell_cmd = 'python3 subprogram.py'
     # cmd = r'C:\workspace\code\chromium24\src\build\Debug\adb\adb install -r C:\workspace\code\chromium24\src\build\Debug\ctp_data\apk\com.tencent.android.qqdownloader.apk'
     # p = subprocess.Popen(cmd, shell=False, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
     # while p.poll() is None:
     #     line = p.stdout.readline()
     #     line = line.strip()
     #
     #     if line:
     #         print('Subprogram output: [{}]'.format(line))
     # if p.returncode == 0:
     #     print('Subprogram success')
     # else:
     #     print('Subprogram failed')
