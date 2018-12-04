# encoding: utf-8


import subprocess
import adbtool.start_server
import cmdtool.netstat
import cmdtool.taskkill
import os
import psutil
import re
import util.utility


import requests
# import progressbar
import requests.packages.urllib3

requests.packages.urllib3.disable_warnings()



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


if __name__ == '__main__':
  md5 = util.utility.GetFileMD5(r'C:\workspace\code\chromium24\src\build\out\adb\adb.exe')
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
