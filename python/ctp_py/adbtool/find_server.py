#!/usr/bin/env python
# encoding: utf-8


import re
import traceback

# import util
# import base
import psutil


#用于测试，自测
def _debug_jump(one):
  # for one_cmd in one['cmd']:
  #   if one_cmd.isdigit() and one_cmd == '5038':
  #     return True

  return False



def KillAllServer(out):
  #检查找到的进程，能杀的全杀了
  try:
    for one in out:
      if _debug_jump(one) == False:
        try:
          p = psutil.Process(one['pid'])
          p.kill()
        except Exception as e:
          pass
  except Exception as e:
    pass


def FindAllServer():
  out = []
  for pnum in psutil.pids():
    try:
      p = psutil.Process(pnum)
      cmd = p.cmdline()
      param = set(cmd)
      if 'fork-server' in param and '-P' in param:
        one_result = {}
        one_result['pid'] = pnum
        one_result['cmd'] = cmd
        one_result['cwd'] = p.cwd()
        one_result['status'] = p.status()
        one_result['create_time'] = p.create_time()
        out.append(one_result)

    except Exception as e:
      print(e)

  for one in out:
    print(one)

  return out


def FindPort(live):
  out = []
  for one in live:
    for one_cmd in one['cmd']:
      if one_cmd.isdigit():
        out.append(one_cmd)
        break

  return out

#======================================================
if __name__ == '__main__':
  out = FindAllServer()
  KillAllServer(out)
  out = FindAllServer()
  print(out)