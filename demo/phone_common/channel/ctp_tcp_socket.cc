// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ctp_tcp_socket.h"


#include "base/string_util.h"

#include "net/base/address_list.h"
#include "net/base/ip_endpoint.h"
#include "net/base/net_errors.h"
#include "net/base/rand_callback.h"
#include "net/socket/tcp_client_socket.h"

#include "codec/protobuf_codec.h"
#include "codec/tlv_codec.h"
#include "codec/codec_util.h"
#include "../channel/ctp_tcp_socket.h"

namespace channel {

const char kTCPSocketTypeInvalidError[] =
    "Cannot call both connect and listen on the same socket.";
const char kSocketListenError[] = "Could not listen on the specified port.";

//服务器状态只有一种，客户端有两种，走这个路径的，是主动连的客户端=
TCPSocket::TCPSocket(SocketMode const mode, std::string const & name)
    :socket_mode_(mode),
    name_(name) {
  if (mode == CLIENT) {
    //主动连接的客户端，知道自己采用的编码方式，解码时，跳过握手阶段
    common_codec_.reset(new codec::Codec(name));
    
  } else if (mode == SERVER) {

  } else {
    DCHECK(false);
  }
}

//客户端走这个路径创建的时候，是被动连，可肯定已经连上了
TCPSocket::TCPSocket(net::TCPClientSocket* tcp_client_socket,
                     codec::KnowSocketNameCallback const & know)
    : socket_(tcp_client_socket),
      know_socket_name_callback_(know),
      socket_mode_(CLIENT) {
  common_codec_.reset(new codec::Codec(base::Bind(&TCPSocket::SocketAlreadyKnowName, base::Unretained(this))));
  this->is_connected_ = true;
}

// TCPSocket::TCPSocket(net::TCPServerSocket* tcp_server_socket,
//                      const std::string& owner_extension_id)
//     : Socket(owner_extension_id),
//       server_socket_(tcp_server_socket),
//       socket_mode_(SERVER) {
// }

// static
// TCPSocket* TCPSocket::CreateSocketForTesting(
//     net::TCPClientSocket* tcp_client_socket,
//     const std::string& owner_extension_id) {
//   return new TCPSocket(tcp_client_socket, owner_extension_id);
// }

// static
// TCPSocket* TCPSocket::CreateServerSocketForTesting(
//     net::TCPServerSocket* tcp_server_socket,
//     const std::string& owner_extension_id) {
//   return new TCPSocket(tcp_server_socket, owner_extension_id);
// }

TCPSocket::~TCPSocket() {
  Disconnect();
}

void TCPSocket::Connect(const std::string& address,
                        int port,
                        const CompletionCallback& callback) {
  DCHECK(!callback.is_null());

  if (socket_mode_ == SERVER || !connect_callback_.is_null()) {
    callback.Run(net::ERR_CONNECTION_FAILED);
    return;
  }
  DCHECK(!server_socket_.get());
  socket_mode_ = CLIENT;
  connect_callback_ = callback;

  int result = net::ERR_CONNECTION_FAILED;
  do {
    if (is_connected_)
      break;

    net::AddressList address_list;
    if (!StringAndPortToAddressList(address, port, &address_list)) {
      result = net::ERR_ADDRESS_INVALID;
      break;
    }

    socket_.reset(new net::TCPClientSocket(address_list, NULL,
                                           net::NetLog::Source()));

    connect_callback_ = callback;
    result = socket_->Connect(base::Bind(
        &TCPSocket::OnConnectComplete, base::Unretained(this)));
  } while (false);

  if (result != net::ERR_IO_PENDING)
    OnConnectComplete(result);
}

void TCPSocket::Disconnect() {
  is_connected_ = false;
  if (socket_.get())
    socket_->Disconnect();
  server_socket_.reset(NULL);
}

int TCPSocket::Bind(const std::string& address, int port) {
  return net::ERR_FAILED;
}

void TCPSocket::Read(int count,
                     const ReadCompletionCallback& callback) {
  DCHECK(!callback.is_null());

  if (socket_mode_ != CLIENT) {
    callback.Run(net::ERR_FAILED, NULL);
    return;
  }

  if (!read_callback_.is_null()) {
    callback.Run(net::ERR_IO_PENDING, NULL);
    return;
  } else {
    read_callback_ = callback;
  }

  int result = net::ERR_FAILED;
  scoped_refptr<net::IOBuffer> io_buffer;
  do {
    if (count < 0) {
      result = net::ERR_INVALID_ARGUMENT;
      break;
    }

    if (!socket_.get() || !socket_->IsConnected()) {
        result = net::ERR_SOCKET_NOT_CONNECTED;
        break;
    }

    io_buffer = new net::IOBuffer(count);
    result = socket_->Read(io_buffer.get(), count,
        base::Bind(&TCPSocket::OnReadComplete, base::Unretained(this),
            io_buffer));
  } while (false);

  if (result != net::ERR_IO_PENDING)
    OnReadComplete(io_buffer, result);
}

void TCPSocket::RecvFrom(int count,
                         const RecvFromCompletionCallback& callback) {
  callback.Run(net::ERR_FAILED, NULL, NULL, 0);
}

void TCPSocket::SendTo(scoped_refptr<net::IOBuffer> io_buffer,
                       int byte_count,
                       const std::string& address,
                       int port,
                       const CompletionCallback& callback) {
  callback.Run(net::ERR_FAILED);
}

bool TCPSocket::SetKeepAlive(bool enable, int delay) {
  if (!socket_.get())
    return false;
  return socket_->SetKeepAlive(enable, delay);
}

bool TCPSocket::SetNoDelay(bool no_delay) {
  if (!socket_.get())
    return false;
  return socket_->SetNoDelay(no_delay);
}

int TCPSocket::Listen(const std::string& address, int port, int backlog,
                      std::string* error_msg) {
  if (socket_mode_ == CLIENT) {
    *error_msg = kTCPSocketTypeInvalidError;
    return net::ERR_NOT_IMPLEMENTED;
  }
  DCHECK(!socket_.get());
  socket_mode_ = SERVER;

  scoped_ptr<net::IPEndPoint> bind_address(new net::IPEndPoint());
  if (!StringAndPortToIPEndPoint(address, port, bind_address.get()))
    return net::ERR_INVALID_ARGUMENT;

  if (!server_socket_.get()) {
    server_socket_.reset(new net::TCPServerSocket(NULL,
                                                  net::NetLog::Source()));
    server_socket_->AllowAddressReuse();
  }
  int result = server_socket_->Listen(*bind_address, backlog);
  if (result)
    *error_msg = kSocketListenError;
  return result;
}

void TCPSocket::Accept(const AcceptCompletionCallback &callback) {
  if (socket_mode_ != SERVER || !server_socket_.get()) {
    callback.Run(net::ERR_FAILED, NULL);
    return;
  }

  // Limits to only 1 blocked accept call.
  if (!accept_callback_.is_null()) {
    callback.Run(net::ERR_FAILED, NULL);
    return;
  }

  int result = server_socket_->Accept(&accept_socket_, base::Bind(
      &TCPSocket::OnAccept, base::Unretained(this)));
  if (result == net::ERR_IO_PENDING) {
    accept_callback_ = callback;
  } else if (result == net::OK) {
    accept_callback_ = callback;
    this->OnAccept(result);
  } else {
    callback.Run(result, NULL);
  }
}

bool TCPSocket::GetPeerAddress(net::IPEndPoint* address) {
  if (!socket_.get())
    return false;
  return !socket_->GetPeerAddress(address);
}

bool TCPSocket::GetLocalAddress(net::IPEndPoint* address) {
  if (socket_.get()) {
    return !socket_->GetLocalAddress(address);
  } else if (server_socket_.get()) {
    return !server_socket_->GetLocalAddress(address);
  } else {
    return false;
  }
}

Socket::SocketType TCPSocket::GetSocketType() const {
  return Socket::TYPE_TCP;
}

int TCPSocket::WriteImpl(net::IOBuffer* io_buffer,
                         int io_buffer_size,
                         const net::CompletionCallback& callback) {
//  [TODO:1/6/2014 ken]
//这个函数没有判断套接字是否可写，就直接写，是有问题的。因为底层不判断。==
//所以这个地方要判断是否可写，不可写要缓存，和py的处理逻辑一样===
//调用它的函数判断==
  if (socket_mode_ != CLIENT)
    return net::ERR_FAILED;
  else if (!socket_.get() || !socket_->IsConnected())
    return net::ERR_SOCKET_NOT_CONNECTED;
  else
    return socket_->Write(io_buffer, io_buffer_size, callback);
}

void TCPSocket::OnConnectComplete(int result) {
  DCHECK(!connect_callback_.is_null());
  DCHECK(!is_connected_);
  is_connected_ = result == net::OK;
  CompletionCallback temp = connect_callback_;
  connect_callback_.Reset();
  temp.Run(result);
}

void TCPSocket::SendShakehand(const net::CompletionCallback& callback) {
  if (common_codec_.get()) {
    
    DCHECK(StartsWithASCII(name_, codec::kProtobufCpp, true));
    std::string shakehand = name_;//common_codec_->ShakehandString();
    DCHECK(shakehand.size());
    std::vector<char> out(sizeof(uint32) + shakehand.size());
    *(uint32*)&out[0] = shakehand.size();
    memcpy(&out[sizeof(uint32)], shakehand.c_str(), shakehand.size());
    scoped_refptr<net::IOBufferWithSize> buffer = new net::IOBufferWithSize(out.size());
    memcpy(buffer->data(), out.data(), out.size());

    DCHECK(IsConnected());
    Write(buffer, buffer->size(), callback);
  } else {
    DCHECK(false);
  }
  
}

void TCPSocket::OnReadComplete(scoped_refptr<net::IOBuffer> io_buffer,
                               int result) {
  DCHECK(!read_callback_.is_null());
  //这个地方chrome原生代码有问题，如果在readcomplete里面再read，就设置不上，我就这么干的。。。
  ReadCompletionCallback temp = read_callback_;
  read_callback_.Reset();
  temp.Run(result, io_buffer);
  
}

void TCPSocket::OnAccept(int result) {
  DCHECK(!accept_callback_.is_null());
  AcceptCompletionCallback temp = accept_callback_;
  accept_callback_.Reset();
  if (result == net::OK && accept_socket_.get()) {
    temp.Run(
        result, static_cast<net::TCPClientSocket*>(accept_socket_.release()));
  } else {
    temp.Run(result, NULL);
  }
  
}


//=================================================================
void TCPSocket::SocketAlreadyKnowName(TCPSocket const* p, std::string const &name) {
  DCHECK(this == p);
  DCHECK(name_.size() == 0);
  DCHECK(mode() == CLIENT);
  name_ = name;
  know_socket_name_callback_.Run(p, name);
  know_socket_name_callback_.Reset();
}

}  // namespace extensions
