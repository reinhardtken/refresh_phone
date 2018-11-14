#!/usr/bin/env python
# encoding: utf-8

import subprocess
import re
import traceback

import util
import base


class Command(base.CommandBase):
  # taskkill /t /f /pid 139056
  # 成功: 已终止 PID 139056 (属于 PID 143336 子进程)的进程。
  #

  # netstat -ano | findstr "5037" | findstr "LISTENING"
  # TCP    127.0.0.1:5037         0.0.0.0:0              LISTENING       31048
  cmd = 'taskkill /t /f /pid  '
  
  # 这个命令，shell必须是true
  # shell=False，回返回错误码 1
  
  def __init__(self, pid):
    super(Command, self).__init__()
    self.log = util.log.GetLogger(self.__class__.__name__)
    self.pid = str(pid)
    self.shell = True
  

  
  def _BuildCmd(self):
    self.cmd_stack.append(Command.cmd + self.pid)
    return super(Command, self)._BuildCmd()
  


def test():
  def callback(one):
    print(one)
  
  one = Command(5040, callback)
  one.Execute()


# ======================================================
if __name__ == '__main__':
  one = Command(5039)
  one.Execute()