// Copyright 2011, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.
//
// Author: Shuo Chen (chenshuo at chenshuo dot com)

#include "codec_util.h"

//#include <muduo/base/Logging.h>
//#include <muduo/net/Endian.h>
//#include <muduo/net/protorpc/google-inl.h>
#include "base/basictypes.h"


namespace codec {
  //const int kHeaderLen = sizeof(int32);
  //const int kMinMessageLen = 2*kHeaderLen + 2; // nameLen + typeName + checkSum
  //const int kMaxMessageLen = 64*1024*1024; // same as codec_stream.h kDefaultTotalBytesLimit

  char const * kProtobuf = "protobuf://";
  char const * kTLV = "tlv://";
  char const * kProtobufCpp = "protobuf://cpp/";
  char const * kProtobufPython = "protobuf://python/";
  char const * kTLVRlang = "tlv://rlang/";


  void CopyRestData(uint32 const len, std::vector<char> &last_received_data) {
    std::vector<char> v(last_received_data.size() - (len + kHeaderLen));
    memcpy(&v[0], &last_received_data[(len + kHeaderLen)], v.size());
    last_received_data.clear();
    last_received_data.swap(v);
  }

  bool HasMoreData(uint32 const len, std::vector<char> &last_received_data) {
    if (last_received_data.size() > (len + kHeaderLen)) {
      CopyRestData(len, last_received_data);
      return true;
    } else {
      last_received_data.clear();
      return false;
    }
  }

  bool HasHead(int size) {
    return size >= kHeaderLen;
  } 













}


