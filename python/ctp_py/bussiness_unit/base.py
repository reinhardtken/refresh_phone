#!/usr/bin/env python
# encoding: utf-8


import util
#=====================================
import util.log
import util.thread_class2


class Base(object):
  
  def __init__(self, queue_out):
    self.thread = util.thread_class2.ThreadClass(self.ProcessMessage)
    self.queue_in = self.thread.queue()
    self.queue_out = queue_out
    
    #二选一
    self.socket = None
    self.socket_multiple = set()
    
    
    self.handler_list = []
    self.parser_list = []

  # #注册自己感兴趣的消息
  # def RegisterHandler(self):
  #   #handler_list.append('apk.Command')
  #   pass


  def Start(self):
    try:
      self.thread.Start()
    except Exception as e:
      print(e)
  
  
  #数据发送到network的queue
  def SendMessage(self, data):
    if self.socket is not None:
      self.queue_out.put((self.socket, data, ))
    else:
      #广播
      for one in self.socket_multiple:
        self.queue_out.put((one, data,))
    
    

  #建立socket和logic的关联，此关联为一个socket对应一个logic
  def Connect(self, socket):
    for one in self.handler_list:
      socket.RegisterMessageHandlerWithName(self, one)
    for one in self.parser_list:
      socket.RegisterParserHandler(self, one)
    self.socket = socket
    
  
  #多个socket对应一个logic
  def ConnectMultiple(self, socket):
    for one in self.handler_list:
      socket.RegisterMessageHandlerWithName(self, one)
    for one in self.parser_list:
      socket.RegisterParserHandler(one, self.ParserMessage)
    self.socket_multiple.add(socket)
 
  
  #数据发送到queue_in，在network线程被调用
  def Handle(self, msg):
    self.queue_in.put(msg)


  def ParserMessage(self, name, data):
    pass


  #在自己线程处理相关消息
  def ProcessMessage(self, msg):
    #msg[0] socket, msg[1] real data
    pass
    # if msg.DESCRIPTOR.full_name == 'apk.Command':
    #   if msg.cmd == 'py_config':
    #     # 完成初始化
    #     if self.prop is None:
    #       self.InitGlobalLogByParam(config.CreateProp(msg))
    #       self.Init2(config.CreateProp(msg))
    #   elif self.prop is not None:
    #     # 只有完成了初始化才能响应业务请求
    #     if msg.cmd == 'check_net_package_list':
    #       self.ProcessCheckUpdatePackageList(msg)
    #     elif msg.cmd == 'remove_local_package_list':
    #       self.ProcessRemoveLocalPackageList(msg)
    #     elif msg.cmd == 'get_local_package_list':
    #       self.ProcessGetLocalPackageList(msg)
    #     elif msg.cmd == 'pyadb_install_apk':
    #       self.ProcessInstallApk2(msg)
    #     elif msg.cmd == 'pyadb_scan_devices':
    #       self.ProcessScanDevices2(msg)
