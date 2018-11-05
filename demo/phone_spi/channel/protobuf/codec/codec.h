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

// struct ProtobufTransportFormat __attribute__ ((__packed__))
// {
//   int32_t  len;
//   int32_t  nameLen;
//   char     typeName[nameLen];
//   char     protobufData[len-nameLen-8];
//   int32_t  checkSum; // adler32 of nameLen, typeName and protobufData
// }


//代码来自muduo网络库，去掉boost和对原有库的依赖
namespace channel {
  class TCPSocket;
}


typedef std::shared_ptr<google::protobuf::Message> MessagePtr;

//
// FIXME: merge with RpcCodec
//
class ProtobufCodec
{
 public:

  enum ErrorCode
  {
    kNoError = 0,
    kInvalidLength,
    kCheckSumError,
    kInvalidNameLen,
    kUnknownMessageType,
    kParseError,
  };

  // typedef boost::function<void (const muduo::net::TcpConnectionPtr&,
  //                               const MessagePtr&,
  //                               muduo::Timestamp)> ProtobufMessageCallback;

  // typedef boost::function<void (const muduo::net::TcpConnectionPtr&,
  //                               muduo::net::Buffer*,
  //                               muduo::Timestamp,
  //                               ErrorCode)> ErrorCallback;

  typedef base::Callback<void(MessagePtr const&, base::Time const&)> ProtobufMessageCallback;
  typedef base::Callback<void(channel::TCPSocket &, base::Time const&, int32 const)> ErrorCallback;

  explicit ProtobufCodec(const ProtobufMessageCallback& messageCb)
    : messageCallback_(messageCb),
      errorCallback_(base::Bind(&ProtobufCodec::DefaultErrorCallback))
  {
  }

  ProtobufCodec(const ProtobufMessageCallback& messageCb, const ErrorCallback& errorCb)
    : messageCallback_(messageCb),
      errorCallback_(errorCb)
  {
  }

  void OnMessage(net::IOBuffer* buf,
                 int buf_len,
                 base::Time const& receiveTime);

  void Send(const channel::TCPSocket& conn,
            const google::protobuf::Message& message)
  {
    // FIXME: serialize to TcpConnection::outputBuffer()
    //net::IOBuffer buf;
    //fillEmptyBuffer(&buf, message);
    //conn->send(&buf);
  }

  static const std::string ErrorCodeToString(int32 errorCode);
  static void FillEmptyBuffer(const google::protobuf::Message& message, std::vector<char> *out);
  static google::protobuf::Message* CreateMessage(const std::string& type_name);
  //static MessagePtr Parse(const char* buf, int len, int32* errorCode);

 private:
  static void DefaultErrorCallback(channel::TCPSocket &, base::Time const&, int32 const);
  static bool HasHead(int size);

  ProtobufMessageCallback messageCallback_;
  ErrorCallback errorCallback_;

  std::vector<char> last_received_data_;

  const static int kHeaderLen = sizeof(int32);
  const static int kMinMessageLen = 2*kHeaderLen + 2; // nameLen + typeName + checkSum
  const static int kMaxMessageLen = 64*1024*1024; // same as codec_stream.h kDefaultTotalBytesLimit


  DISALLOW_COPY_AND_ASSIGN(ProtobufCodec);
};

#endif  // MUDUO_EXAMPLES_PROTOBUF_CODEC_CODEC_H
