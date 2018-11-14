# #!/usr/bin/env python
# # encoding: utf-8
#
# import subprocess
# import re
# import traceback
#
# import util
# import base
#
#
# class Command(base.CommandBase):
#   # 'netstat -aon|findstr "5040"'
#   # TCP    127.0.0.1:5037         0.0.0.0:0              LISTENING       31048
#   # TCP    127.0.0.1:5037         127.0.0.1:3759         TIME_WAIT       0
#   # TCP    127.0.0.1:5037         127.0.0.1:3760         TIME_WAIT       0
#   # TCP    127.0.0.1:5037         127.0.0.1:3762         TIME_WAIT       0
#   # TCP    127.0.0.1:5037         127.0.0.1:3763         TIME_WAIT       0
#   # TCP    127.0.0.1:5037         127.0.0.1:3765         TIME_WAIT       0
#   # TCP    127.0.0.1:5037         127.0.0.1:3766         TIME_WAIT       0
#
#   # tasklist | findstr "139056"
#   # TCP    127.0.0.1:5037         0.0.0.0:0              LISTENING       31048
#   cmd = 'netstat -aon | findstr "'
#   tail = '" | findstr "LISTENING"'
#
#   # 这个命令，shell必须是true
#   # shell=False，回返回错误码 1
#
#   def __init__(self, find_port, callback_exit):
#     super(Command, self).__init__(self._my_succ_callback, None, self._my_exit_callback, None)
#     self.log = util.log.GetLogger(self.__class__.__name__)
#     self.my_callback = callback_exit
#     self.pid = None
#     self.find_port = str(find_port)
#     self.success = False
#     self.shell = True
#
#   def _my_succ_callback(self, content):
#     if content == base.CommandBase.SUCCESS:
#       self.success = True
#
#   def _my_exit_callback(self, content):
#     if content == base.CommandBase.SUCCESS:
#       self.success = True
#
#     if self.my_callback is not None:
#       if self.success:
#         self.my_callback((True, self.pid))
#       else:
#         self.my_callback((False, None))
#
#   def _BuildCmd(self):
#     self.cmd_stack.append(Command.cmd + self.find_port + Command.tail)
#     return super(Command, self)._BuildCmd()
#
#   def Parser(self, line):
#     '''
#     * daemon not running. starting it now at tcp:5038 *
#     * daemon started successfully *
#     '''
#     try:
#       print(line)
#       self.log.info(line)
#       if line.startswith('TCP'):
#         lines = line.split(' ')
#         self.pid = int(lines[-1])
#         return (True, base.CommandBase.SUCCESS)
#
#     except Exception as e:
#       exstr = traceback.format_exc()
#       print(exstr)
#       self.log.info(exstr)
#       return (False, e)
#
#
# def test():
#   def callback(one):
#     print(one)
#
#   one = Command(5040, callback)
#   one.Execute()
#
#
# # ======================================================
# if __name__ == '__main__':
#   one = Command(5039)
#   one.Execute()