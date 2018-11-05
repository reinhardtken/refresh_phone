// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <string>
#include "ctp_tcp_socket.h"


// This looks like it should be forward-declarable, but it does some tricky
// moves that make it easier to just include it.
#include "net/socket/tcp_client_socket.h"
#include "net/socket/tcp_server_socket.h"


#include "./protobuf/codec/codec.h"

namespace net {
class Socket;
}

namespace channel {

class CompleteHandler {
 public:
  virtual void OnWriteComplete(channel::TCPSocket*, int result_code) = 0;
  virtual void OnReadComplete (channel::TCPSocket*, int result_code,
      scoped_refptr<net::IOBuffer> io_buffer) = 0;
  virtual void OnAccept(int, net::TCPClientSocket*) = 0;
  virtual void OnConnect(int) = 0;
};


//=======================================================
class ChannelHost : public CompleteHandler {
 public:
  ChannelHost();
  void InitializeServer();
  void InitializeClient();

  ~ChannelHost() {}

  void CodecCallback(MessagePtr const&, base::Time const&);

 private:
  virtual void OnWriteComplete(channel::TCPSocket*, int result_code) OVERRIDE;
  virtual void OnReadComplete (channel::TCPSocket*, int result_code,
      scoped_refptr<net::IOBuffer> io_buffer) OVERRIDE;
  virtual void OnAccept(int, net::TCPClientSocket*) OVERRIDE;
  virtual void OnConnect(int) OVERRIDE;

  scoped_ptr<channel::TCPSocket> listen_socket_;
  //现在只有一个python，将来有更多的再说=
  scoped_ptr<channel::TCPSocket> python_socket_;
  
  ProtobufCodec protobuf_codec_;



private:
  DISALLOW_COPY_AND_ASSIGN(ChannelHost);
};

}  //  namespace extensions


