// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <string>
#include "ctp_socket.h"


// This looks like it should be forward-declarable, but it does some tricky
// moves that make it easier to just include it.
#include "net/socket/tcp_client_socket.h"
#include "net/socket/tcp_server_socket.h"

#include "../include/ctp_define.h"

#include "./codec/codec.h"
#include "./codec/codec_util.h"


namespace net {
class Socket;
}

namespace channel {

class PHONE_COMMON_EXPORT TCPSocket : public Socket {
 public:
  enum SocketMode {
    UNKNOWN = 0,
    CLIENT,
    SERVER,
  };


  TCPSocket(SocketMode const mode, std::string const & name);
  TCPSocket(net::TCPClientSocket* tcp_client_socket,
            codec::KnowSocketNameCallback const & know);

  virtual ~TCPSocket();

  virtual void Connect(const std::string& address,
                       int port,
                       const CompletionCallback& callback) OVERRIDE;
  virtual void Disconnect() OVERRIDE;
  virtual int Bind(const std::string& address, int port) OVERRIDE;
  virtual void Read(int count,
                    const ReadCompletionCallback& callback) OVERRIDE;
  virtual void RecvFrom(int count,
                        const RecvFromCompletionCallback& callback) OVERRIDE;
  virtual void SendTo(scoped_refptr<net::IOBuffer> io_buffer,
                      int byte_count,
                      const std::string& address,
                      int port,
                      const CompletionCallback& callback) OVERRIDE;
  virtual bool SetKeepAlive(bool enable, int delay) OVERRIDE;
  virtual bool SetNoDelay(bool no_delay) OVERRIDE;
  virtual int Listen(const std::string& address, int port,
                     int backlog, std::string* error_msg) OVERRIDE;
  virtual void Accept(const AcceptCompletionCallback &callback) OVERRIDE;

  virtual bool GetPeerAddress(net::IPEndPoint* address) OVERRIDE;
  virtual bool GetLocalAddress(net::IPEndPoint* address) OVERRIDE;
  virtual Socket::SocketType GetSocketType() const OVERRIDE;

  static TCPSocket* CreateSocketForTesting(
      net::TCPClientSocket* tcp_client_socket,
      const std::string& owner_extension_id);
  static TCPSocket* CreateServerSocketForTesting(
      net::TCPServerSocket* tcp_server_socket,
      const std::string& owner_extension_id);

  


  void SocketAlreadyKnowName(TCPSocket const* p, std::string const &name);
  void SendShakehand(const net::CompletionCallback& callback);
  

  SocketMode mode() { return socket_mode_; }
  std::string const & name() { return name_; }
  codec::Codec * common_codec() { return common_codec_.get(); }

 protected:
   //为什么是protected，这个还有子类？？？=
/*
  virtual int WriteImpl(net::IOBuffer* io_buffer,
                        int io_buffer_size,
                        const net::CompletionCallback& callback) OVERRIDE;*/

   virtual int WriteImpl(net::IOBuffer* io_buffer,
     int io_buffer_size,
     const net::CompletionCallback& callback) OVERRIDE;

 private:
  void OnConnectComplete(int result);
  void OnReadComplete(scoped_refptr<net::IOBuffer> io_buffer,
                      int result);
  void OnAccept(int result);

  // TCPSocket(net::TCPServerSocket* tcp_server_socket,
  //           const std::string& owner_extension_id);

  scoped_ptr<net::TCPClientSocket> socket_;
  scoped_ptr<net::TCPServerSocket> server_socket_;

  
  SocketMode socket_mode_;

  CompletionCallback connect_callback_;

  ReadCompletionCallback read_callback_;

  scoped_ptr<net::StreamSocket> accept_socket_;
  AcceptCompletionCallback accept_callback_;


  //===========================================================
  scoped_ptr<codec::Codec> common_codec_;
  std::string name_;
  codec::KnowSocketNameCallback know_socket_name_callback_;

};

}  //  namespace extensions


