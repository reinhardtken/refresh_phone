// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "channel_host.h"

#include "base/bind.h"
#include "net/base/address_list.h"
#include "net/base/io_buffer.h"
#include "net/base/ip_endpoint.h"
#include "net/base/net_errors.h"
#include "net/socket/socket.h"


#include "../../protobuf/out/cc_comunication.pb.h"

namespace {
  uint32 const kReadBufferSize = 4096;
  char const* noused = "";

    
}

namespace channel {

  ChannelHost::ChannelHost()
    :protobuf_codec_(base::Bind(&ChannelHost::CodecCallback, base::Unretained(this))) {

  }

  void ChannelHost::InitializeServer() {
    //测试protobuf的reflection
    ctp::c2c::Command c;
    c.set_cmd("hello");
    c.set_params("world");

    ctp::c2c::Command c2;
    c2.set_cmd("fine=======");
    c2.set_params("thanku");
    
    std::vector<char> out;
    protobuf_codec_.FillEmptyBuffer(c, &out);
    std::vector<char> out2;
    protobuf_codec_.FillEmptyBuffer(c2, &out2);
    std::vector<char> out3(out.size() + out2.size());
    memcpy(&out3[0], out.data(), out.size());
    memcpy(&out3[out.size()], out2.data(), out2.size());

    scoped_refptr<net::IOBufferWithSize> buffer = new net::IOBufferWithSize(out3.size());
    memcpy(buffer->data(), out3.data(), out3.size());
    protobuf_codec_.OnMessage(buffer.get(), buffer->size(), base::Time::Now());
    //=============================================
    listen_socket_.reset(new TCPSocket(noused));
    std::string error;
    int re = listen_socket_->Listen("127.0.0.1", 6789, 5, &error);
    DCHECK(re == 0);
    listen_socket_->Accept(base::Bind(&ChannelHost::OnAccept,
        base::Unretained(this)));
  }

  void ChannelHost::InitializeClient() {
    python_socket_.reset(new TCPSocket(noused));
    python_socket_->Connect("127.0.0.1", 6789, base::Bind(&ChannelHost::OnConnect, base::Unretained(this)));
  }

  void ChannelHost::OnWriteComplete(channel::TCPSocket* who, int result_code) {

  }

  void ChannelHost::OnReadComplete (channel::TCPSocket* who, int result_code,
      scoped_refptr<net::IOBuffer> io_buffer) {
    if (result_code > 0) {
      if (who == python_socket_.get()) {
        
      } else {
        DCHECK(false);
      }
    } else {
      DCHECK(false);
    }
  }

  void ChannelHost::OnAccept(int, net::TCPClientSocket* ps) {
    DCHECK(python_socket_.get() == NULL);
    python_socket_.reset(new TCPSocket(ps, noused, true));

    python_socket_->Read(kReadBufferSize,
      base::Bind(&ChannelHost::OnReadComplete,
        base::Unretained(this), python_socket_.get()));
  }

  void ChannelHost::OnConnect(int re) {
    if (re == net::OK) {
      python_socket_->Read(kReadBufferSize,
        base::Bind(&ChannelHost::OnReadComplete,
          base::Unretained(this), python_socket_.get()));
    } else {
      DCHECK(false);
    }
  }

  void ChannelHost::CodecCallback(MessagePtr const&, base::Time const&) {

  }
  

}  // namespace extensions
