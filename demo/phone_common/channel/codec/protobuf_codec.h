// Copyright 2011, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)
#pragma  once
#ifndef MUDUO_EXAMPLES_PROTOBUF_CODEC_CODEC_H
#define MUDUO_EXAMPLES_PROTOBUF_CODEC_CODEC_H

// #include <muduo/net/Buffer.h>
// #include <muduo/net/TcpConnection.h>

// #include <boost/function.hpp>
// #include <boost/noncopyable.hpp>
// #include <boost/shared_ptr.hpp>

#include <memory>
#include <string>
#include <vector>


#include "base/callback.h"
#include "base/time.h"
#include "net/base/io_buffer.h"
#include "base/bind.h"


#include <google/protobuf/message.h>

#include "../../include/ctp_define.h"

// struct ProtobufTransportFormat __attribute__ ((__packed__))
// {
//   int32_t  len;
//   int32_t  nameLen;
//   char     typeName[nameLen];
//   char     protobufData[len-nameLen-8];
//   int32_t  checkSum; // adler32 of nameLen, typeName and protobufData
// }
#include "codec_util.h"

//代码来自muduo网络库，去掉boost和对原有库的依赖





//
// FIXME: merge with RpcCodec
//

namespace codec {
  class PHONE_COMMON_EXPORT ProtobufCodec {
   public:
    

    ProtobufCodec(std::vector<char> &last_received_data)
      :last_received_data_(last_received_data) {
    }


    void RegisterHandler(std::string const & name, const ProtobufMessageCallback& messageCb) {
      message_callback_[name] = messageCb;
      errorCallback_ = base::Bind(&ProtobufCodec::DefaultErrorCallback);
    }

    void RegisterHandler(std::string const & name, const ProtobufMessageCallback& messageCb, const ErrorCallback& errorCb) {
      message_callback_[name] = messageCb;
      errorCallback_ = errorCb;
    }

    void OnMessage(std::string const & name, base::Time const& receiveTime);

    static const std::string ErrorCodeToString(int32 errorCode);
    static void FillEmptyBuffer(const google::protobuf::Message& message, std::vector<char> *out);
    static google::protobuf::Message* CreateMessage(const std::string& type_name);
    //static MessagePtr Parse(const char* buf, int len, int32* errorCode);

   private:
    static void DefaultErrorCallback(channel::TCPSocket &, base::Time const&, int32 const);

    ProtobufMessageCallback messageCallback_;
    std::map<std::string, codec::ProtobufMessageCallback> message_callback_;
    ErrorCallback errorCallback_;

    std::vector<char> &last_received_data_;


    DISALLOW_COPY_AND_ASSIGN(ProtobufCodec);
  };
}


#endif  // MUDUO_EXAMPLES_PROTOBUF_CODEC_CODEC_H
