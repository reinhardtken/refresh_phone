import struct
import binascii
import ctypes
#===========================
from google.protobuf import descriptor_pool
from google.protobuf import descriptor_pb2
from google.protobuf import reflection

#import socket
#from cStringIO import StringIO
#===========================
import cc_comunication_pb2
#begin================================================
class ProtobufCodec:

  def __init__(self):
    self.data = ''
    self.message_handler = {}
    self.module_handler = {}
    self.InitProtobufDescriptors()


  def InitProtobufDescriptors(self):
    self.pool = descriptor_pool.DescriptorPool()
    #=================
    id = descriptor_pb2.FileDescriptorProto.FromString(
        cc_comunication_pb2.DESCRIPTOR.serialized_pb)
    self.pool.Add(id)
    #======================

  def OnMessage(self, data):
    self.data = self.data[:] + data
    #self.data.write(data)

    print(self.data)
    while(True):
      buffer_size = len(self.data)
      if buffer_size > 4:
        total_size = self.GetTotalSize()
        if buffer_size - 4 >= total_size:
          name = self.GetName()
          msg = self.GetMessage(name, total_size)
          self.Dispatch(msg)
          self.data = self.data[total_size + 4:]
        else:
          return
      else:
        return


  def GetTotalSize(self):
    print('unpack headsize')
    unpack_head = struct.Struct('I')
    size = unpack_head.unpack(self.data[:4])[0]
    print(size)
    return size

  def GetName(self):
    print('unpack name')
    unpack_name = struct.Struct('I')
    unpack_name_len = unpack_name.unpack(self.data[4:8])[0]
    format_string = str(unpack_name_len) + 's'
    unpack_name_string = struct.Struct(format_string)
    name = unpack_name_string.unpack(self.data[8:8 + unpack_name_len])[0]
    print(name)
    return name

  def GetMessage(self, name, total_size):
    print('unpack msg')
    ds = self.pool.FindMessageTypeByName(name)
    if ds:
      return reflection.ParseMessage(ds, self.data[8 + len(name):total_size + 4])

  def Dispatch(self, msg):
    print('dispatch msg')
    #print(msg)
    if self.message_handler.has_key(msg.DESCRIPTOR.full_name):
      self.message_handler[msg.DESCRIPTOR.full_name].Handle(msg)
    else:
      index = msg.DESCRIPTOR.full_name.rfind('.')
      module_name = ''
      if index != -1:
        module_name = msg.DESCRIPTOR.full_name[:index]

      if len(module_name) and self.module_handler.has_key(module_name):
        self.module_handler[module_name].Handle(msg)
      else:  
        print("err no handler.....")
    pass


  def PackMessage(self, msg):
    print('pack message')
    total_size = 4
    name = msg.DESCRIPTOR.full_name
    total_size = total_size + len(name)
    data = msg.SerializeToString()
    total_size = total_size + len(data)
    format_string = '2I ' + str(len(name)) + 's ' + str(len(data)) + 's'
    print(format_string)
    in_data = (total_size, len(name), name, data)
    codec = struct.Struct(format_string)
    out_data = codec.pack(*in_data)
    print(out_data)
    return out_data

  def RegisterMessageHandler(self, handler):
    self.message_handler[handler.Name()] = handler

  def RegisterModuleHandler(self, handler):
    self.module_handler[handler.Name()] = handler
#end===========================================================

class PrintHandler:
  def __init__(self, name):
    self.name = name


  def Handle(self, msg):
    print(msg)

  def Name(self):
    return self.name

#============================================================    