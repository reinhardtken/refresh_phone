// Copyright 2011, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)
#pragma  once
#ifndef MUDUO_EXAMPLES_CODEC_CODEC_H
#define MUDUO_EXAMPLES_CODEC_CODEC_H

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
#include "base/logging.h"
#include "net/base/io_buffer.h"
#include "base/bind.h"


//#include <google/protobuf/message.h>

#include "../../include/ctp_define.h"

#include "codec_util.h"

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


//typedef std::shared_ptr<google::protobuf::Message> MessagePtr;

//
// FIXME: merge with RpcCodec
//
namespace codec {



class ProtobufCodec;
class TLVCodec;





class PHONE_COMMON_EXPORT Codec {
 public:

  enum ErrorCode {
    kNoError = 0,
    kInvalidLength,
    kCheckSumError,
    kInvalidNameLen,
    kUnknownMessageType,
    kParseError,
  };


  Codec(std::string const & name)
    :errorCallback_(base::Bind(&Codec::DefaultErrorCallback)) {
    //protobuf_codec_(last_received_data_) {
    if (Shakehand(name)) {

    } else {
      DCHECK(false);
    }
  }


  Codec(KnowSocketNameCallback const & know)
      :know_socket_name_callback_(know),
      errorCallback_(base::Bind(&Codec::DefaultErrorCallback)),
      //protobuf_codec_(last_received_data_),
      stage_(STAGE_SHAKEHAND) {
  }


  void OnMessage(channel::TCPSocket* who,
                  net::IOBuffer* buf,
                  int buf_len,
                  base::Time const& receiveTime);


  void RegisterProtobufHandler(std::string const & name, const ProtobufMessageCallback& messageCb);
  void RegisterTLVHandler(std::string const & name, const TLVMessageCallback& messageCb);

  static const std::string ErrorCodeToString(int32 errorCode);

 private:
  enum Stage {
    STAGE_SHAKEHAND,
    STAGE_PROTOBUF,
    STAGE_TLV,
  };


  static void DefaultErrorCallback(channel::TCPSocket &, base::Time const&, int32 const);

  bool Shakehand(std::string const & scheme);


  //ProtobufMessageCallback messageCallback_;
  ErrorCallback errorCallback_;
  KnowSocketNameCallback know_socket_name_callback_;

  std::vector<char> last_received_data_;
  
  Stage stage_;
  Stage stage() { return stage_; }
  void stage(Stage const s) { stage_ = s; }

  
  scoped_ptr<ProtobufCodec> protobuf_codec_;
  scoped_ptr<TLVCodec> tlv_codec_;


  DISALLOW_COPY_AND_ASSIGN(Codec);
};

}

#endif  // MUDUO_EXAMPLES_CODEC_CODEC_H
