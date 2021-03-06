// Copyright 2011, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#include "protobuf_codec.h"

//#include <muduo/base/Logging.h>
//#include <muduo/net/Endian.h>
//#include <muduo/net/protorpc/google-inl.h>
#include "base/basictypes.h"
#include "base/logging.h"


#include <google/protobuf/message_lite.h>
#include <google/protobuf/descriptor.h>

//#include <zlib.h>  // adler32

//using namespace muduo;
//using namespace muduo::net;


namespace {
  const char* kNoErrorStr = "NoError";
  const char* kInvalidLengthStr = "InvalidLength";
  const char* kCheckSumErrorStr = "CheckSumError";
  const char* kInvalidNameLenStr = "InvalidNameLen";
  const char* kUnknownMessageTypeStr = "UnknownMessageType";
  const char* kParseErrorStr = "ParseError";
  const char* kUnknownErrorStr = "UnknownError";
}

namespace codec {

void ProtobufCodec::FillEmptyBuffer(const google::protobuf::Message& message, std::vector<char> *out) {
  //这里应该有个自动增长的buf搞定的，不过没找到，先低效的跑通再说=

  DCHECK_RLOG(message.IsInitialized());
  std::vector<char> buf(2*kHeaderLen + message.GetTypeName().size() + message.ByteSize());
  uint32 writed = 4;//最开始四个字节用于记录总长度，最后回填=

  const std::string& typeName = message.GetTypeName();
  int32 nameLen = static_cast<int32>(typeName.size());
  *(int32*)&buf[writed] = nameLen;
  writed += 4;
  memcpy(&buf[writed], typeName.c_str(), nameLen);
  writed += nameLen;

  

  int byte_size = message.ByteSize();
  if (buf.size() - writed == (uint32)byte_size) {

    uint8* end = message.SerializeWithCachedSizesToArray((google::protobuf::uint8*)&buf[writed]);
    // if (end - start != byte_size)
    // {
    //   ByteSizeConsistencyError(byte_size, message.ByteSize(), static_cast<int>(end - start));
    // }
    writed += byte_size;

    *(int32*)&buf[0] = writed - 4;//总长度并不包括总长度自己这4个字节=
    out->swap(buf);
  } else {
    DCHECK(false);
  }
}



const std::string ProtobufCodec::ErrorCodeToString(int32 errorCode) {
  switch (errorCode) {
   case kNoError:
     return kNoErrorStr;
   case kInvalidLength:
     return kInvalidLengthStr;
   case kCheckSumError:
     return kCheckSumErrorStr;
   case kInvalidNameLen:
     return kInvalidNameLenStr;
   case kUnknownMessageType:
     return kUnknownMessageTypeStr;
   case kParseError:
     return kParseErrorStr;
   default:
     return kUnknownErrorStr;
  }
}

void ProtobufCodec::DefaultErrorCallback(channel::TCPSocket &, base::Time const&, int32 const errorCode) {
  LOG(ERROR) << "ProtobufCodec::defaultErrorCallback - " << ErrorCodeToString(errorCode);
  DCHECK(false);
}

static int32 asInt32(const char* buf) {
  int32 be32 = 0;
  ::memcpy(&be32, buf, sizeof(be32));
  //在同一台机器上做本地回环，不需要字节序转换。。。=
  be32 = *((int32*)buf);
  return be32;
  //return sockets::networkToHost32(be32);=
}


void ProtobufCodec::OnMessage(std::string const & name, base::Time const& receiveTime) {

  while(true) {
     if (HasHead(last_received_data_.size())) {
  
      uint32 len = *(uint32*)&last_received_data_[0];
      if (kHeaderLen + len <= last_received_data_.size()) {

        uint32 type_string_len = *(uint32*)&last_received_data_[kHeaderLen];
        DCHECK(type_string_len > 0 && type_string_len < last_received_data_.size() - 2*kHeaderLen);

        std::string type(&last_received_data_[2*kHeaderLen], type_string_len);
        MessagePtr message;
        message.reset(CreateMessage(type));
        if (message) {
          int32 dada_len = len - type_string_len - 1*kHeaderLen;
          if (message->ParseFromArray(&last_received_data_[2*kHeaderLen + type_string_len], dada_len)) {
            //调用分发函数=
            auto it = message_callback_.find(name);
            if (it != message_callback_.end()) {
              it->second.Run(name, message, receiveTime);
            }
            //把余下的数据copy到头部=
            if (codec::HasMoreData(len, last_received_data_)) {
              continue;
            } else {
              return;
            }

          } else {
            //DCHECK(false);
            CHECK(false);
            return;
          }

        } else {
          DCHECK(false);
          return;
        }

      } else {
        return;
      }
   } else {
     //  [1/18/2014 ken]没数据要退。。。==
     return;
   }
  }
}

google::protobuf::Message* ProtobufCodec::CreateMessage(const std::string& typeName)
{
  google::protobuf::Message* message = NULL;
  const google::protobuf::Descriptor* descriptor =
    google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(typeName);
  if (descriptor)
  {
    const google::protobuf::Message* prototype =
      google::protobuf::MessageFactory::generated_factory()->GetPrototype(descriptor);
    if (prototype)
    {
      message = prototype->New();
    }
  }
  return message;
}


}
