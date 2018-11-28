# encoding: utf-8

import struct
import binascii
import ctypes
import sys
import traceback
#===========================
from google.protobuf import descriptor_pool
from google.protobuf import descriptor_pb2
from google.protobuf import reflection
from google.protobuf import descriptor_database
from google.protobuf import descriptor

#import socket
#from cStringIO import StringIO
#===========================
import pb.cc_comunication_pb2
import pb.cp_comunication_pb2
import pb.mq_protomsg_pb2
import pb.guard_comunication_pb2
import pb.apk_protomsg_pb2
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
        pb.cc_comunication_pb2.DESCRIPTOR.serialized_pb)
    self.pool.Add(id)
    #======================
    id = descriptor_pb2.FileDescriptorProto.FromString(
        pb.cp_comunication_pb2.DESCRIPTOR.serialized_pb)
    self.pool.Add(id)
    #======================
    id = descriptor_pb2.FileDescriptorProto.FromString(
        pb.mq_protomsg_pb2.DESCRIPTOR.serialized_pb)
    self.pool.Add(id)
    #======================
    id = descriptor_pb2.FileDescriptorProto.FromString(
        pb.guard_comunication_pb2.DESCRIPTOR.serialized_pb)
    self.pool.Add(id)
    # ======================
    id = descriptor_pb2.FileDescriptorProto.FromString(
      pb.apk_protomsg_pb2.DESCRIPTOR.serialized_pb)
    self.pool.Add(id)
	#========================

  def OnMessage(self, data):
    self.data = self.data[:] + data
    #self.data.write(data)


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
    #print(name)
    return name

  def GetMessage(self, name, total_size):

    ds = self.pool.FindMessageTypeByName(name)
    if ds:
      return reflection.ParseMessage(ds, self.data[8 + len(name):total_size + 4])

  #20140222 TODO:memory leak by call"reflection.ParseMessage"
  #don't know why reduce call it times to avoid
  def GetMessage2(self, name, total_size):

    try:
      if name == 'ctp.mq.BasicMQ':
        msg = pb.mq_protomsg_pb2.BasicMQ()
        msg.ParseFromString(self.data[8 + len(name):total_size + 4])
        return msg
      elif name == 'ctp.mq.LevelMQ':
        msg = pb.mq_protomsg_pb2.LevelMQ()
        msg.ParseFromString(self.data[8 + len(name):total_size + 4])
        return msg
      elif name == 'ctp.cp.BacktestingResultSave2DBRequery':
        #嵌套自定义报错，原因不明
        msg = pb.cp_comunication_pb2.BacktestingResultSave2DBRequery()
        msg.ParseFromString(self.data[8 + len(name):total_size + 4])
        return msg
      else:
        ds = self.pool.FindMessageTypeByName(name)
        if ds:

          #这个地方就是有问题的。你通过name拿到的只是外面那个message的des。如果里面嵌套message，
          #确实不知道ds是什么啊。。。
          #或者说，最外层的des，有携带内层des的义务吗？？
          return reflection.ParseMessage(ds, self.data[8 + len(name):total_size + 4])
        else:
          print('FindMessageTypeByName no find the data : ', name)
        return None
    except Exception as inst:
      exstr = traceback.format_exc()
      print ('exception stack',exstr)
      print(inst)

  def Dispatch(self, msg):
    #print('dispatch msg', msg.DESCRIPTOR.full_name)
    #print(msg)
    if self.message_handler.has_key(msg.DESCRIPTOR.full_name):
      #print('handled================== ', msg.DESCRIPTOR.full_name)
      self.message_handler[msg.DESCRIPTOR.full_name].Handle(msg)
    else:
      index = msg.DESCRIPTOR.full_name.rfind('.')
      module_name = ''
      if index != -1:
        module_name = msg.DESCRIPTOR.full_name[:index]
        #print('module_name', module_name)

      if len(module_name) and self.module_handler.has_key(module_name):
        self.module_handler[module_name].Handle(msg)
      else:  
        print("err no handler.....")
    pass


  def PackMessage(self, msg):
    #print('pack message')
    total_size = 4
    name = msg.DESCRIPTOR.full_name
    total_size = total_size + len(name)
    data = msg.SerializeToString()
    total_size = total_size + len(data)
    format_string = '2I ' + str(len(name)) + 's ' + str(len(data)) + 's'
    #print(format_string)
    in_data = (total_size, len(name), name, data)
    codec = struct.Struct(format_string)
    out_data = codec.pack(*in_data)
    #print(out_data)
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
#end===========================================================

class PrintHandler:
  def __init__(self, name):
    self.name = name


  def Handle(self, msg):
    #print('PrintHandler', 'Handle')
    #print(msg)
    pass

  def Name(self):
    return self.name

#============================================================    
def Test():
  #t1 = pb.cp_comunication_pb2.test1()
  #t1.s = 'hello'

  command = pb.cp_comunication_pb2.Command()
  command.cmd = 'hello'
  command.params.append('world')
  command_out = command.SerializeToString()


  #
  t2 = pb.cp_comunication_pb2.test2()
  t2.s2 = 'world'
  t2.t2.s = 'hello'

  codec = ProtobufCodec()
  out = codec.PackMessage(t2)
  out2 = t2.SerializeToString()
  #=======================================
  #pool = descriptor_pool.DescriptorPool()
  #id = descriptor_pb2.FileDescriptorProto.FromString(
  #    pb.cp_comunication_pb2.DESCRIPTOR.serialized_pb)
  #pool.Add(id)

  id = descriptor_pb2.FileDescriptorProto.FromString(
      pb.cp_comunication_pb2.DESCRIPTOR.serialized_pb)
  

  db = descriptor_database.DescriptorDatabase()
  pool = descriptor_pool.DescriptorPool(db)
  db.Add(id)
  #db.Add(self.factory_test2_fd)
  #=================
  ##id = descriptor_pb2.FileDescriptorProto.FromString(
  #    pb.cc_comunication_pb2.DESCRIPTOR.serialized_pb)
  #pool.Add(id)
  #======================
  
  #======================
  #id = descriptor_pb2.FileDescriptorProto.FromString(
  #    pb.mq_protomsg_pb2.DESCRIPTOR.serialized_pb)
  #pool.Add(id)
  #======================
  #id = descriptor_pb2.FileDescriptorProto.FromString(
  #    pb.guard_comunication_pb2.DESCRIPTOR.serialized_pb)
  #pool.Add(id)


  #========================================================================
  d1 = pb.cp_comunication_pb2.test2.DESCRIPTOR
  #dp1 = DescriptorProto()
  #d1.CopyToProto(dp1)
  # dp1 serialized, sent over network, deserialized
  #d2 = descriptor.MakeDescriptor(dp1)
  ## Un-commenting this block fixes the problem
  #
  #for desc in dp1.nested_type:
  #    d2.nested_types.append( descriptor.MakeDescriptor(desc) )
  #d2.nested_types_by_name = dict((t.name, t) for t in d2.nested_types)
  #    
  #for f in dp1.field:
  #    d2.fields_by_number[f.number].message_type = d2.nested_types_by_name[f.type_name.split('.')[-1]]
  #
  # This cannot be duplicated across the net
  #d2.fields[0].message_type._concrete_class = d1.fields[0].message_type._concrete_class 
  #=======================================

  ds_command = pool.FindMessageTypeByName('ctp.cp.Command')
  print(str(reflection.ParseMessage(ds_command, command_out)))
  #========================================
  temp = pb.cp_comunication_pb2.test2()
  temp.ParseFromString(out2)
  print('temp :', str(temp))
  ds = pool.FindMessageTypeByName('ctp.cp.test2')
  #ds = pool.FindMessageTypeByName('pb.cp_comunication_pb2.ctp.cp.test2')
  print (isinstance(ds, descriptor.Descriptor))
  if ds:
    print('find ds', str(ds))
    print('find ds', str(d1))
    print(str(reflection.ParseMessage(d1, out2)))
    print(str(reflection.ParseMessage(ds, out2)))

#======================================================
if __name__ == '__main__':
  Test()