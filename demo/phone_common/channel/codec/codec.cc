// Copyright 2011, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#include "codec.h"

//#include <muduo/base/Logging.h>
//#include <muduo/net/Endian.h>
//#include <muduo/net/protorpc/google-inl.h>
#include "base/basictypes.h"
#include "base/logging.h"
#include "base/string_util.h"


#include <google/protobuf/message_lite.h>
#include <google/protobuf/descriptor.h>

//#include <zlib.h>  // adler32

//using namespace muduo;
//using namespace muduo::net;

#include "protobuf_codec.h"
#include "tlv_codec.h"
#include "../ctp_tcp_socket.h"

namespace {
  const char* kNoErrorStr = "NoError";
  const char* kInvalidLengthStr = "InvalidLength";
  const char* kCheckSumErrorStr = "CheckSumError";
  const char* kInvalidNameLenStr = "InvalidNameLen";
  const char* kUnknownMessageTypeStr = "UnknownMessageType";
  const char* kParseErrorStr = "ParseError";
  const char* kUnknownErrorStr = "UnknownError";
}



//
// no more google code after this
//

//
// FIXME: merge with RpcCodec
//

namespace codec {



const std::string Codec::ErrorCodeToString(int32 errorCode) {
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

void Codec::DefaultErrorCallback(channel::TCPSocket &, base::Time const&, int32 const errorCode) {
  LOG(ERROR) << "Codec::defaultErrorCallback - " << ErrorCodeToString(errorCode);
  DCHECK(false);
  // if (conn && conn->connected())
  // {
  //   conn->shutdown();
  // }
}

static int32 asInt32(const char* buf) {
  int32 be32 = 0;
  ::memcpy(&be32, buf, sizeof(be32));
  //在同一台机器上做本地回环，不需要字节序转换。。。=
  be32 = *((int32*)buf);
  return be32;
  //return sockets::networkToHost32(be32);=
}

bool Codec::Shakehand(std::string const & scheme) {
  if (StartsWithASCII(scheme, kProtobufPython, true) || StartsWithASCII(scheme, kProtobufCpp, true)) {
    if (!protobuf_codec_.get()) {
      protobuf_codec_.reset(new ProtobufCodec(last_received_data_));
    } else {
      DCHECK(false);
    }
    stage(STAGE_PROTOBUF);
    return true;
  } else if (StartsWithASCII(scheme, kTLVRlang, true)) {
    if (!tlv_codec_.get()) {
      tlv_codec_.reset(new TLVCodec(last_received_data_));
    } else {
      DCHECK(false);
    }
    stage(STAGE_TLV);
    return true;
  } else {
    DCHECK(false);
  }

  return false;
}

void Codec::OnMessage(channel::TCPSocket* who,
                      net::IOBuffer* buf,
                      int buf_len,
                      base::Time const& receiveTime) {
  //不考虑效率的，每次都把buf的数据拷贝到last_received_data_末尾，然后处理last_received_data_=
  //没处理完一条消息，都把后面的数据移到最开始，然后从头处理下一条。=
  //每次循环完成last_received_data_中只有下一条非完整数据的开头，同时也不会有更多的未使用size=
  uint32 size = last_received_data_.size();
  last_received_data_.resize(size + buf_len);
  memcpy(&last_received_data_[size], buf->data(), buf_len);

  while(true) {
     if (HasHead(last_received_data_.size())) {
  
      uint32 len = *(uint32*)&last_received_data_[0];
      if (codec::kHeaderLen + len <= last_received_data_.size()) {
        if (stage() == STAGE_SHAKEHAND) {
          //增加一个握手阶段，这样r和python可以共用一个tcp框架。握手阶段是一个四字节长度后面跟着字符串，表明我是什么客户端=
          //有至少一条完整的数据到达，开始处理=
          std::string scheme(&last_received_data_[codec::kHeaderLen], len);
          //r的填充socket函数，如果是字符串，总是自作主张加个null，为了容错，如果最后一个字符是null，干掉=
          if (scheme.at(scheme.size() - 1) == 0) {
            scheme.erase(scheme.size() - 1, 1);
          }
          if (Shakehand(scheme)) {
            //握手成功，余下的数据处理根据由具体协议codec负责=
            //在这个之前，通知host，已经知道这个socket走什么业务用什么协议了，这样在处理数据的过程中，可能会发生发送数据请求=
            //发送请求根据业务协议名称请求发送，host要知道业务名称对应什么socket=
            know_socket_name_callback_.Run(who, scheme);
            know_socket_name_callback_.Reset();
            if (codec::HasMoreData(len, last_received_data_)) {
              continue;
            } else {
              return;
            }
          } else {
            DCHECK(false);
          }
        } else if (stage() == STAGE_PROTOBUF) {
          protobuf_codec_->OnMessage(who->name(), receiveTime);
          return;
        } else if (stage() == STAGE_TLV) {
          tlv_codec_->OnMessage(who->name(), receiveTime);
          return;
        } else {
          DCHECK(false);
          return;
        }
      } else {
        return;
      }
    } else {
      return;
    }
  }
}


void Codec::RegisterProtobufHandler(std::string const & name, const ProtobufMessageCallback& messageCb) {
  if (protobuf_codec_.get()) {
    protobuf_codec_->RegisterHandler(name, messageCb);
  } else {
    DCHECK(false);
  }
}

void Codec::RegisterTLVHandler(std::string const & name, const TLVMessageCallback& messageCb) {
  if (tlv_codec_.get()) {
    tlv_codec_->RegisterHandler(name, messageCb);
  } else {
    DCHECK(false);
  }
}


}