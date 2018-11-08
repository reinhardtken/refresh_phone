// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <string>
#include <functional>
#include <tuple>

#include "ctp_tcp_socket.h"

#include "base/cancelable_callback.h"
// This looks like it should be forward-declarable, but it does some tricky
// moves that make it easier to just include it.
#include "net/socket/tcp_client_socket.h"
#include "net/socket/tcp_server_socket.h"



#include "../common/common_thread.h"
#include "../include/ctp_define.h"
#include "codec/codec_util.h"
#include "codec/protobuf_codec.h"
#include "codec/tlv_codec.h"

namespace net {
class Socket;
}

namespace channel {
  typedef std::pair<bool, int> ServerResult;
class CompleteHandler {
 public:
  virtual void OnWriteComplete(channel::TCPSocket*, int result_code) = 0;
  virtual void OnReadComplete (channel::TCPSocket*, int result_code,
      scoped_refptr<net::IOBuffer> io_buffer) = 0;
  virtual void OnAccept(int, net::TCPClientSocket*) = 0;
  virtual void OnConnect(channel::TCPSocket*, int) = 0;
};

//=======================================================
class PHONE_COMMON_EXPORT ChannelHost : public CompleteHandler {
 public:
  // struct Delegate {
  //   virtual void Dispatch(codec::MessagePtr const &) = 0;
  // };
  ChannelHost();
  
  ServerResult InitializeServer(std::string const &name, bool const retry=true);
  void InitializeClient(std::string const &name, std::string const &server_name);
  void SendProtobufMsg(std::string const& name, codec::MessagePtr const &);
  void SendTLVMsg(std::string const& name, codec::TLVMessagePtr const &);
  bool HasConnection(std::string const &name);
  ~ChannelHost();

  void ProtobufCallback(std::string const& name, codec::MessagePtr const&, base::Time const&);
  void TLVCallback(std::string const& name, codec::TLVMessagePtr const&, base::Time const&);
  //codec::Codec & common_codec() { return common_codec_; }
  void SocketAlreadyKnowName(TCPSocket const* p, std::string const &name);

  void RegisterProtobufHandler(std::string const & name, const codec::ProtobufMessageCallback& messageCb);
  void RegisterTLVHandler(std::string const & name, const codec::TLVMessageCallback& messageCb);

  std::string const & server_name() { return name_; }

  void StartKeepAlive(std::string const &name, std::string const &target_name);
  void StopKeepAlive(std::string const &name);

  void RegisterConnectStatusHandler(std::string const &name, std::function<void(std::string const &,bool)> const &func);

private:
  friend class channel::TCPSocket;
  //void CheckAlive();
  virtual void OnWriteComplete(channel::TCPSocket*, int result_code) OVERRIDE;
  virtual void OnReadComplete (channel::TCPSocket*, int result_code,
      scoped_refptr<net::IOBuffer> io_buffer) OVERRIDE;
  virtual void OnAccept(int, net::TCPClientSocket*) OVERRIDE;
  virtual void OnConnect(channel::TCPSocket*, int) OVERRIDE;

  void AsyncRead(channel::TCPSocket* who);

  scoped_ptr<channel::TCPSocket> listen_socket_;
  //现在只有一个python，将来有更多的再说=
  //scoped_ptr<channel::TCPSocket> python_socket_;=

  //NSMap用来在业务要发送的时候根据业务名称找对应的socket=
  //SNMap用来在接收的时候，根据socket找对应的handle=
  //SNMap刚连上的时候，因为不晓得走什么协议，名字是空的，=
  //在握手完成之后再补上=
  typedef std::map<std::string, TCPSocket*> NSMap;
  typedef std::map<std::shared_ptr<TCPSocket>, std::string> SNMap;
  SNMap socket_name_map_;
  NSMap name_socket_map_;
  typedef std::map<std::string, std::function<void(std::string const &, bool)>> ConnectCallbackMap;
  ConnectCallbackMap connect_map_;


  std::map<std::string, codec::ProtobufMessageCallback> protobuf_callback_map_;
  std::map<std::string, codec::TLVMessageCallback> tlv_callback_map_;
  //codec::ProtobufMessageCallback protobuf_callback_;
  //codec::TLVMessageCallback tlv_callback_;
  std::string name_;

  typedef base::CancelableCallback<void(void)> VoidVoidCallback;
  //scoped_ptr<VoidVoidCallback> alive_check_;

  std::map<std::string, std::string> alive_check_list_;
  std::function<void()> keep_alive_checker_;
  void KeepAlive();
  
  CommonThread::ID id_;
  CommonThread::ID id() { return id_; }
  //Delegate *delegate_;

  //  [2/21/2014 ken] 目前写套接字有一个严重错误，就是没判断是否可写就写，先加个变量，方便出问题的时候查==
  int last_write_status_;

private:
  DISALLOW_COPY_AND_ASSIGN(ChannelHost);
};

}  //  namespace extensions


