# encoding: utf-8

import struct
import sys
import traceback
#py3
from io import BytesIO
#py2
from cStringIO import StringIO
#########################################################
from google.protobuf import descriptor_pool
from google.protobuf import descriptor_pb2
from google.protobuf import reflection
#########################################################
import pb.apk_protomsg_pb2

#py3
_b = (lambda x:x.encode('latin1'))
_s = (lambda x:x.decode('latin1'))
#py2
_b = (lambda x:x)
_s = (lambda x:x)
#begin================================================
class ProtobufCodec:
  STAGE_SHAKEHAND = 0
  STAGE_WORKING = 1
  def __init__(self, server=False, token=None):
    #当调用回调的时候，会把token带上，让logic知道数据来自哪个socket
    self.token = token
    #py3
    self.data = bytes()
    #py2
    self.data = ''
    self.message_handler = {}
    self.module_handler = {}
    self.InitProtobufDescriptors()
    
    #因为py不支持解析嵌套的pb消息，目前又没有找到好的方案
    #先简单粗暴的解决
    self.parser_handler = {}
    
    if server:
      self.stage = ProtobufCodec.STAGE_SHAKEHAND
    else:
      self.stage = ProtobufCodec.STAGE_WORKING


  def InitProtobufDescriptors(self):
    self.pool = descriptor_pool.DescriptorPool()
    # ======================
    id = descriptor_pb2.FileDescriptorProto.FromString(
      pb.apk_protomsg_pb2.DESCRIPTOR.serialized_pb)
    self.pool.Add(id)


  
  
  def ProcessShakehand(self):
    buffer_size = len(self.data)
    if buffer_size > 4:
      unpack_head = struct.Struct('I')
      size = unpack_head.unpack(self.data[:4])[0]
      if buffer_size - 4 >= size:
        format_string = str(size) + 's'
        unpack_name_string = struct.Struct(format_string)
        name = unpack_name_string.unpack(self.data[4:4+size])[0]
        self.data = self.data[size + 4:]
        self.stage = ProtobufCodec.STAGE_WORKING
        print(name)
        return True
    
    return False
  

  def OnMessage(self, data):
    self.data = self.data[:] + data

    
    #如果是握手阶段，把握手信息提取出来，切换状态
    if self.stage == ProtobufCodec.STAGE_SHAKEHAND:
      #握手没有完成
      if not self.ProcessShakehand():
        return

    while(True):
      buffer_size = len(self.data)
      if buffer_size > 4:
        total_size = self.GetTotalSize()
        if buffer_size - 4 >= total_size:
          name = self.GetName()
          msg = self.GetMessage2(name, total_size)
          if msg != None:
            self.Dispatch(msg)
            self.data = self.data[total_size + 4:]
          else:
            sys.exit(0)
        else:
          return
      else:
        return


  def GetTotalSize(self):
    unpack_head = struct.Struct('I')
    size = unpack_head.unpack(self.data[:4])[0]
    return size

  def GetName(self):
    unpack_name = struct.Struct('I')
    unpack_name_len = unpack_name.unpack(self.data[4:8])[0]
    format_string = str(unpack_name_len) + 's'
    unpack_name_string = struct.Struct(format_string)
    name = unpack_name_string.unpack(self.data[8:8 + unpack_name_len])[0]
    return name


  def GetMessage(self, name, total_size):
    ds = self.pool.FindMessageTypeByName(name)
    if ds:
      return reflection.ParseMessage(ds, self.data[8 + len(name):total_size + 4])

  #20140222 TODO:memory leak by call"reflection.ParseMessage"
  #don't know why reduce call it times to avoid
  def GetMessage2(self, name, total_size):

    try:
      if _s(name) in self.parser_handler:
        msg = self.parser_handler[_s(name)](name, self.data[8 + len(name):total_size + 4])
        if msg is not None:
          return msg
        
      else:
        ds = self.pool.FindMessageTypeByName(_s(name))
        if ds:
          #这个地方就是有问题的。你通过name拿到的只是外面那个message的des。如果里面嵌套message，
          #确实不知道ds是什么啊。。。
          #或者说，最外层的des，有携带内层des的义务吗？？
          #2018-11-15
          #按如下帖子的说法，这个问题至少在13（15）年的版本无解？？
          #帖子明确说了嵌套message在c++下work，但是python不行
          #https://stackoverflow.com/questions/16607074/accessing-field-of-protobuf-message-of-unknown-type-in-python
          return reflection.ParseMessage(ds, self.data[8 + len(name):total_size + 4])
        else:
          print('FindMessageTypeByName no find the data : ', name)
        return None
    except Exception as inst:
      exstr = traceback.format_exc()
      print ('exception stack',exstr)
      print(inst)

  def Dispatch(self, msg):
    if msg.DESCRIPTOR.full_name in self.message_handler:
      self.message_handler[msg.DESCRIPTOR.full_name].Handle((self.token, msg, ))
    else:
      index = msg.DESCRIPTOR.full_name.rfind('.')
      module_name = ''
      if index != -1:
        module_name = msg.DESCRIPTOR.full_name[:index]
        #print('module_name', module_name)

      if len(module_name) and module_name in self.module_handler:
        self.module_handler[module_name].Handle((self.token, msg, ))
      else:  
        print("err no handler.....")
    pass


  def PackMessage(self, msg):
    total_size = 4
    name = msg.DESCRIPTOR.full_name
    total_size = total_size + len(name)
    data = msg.SerializeToString()
    total_size = total_size + len(data)
    format_string = '2I ' + str(len(name)) + 's ' + str(len(data)) + 's'

    in_data = (total_size, len(name), _b(name), data)
    codec = struct.Struct(format_string)
    out_data = codec.pack(*in_data)

    return out_data

  def RegisterMessageHandler(self, handler):
    print('RegisterMessageHandler', handler.Name())
    self.message_handler[handler.Name()] = handler

  def RegisterMessageHandlerWithName(self, handler, name):
    print('RegisterMessageHandlerWithName', name)
    self.message_handler[name] = handler

  def RegisterModuleHandler(self, handler):
    print('RegisterModuleHandler', handler.Name())
    self.module_handler[handler.Name()] = handler
    
  
  def RegisterParserHandler(self, name, handler):
    print('RegisterParserHandler', name)
    self.parser_handler[name] = handler
#end===========================================================

class PrintHandler:
  def __init__(self, name):
    self.name = name


  def Handle(self, msg):
    pass

  def Name(self):
    return self.name



#======================================================
if __name__ == '__main__':
  pass