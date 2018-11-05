// Copyright 2011, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)
#pragma  once


// #include <muduo/net/Buffer.h>
// #include <muduo/net/TcpConnection.h>

// #include <boost/function.hpp>
// #include <boost/noncopyable.hpp>
// #include <boost/shared_ptr.hpp>


#include <string>
#include <vector>
#include <memory>


#include "base/callback.h"
#include "base/time.h"
#include "base/bind.h"

#include <google/protobuf/message.h>

#include "../../include/ctp_define.h"

#include "tlv_define.h"

namespace channel {
  class TCPSocket;
}




namespace codec {
  const int kHeaderLen = sizeof(int32);
  const int kMinMessageLen = 2*kHeaderLen + 2; // nameLen + typeName + checkSum
  const int kMaxMessageLen = 64*1024*1024; // same as codec_stream.h kDefaultTotalBytesLimit

  //scheme://business
  //因为现在某个语言就是某个业务，所以先这样=
  extern PHONE_COMMON_EXPORT char const * kProtobuf;
  extern PHONE_COMMON_EXPORT char const * kTLV;
  extern PHONE_COMMON_EXPORT char const * kProtobufCpp;
  extern PHONE_COMMON_EXPORT char const * kProtobufPython;
  extern PHONE_COMMON_EXPORT char const * kTLVRlang;

  void CopyRestData(uint32 const len, std::vector<char> &last_received_data);
  bool HasMoreData(uint32 const len, std::vector<char> &last_received_data);
  bool HasHead(int size);
//========================================================================================================
  typedef std::shared_ptr<google::protobuf::Message> MessagePtr;
  typedef base::Callback<void(std::string const & name, MessagePtr const&, base::Time const&)> ProtobufMessageCallback;
  typedef base::Callback<void(channel::TCPSocket &, base::Time const&, int32 const)> ErrorCallback;
  typedef base::Callback<void(channel::TCPSocket const* p, std::string const &name)> KnowSocketNameCallback;

  enum ErrorCode
  {
    kNoError = 0,
    kInvalidLength,
    kCheckSumError,
    kInvalidNameLen,
    kUnknownMessageType,
    kParseError,
  };


//======================================================================================================== 
  typedef std::vector<TLVObject> TLVObjectVector;
  typedef std::shared_ptr<TLVObjectVector> TLVMessagePtr;
  typedef base::Callback<void(std::string const & name, TLVMessagePtr const&, base::Time const&)> TLVMessageCallback;

}
