# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: apk_protomsg.proto

from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from google.protobuf import reflection as _reflection
from google.protobuf import descriptor_pb2
# @@protoc_insertion_point(imports)




DESCRIPTOR = _descriptor.FileDescriptor(
  name='apk_protomsg.proto',
  package='apk',
  serialized_pb='\n\x12\x61pk_protomsg.proto\x12\x03\x61pk\"H\n\x07\x43ommand\x12\x0b\n\x03\x63md\x18\x01 \x02(\t\x12\x0e\n\x06\x63md_no\x18\x02 \x02(\x05\x12\x11\n\ttimestamp\x18\x03 \x02(\x03\x12\r\n\x05param\x18\x04 \x03(\t\"]\n\x0f\x43ommandProgress\x12\x0b\n\x03\x63md\x18\x01 \x02(\t\x12\x0e\n\x06\x63md_no\x18\x02 \x02(\x05\x12\x0c\n\x04\x63ode\x18\x03 \x02(\x05\x12\x11\n\ttime_cost\x18\x04 \x01(\x03\x12\x0c\n\x04info\x18\x05 \x03(\t\"]\n\x0f\x43ommandResponse\x12\x0b\n\x03\x63md\x18\x01 \x02(\t\x12\x0e\n\x06\x63md_no\x18\x02 \x02(\x05\x12\x0c\n\x04\x63ode\x18\x03 \x02(\x05\x12\x11\n\ttime_cost\x18\x04 \x01(\x03\x12\x0c\n\x04info\x18\x05 \x03(\t\"\xb5\x01\n\x06OneApk\x12\n\n\x02id\x18\x01 \x01(\x05\x12\x0b\n\x03url\x18\x02 \x01(\t\x12\x0b\n\x03md5\x18\x03 \x01(\t\x12\x0c\n\x04name\x18\x04 \x02(\t\x12\r\n\x05\x62rief\x18\x05 \x01(\t\x12\x0f\n\x07orderid\x18\x06 \x01(\x05\x12\x13\n\x0bupdate_time\x18\x07 \x01(\t\x12\x13\n\x0b\x63reate_time\x18\x08 \x01(\t\x12\x10\n\x08\x61pk_name\x18\t \x02(\t\x12\r\n\x05price\x18\n \x02(\x01\x12\x0c\n\x04type\x18\x0b \x02(\x05\"L\n\x07\x41pkList\x12\"\n\x04head\x18\x01 \x02(\x0b\x32\x14.apk.CommandResponse\x12\x1d\n\x08\x61pk_list\x18\x02 \x03(\x0b\x32\x0b.apk.OneApk\"]\n\tAdbDevice\x12\x11\n\tserial_no\x18\x01 \x02(\t\x12\r\n\x05state\x18\x02 \x01(\t\x12\r\n\x05model\x18\x03 \x01(\t\x12\x0f\n\x07product\x18\x04 \x01(\t\x12\x0e\n\x06\x64\x65vice\x18\x05 \x01(\t\"W\n\x0b\x44\x65vicesList\x12\"\n\x04head\x18\x01 \x02(\x0b\x32\x14.apk.CommandResponse\x12$\n\x0c\x64\x65vices_list\x18\x02 \x03(\x0b\x32\x0e.apk.AdbDevice')




_COMMAND = _descriptor.Descriptor(
  name='Command',
  full_name='apk.Command',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='cmd', full_name='apk.Command.cmd', index=0,
      number=1, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='cmd_no', full_name='apk.Command.cmd_no', index=1,
      number=2, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='timestamp', full_name='apk.Command.timestamp', index=2,
      number=3, type=3, cpp_type=2, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='param', full_name='apk.Command.param', index=3,
      number=4, type=9, cpp_type=9, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=27,
  serialized_end=99,
)


_COMMANDPROGRESS = _descriptor.Descriptor(
  name='CommandProgress',
  full_name='apk.CommandProgress',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='cmd', full_name='apk.CommandProgress.cmd', index=0,
      number=1, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='cmd_no', full_name='apk.CommandProgress.cmd_no', index=1,
      number=2, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='code', full_name='apk.CommandProgress.code', index=2,
      number=3, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='time_cost', full_name='apk.CommandProgress.time_cost', index=3,
      number=4, type=3, cpp_type=2, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='info', full_name='apk.CommandProgress.info', index=4,
      number=5, type=9, cpp_type=9, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=101,
  serialized_end=194,
)


_COMMANDRESPONSE = _descriptor.Descriptor(
  name='CommandResponse',
  full_name='apk.CommandResponse',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='cmd', full_name='apk.CommandResponse.cmd', index=0,
      number=1, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='cmd_no', full_name='apk.CommandResponse.cmd_no', index=1,
      number=2, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='code', full_name='apk.CommandResponse.code', index=2,
      number=3, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='time_cost', full_name='apk.CommandResponse.time_cost', index=3,
      number=4, type=3, cpp_type=2, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='info', full_name='apk.CommandResponse.info', index=4,
      number=5, type=9, cpp_type=9, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=196,
  serialized_end=289,
)


_ONEAPK = _descriptor.Descriptor(
  name='OneApk',
  full_name='apk.OneApk',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='id', full_name='apk.OneApk.id', index=0,
      number=1, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='url', full_name='apk.OneApk.url', index=1,
      number=2, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='md5', full_name='apk.OneApk.md5', index=2,
      number=3, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='name', full_name='apk.OneApk.name', index=3,
      number=4, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='brief', full_name='apk.OneApk.brief', index=4,
      number=5, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='orderid', full_name='apk.OneApk.orderid', index=5,
      number=6, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='update_time', full_name='apk.OneApk.update_time', index=6,
      number=7, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='create_time', full_name='apk.OneApk.create_time', index=7,
      number=8, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='apk_name', full_name='apk.OneApk.apk_name', index=8,
      number=9, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='price', full_name='apk.OneApk.price', index=9,
      number=10, type=1, cpp_type=5, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='type', full_name='apk.OneApk.type', index=10,
      number=11, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=292,
  serialized_end=473,
)


_APKLIST = _descriptor.Descriptor(
  name='ApkList',
  full_name='apk.ApkList',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='head', full_name='apk.ApkList.head', index=0,
      number=1, type=11, cpp_type=10, label=2,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='apk_list', full_name='apk.ApkList.apk_list', index=1,
      number=2, type=11, cpp_type=10, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=475,
  serialized_end=551,
)


_ADBDEVICE = _descriptor.Descriptor(
  name='AdbDevice',
  full_name='apk.AdbDevice',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='serial_no', full_name='apk.AdbDevice.serial_no', index=0,
      number=1, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='state', full_name='apk.AdbDevice.state', index=1,
      number=2, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='model', full_name='apk.AdbDevice.model', index=2,
      number=3, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='product', full_name='apk.AdbDevice.product', index=3,
      number=4, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='device', full_name='apk.AdbDevice.device', index=4,
      number=5, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=553,
  serialized_end=646,
)


_DEVICESLIST = _descriptor.Descriptor(
  name='DevicesList',
  full_name='apk.DevicesList',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='head', full_name='apk.DevicesList.head', index=0,
      number=1, type=11, cpp_type=10, label=2,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='devices_list', full_name='apk.DevicesList.devices_list', index=1,
      number=2, type=11, cpp_type=10, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=648,
  serialized_end=735,
)

_APKLIST.fields_by_name['head'].message_type = _COMMANDRESPONSE
_APKLIST.fields_by_name['apk_list'].message_type = _ONEAPK
_DEVICESLIST.fields_by_name['head'].message_type = _COMMANDRESPONSE
_DEVICESLIST.fields_by_name['devices_list'].message_type = _ADBDEVICE
DESCRIPTOR.message_types_by_name['Command'] = _COMMAND
DESCRIPTOR.message_types_by_name['CommandProgress'] = _COMMANDPROGRESS
DESCRIPTOR.message_types_by_name['CommandResponse'] = _COMMANDRESPONSE
DESCRIPTOR.message_types_by_name['OneApk'] = _ONEAPK
DESCRIPTOR.message_types_by_name['ApkList'] = _APKLIST
DESCRIPTOR.message_types_by_name['AdbDevice'] = _ADBDEVICE
DESCRIPTOR.message_types_by_name['DevicesList'] = _DEVICESLIST

class Command(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _COMMAND

  # @@protoc_insertion_point(class_scope:apk.Command)

class CommandProgress(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _COMMANDPROGRESS

  # @@protoc_insertion_point(class_scope:apk.CommandProgress)

class CommandResponse(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _COMMANDRESPONSE

  # @@protoc_insertion_point(class_scope:apk.CommandResponse)

class OneApk(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _ONEAPK

  # @@protoc_insertion_point(class_scope:apk.OneApk)

class ApkList(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _APKLIST

  # @@protoc_insertion_point(class_scope:apk.ApkList)

class AdbDevice(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _ADBDEVICE

  # @@protoc_insertion_point(class_scope:apk.AdbDevice)

class DevicesList(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _DEVICESLIST

  # @@protoc_insertion_point(class_scope:apk.DevicesList)


# @@protoc_insertion_point(module_scope)
