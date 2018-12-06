// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include <memory>
#include <utility>

#include "channel_host.h"

#include "base/bind.h"
#include "base/string_util.h"
#include "base/string_number_conversions.h"
#include "base/utf_string_conversions.h"

#include "net/base/address_list.h"
#include "net/base/io_buffer.h"
#include "net/base/ip_endpoint.h"
#include "net/base/net_errors.h"
#include "net/socket/socket.h"

#include "../common/common_common.h"
#include "../common/pref_service.h"
#include "../common/common_util.h"




namespace {
  uint32 const kReadBufferSize = 4096;
  //char const* noused = "";

  //char const * kProtobufName = "protobuf://";
  //char const * kTLVName = "tlv://";


  template <typename T>
  class DoNothingDeleter
  {
  public:
    void operator () (T * d) const
    {
      //
    }
  };

  bool ServerName2IpPort(std::string const &name, std::pair<std::string, int> & out) {
    std::string ip_port;
    if (name == prefs::kGuardServer) {
      ip_port = common_util::g_pref_service->GetString(prefs::kGuardServer);
    } else if (name == prefs::kMQServer) {
      ip_port = common_util::g_pref_service->GetString(prefs::kMQServer);
    } else if (name == prefs::kTUServer) {
      ip_port = common_util::g_pref_service->GetString(prefs::kTUServer);
    } else if (name == prefs::kMQTUServer) {
      ip_port = common_util::g_pref_service->GetString(prefs::kMQTUServer);
    } else if (name == prefs::kBacktestingServer) {
      ip_port = common_util::g_pref_service->GetString(prefs::kBacktestingServer);
    }  else if (name == prefs::kCheckUpdateApkList) {
      ip_port = common_util::g_pref_service->GetString(prefs::kCheckUpdateApkList);
    } else if (name == prefs::kAdbServer) {
      ip_port = common_util::g_pref_service->GetString(prefs::kAdbServer);
    } else {
      DCHECK(false);
      return false;
    }

    std::size_t index = ip_port.find(":");
    if (index != std::string::npos) {
      std::string ip = ip_port.substr(0, index);
      std::string port = ip_port.substr(index + 1, ip_port.size());
      int port_number;
      if (base::StringToInt(port, &port_number)) {
        out.first = ip;
        out.second = port_number;
        return true;
      }
    }

    DCHECK(false);
    return false;
  }


}

namespace channel {

  ChannelHost::ChannelHost()
    :last_write_status_(0) {
    // common_codec_(base::Bind(&ChannelHost::SocketAlreadyKnowName, base::Unretained(this))) {
    //   common_codec_.RegisterProtobufHandler(base::Bind(&ChannelHost::CodecCallback, base::Unretained(this)));
    CommonThread::GetCurrentThreadIdentifier(&id_);

    keep_alive_checker_ = [&]() ->void {
      for (auto it = alive_check_list_.begin(); it != alive_check_list_.end(); ++it) {
        if (HasConnection(it->first) == false) {
          InitializeClient(it->first, it->second);
          LOG(WARNING) << "ChannelHost::reconnect " << it->first << " " << it->second;
        }
      }

      CommonThread::PostDelayedTask(id(),
        FROM_HERE,
        base::Bind(&ChannelHost::KeepAlive, base::Unretained(this)),
        1000 * 60 * 1);

    };

  }

  ChannelHost::~ChannelHost() {
    DCHECK_RLOG(CommonThread::CurrentlyOn(id()));
    alive_check_list_.clear();
    // if (alive_check_.get()) {
    //   alive_check_->Cancel();
    //   alive_check_.reset(NULL);
    // }
  }

  void ChannelHost::KeepAlive() {
    keep_alive_checker_();
  }

  ServerResult ChannelHost::InitializeServer(std::string const &name, bool const retry) {
    DCHECK_RLOG(CommonThread::CurrentlyOn(id()));
    name_ = name;
    std::pair<std::string, int> out;
    ServerResult result(false, -1);
    if (!ServerName2IpPort(name_, out)) {
      CHECK(false);
      return result;
    }
    //=============================================
    //20181105，bind容易发生端口被占用的情况。考虑通用性。
    //在给定端口上递增100，如果还不能搞定就放弃
    uint32 counter = 0;
    while (true) {
      listen_socket_.reset(new TCPSocket(TCPSocket::SERVER, name_));
      std::string error;
      int re = listen_socket_->Listen(out.first, out.second, 5, &error);
      if (re == 0) {
        listen_socket_->Accept(base::Bind(&ChannelHost::OnAccept,
          base::Unretained(this)));
        result.first = true;
        result.second = out.second;
        return result;
      } else {
        if (retry) {
          listen_socket_.reset(NULL);
          ++counter;
          ++out.second;
          if (counter > 100) {
            DCHECK(false);
            LOG(WARNING) << "bind socket failed" << name_ << " " << re << " " << out.second;
            return result;
          }
        } else {
          return result;
        }
      }
    }
    
  }

  void ChannelHost::InitializeClient(std::string const &name, std::string const &server_name) {
    DCHECK_RLOG(CommonThread::CurrentlyOn(id()));
    std::pair<std::string, int> out;
    if (!ServerName2IpPort(server_name, out)) {
      CHECK(false);
      return;
    }
    //===========================================
    std::shared_ptr<TCPSocket> p(new TCPSocket(TCPSocket::CLIENT, name));
    if (StartsWithASCII(name, codec::kProtobuf, true)) {
      p->common_codec()->RegisterProtobufHandler(name, base::Bind(&ChannelHost::ProtobufCallback, base::Unretained(this)));
    } else if (StartsWithASCII(name, codec::kTLV, true)) {
      p->common_codec()->RegisterTLVHandler(name, base::Bind(&ChannelHost::TLVCallback, base::Unretained(this)));
    } else {
      DCHECK(false);
    }

    socket_name_map_[p] = name;
    LOG(INFO) << L"name_socket_map_[name] = p.get(); " << ASCIIToWide(name);
    name_socket_map_[name] = p.get();
    p->Connect(out.first, out.second, base::Bind(&ChannelHost::OnConnect, base::Unretained(this), p.get()));
  }

  void ChannelHost::OnWriteComplete(channel::TCPSocket* who, int result_code) {
    DCHECK_RLOG(CommonThread::CurrentlyOn(id()));
    if (result_code > 0 || result_code == net::ERR_IO_PENDING) {

    } else {
      LOG(WARNING) << "ChannelHost::OnWriteComplete ERR " << who->name() << " " << result_code;
      //  [7/20/2016 liuqingping]
      //在交易端主动关闭的情况下，似乎行情端容易走到这个地方，返回值是-101，这种情况，我们先视为正常=
      if (result_code != net::ERR_CONNECTION_RESET) {
        DCHECK_RLOG(false);
      }
      //  [4/21/2014 ken]
      //4月15的日志
      //NET_ERROR(CONNECTION_RESET, -101)
      //这个正确的处理方法是什么呢？？
      //TODO
      //  [7/13/2016 liuqingping]
      //win上感觉比较容易出，出了几次了，
      //ChannelHost::OnWriteComplete ERROR protobuf://cpp/mq -101
      //按标准这个时候就是要断开连接
    }
  }

  void ChannelHost::SocketAlreadyKnowName(TCPSocket const* p, std::string const &name) {
    DCHECK_RLOG(CommonThread::CurrentlyOn(id()));
    std::shared_ptr<TCPSocket > findp(const_cast<TCPSocket*>(p), DoNothingDeleter<TCPSocket >());
    SNMap::iterator it = socket_name_map_.find(findp);
    //findp.release();
    if (it != socket_name_map_.end()) {
      it->second = name;

      LOG(INFO) << L"name_socket_map_[name] = it->first.get(); " << ASCIIToWide(name);
      name_socket_map_[name] = it->first.get();

      //通知=
      auto func_it = connect_map_.find(name);
      if (func_it != connect_map_.end()) {
        func_it->second(name, true);
      }

      if (StartsWithASCII(name, codec::kProtobuf, true)) {
        it->first->common_codec()->RegisterProtobufHandler(name, base::Bind(&ChannelHost::ProtobufCallback, base::Unretained(this)));
      } else if (StartsWithASCII(name, codec::kTLV, true)) {
        it->first->common_codec()->RegisterTLVHandler(name, base::Bind(&ChannelHost::TLVCallback, base::Unretained(this)));
      } else {
        DCHECK(false);
      }

    } else {
      DCHECK(false);
    }
  }

  void ChannelHost::AsyncRead(channel::TCPSocket* who) {
    who->Read(kReadBufferSize,
      base::Bind(&ChannelHost::OnReadComplete,
        base::Unretained(this), who));
  }

  void ChannelHost::OnReadComplete(channel::TCPSocket* who, int result_code,
    scoped_refptr<net::IOBuffer> io_buffer) {
    DCHECK_RLOG(CommonThread::CurrentlyOn(id()));
    //用shared_ptr包装who，但是因为生命周期是用map管理，所有结尾的时候还需要手动释放，放弃管理=
    //这个方法确实挺ugly的=
    std::shared_ptr<channel::TCPSocket> shared_who(who, DoNothingDeleter<TCPSocket>());
    SNMap::iterator it = socket_name_map_.find(shared_who);
    if (it != socket_name_map_.end()) {
      //shared_who.release();

      // [9/26/2013 ken] 
      //出现过一次等于0的。。。=
      //怀疑是tcp建立后，一个数据都没发送，就断开，就是0，而且除了0就没后续了。。。=
      //这不大合理吧。。。。。=
      if (result_code > 0) {
        who->common_codec()->OnMessage(who, io_buffer, result_code, base::Time::Now());
        //  [1/15/2014 ken]
        //如果始终有大量数据到达，比如请求了几万条记录。就会进入一个死循环(debug模式下，再接收数据的地方打断点，让buffer里面充满数据)==
        //TCPSocket::OnReadComplete ---> ChannelHost::OnReadComplete ---> TCPSocket::Read --> TCPSocket::OnReadComplete
        //具体堆栈见：一个tcp的异常，始终有数据到，堆栈溢出.txt
        //为了避免死循环，最简单的方法，把这个read直接设置成异步的===
        CommonThread::PostTask(id(),
          FROM_HERE,
          base::Bind(&ChannelHost::AsyncRead, base::Unretained(this), who));
        /*who->Read(kReadBufferSize,
          base::Bind(&ChannelHost::OnReadComplete,
            base::Unretained(this), who));*/

      } else if (result_code == net::ERR_IO_PENDING) {
        //DCHECK(false);
      } else if (result_code == net::ERR_CONNECTION_RESET || result_code == 0) {
        //  [9/7/2016 liuqingping]
        //看错误码的意思0是不应该算作错误的，这种情况下remove感觉是不对的。。。。
        //通知=
        auto func_it = connect_map_.find(who->name());
        if (func_it != connect_map_.end()) {
          func_it->second(who->name(), false);
        }

        std::string name = it->second;
        DCHECK_RLOG(who->name() == name);

        socket_name_map_.erase(it);
        NSMap::iterator nsit = name_socket_map_.find(name);
        if (nsit != name_socket_map_.end()) {
          LOG(INFO) << L"name_socket_map_.erase(nsit); " << ASCIIToWide(name) << L" " << result_code;
          name_socket_map_.erase(nsit);
        }

        //  [3/10/2014 ken] 重连放到最后，而且5分钟检查一次==
    //2018-10-31，这里没必要检查，name_socket_map_移除后，自动就应该触发重连
        //keep_alive_checker_(name, false);

      } else {
        LOG(WARNING) << L"error code " << ASCIIToWide(who->name()) << L" " << result_code;
        DCHECK_RLOG(false);
      }

    } else {
      LOG(WARNING) << L"not find " << ASCIIToWide(who->name());
      DCHECK_RLOG(false);
      //shared_who.release();
    }
  }

  void ChannelHost::OnAccept(int, net::TCPClientSocket* ps) {
    DCHECK_RLOG(CommonThread::CurrentlyOn(id()));
    //DCHECK(python_socket_.get() == NULL);
    std::shared_ptr<TCPSocket> p(new TCPSocket(ps, base::Bind(&ChannelHost::SocketAlreadyKnowName, base::Unretained(this))));
    //p->common_codec()->RegisterProtobufHandler(base::Bind(&ChannelHost::ProtobufCallback, base::Unretained(this)));
    //p->common_codec()->RegisterTLVHandler(base::Bind(&ChannelHost::TLVCallback, base::Unretained(this)));
    socket_name_map_[p] = "";
    p->Read(kReadBufferSize,
      base::Bind(&ChannelHost::OnReadComplete,
        base::Unretained(this), p.get()));

    listen_socket_->Accept(base::Bind(&ChannelHost::OnAccept,
      base::Unretained(this)));
  }

  void ChannelHost::OnConnect(channel::TCPSocket* who, int re) {
    DCHECK_RLOG(CommonThread::CurrentlyOn(id()));
    if (re == net::OK) {
      LOG(INFO) << "ChannelHost::OnConnect failed " << who->name();
      who->SendShakehand(base::Bind(&ChannelHost::OnWriteComplete, base::Unretained(this), who));

      //通知=
      auto it = connect_map_.find(who->name());
      if (it != connect_map_.end()) {
        it->second(who->name(), true);
      }

      who->Read(kReadBufferSize,
        base::Bind(&ChannelHost::OnReadComplete,
          base::Unretained(this), who));
    } else {
      DCHECK(false);
      LOG(WARNING) << "ChannelHost::OnConnect failed " << who->name() << "re= " << re;

      std::string name = who->name();
      std::shared_ptr<channel::TCPSocket> shared_who(who, DoNothingDeleter<TCPSocket>());
      socket_name_map_.erase(shared_who);
      name_socket_map_.erase(name);
    }
  }

  void ChannelHost::ProtobufCallback(std::string const& name, codec::MessagePtr const& p, base::Time const& t) {
    DCHECK_RLOG(CommonThread::CurrentlyOn(id()));
    auto it = protobuf_callback_map_.find(name);
    if (it != protobuf_callback_map_.end()) {
      if (!it->second.is_null()) {
        it->second.Run(name, p, t);
      }
    }

  }

  void ChannelHost::TLVCallback(std::string const& name, codec::TLVMessagePtr const& p, base::Time const& t) {
    DCHECK_RLOG(CommonThread::CurrentlyOn(id()));
    auto it = tlv_callback_map_.find(name);
    if (it != tlv_callback_map_.end()) {
      if (!it->second.is_null()) {
        it->second.Run(name, p, t);
      }
    }
  }

  bool ChannelHost::HasConnection(std::string const &name) {
    DCHECK_RLOG(CommonThread::CurrentlyOn(id()));
    auto it = name_socket_map_.find(name);
    if (it != name_socket_map_.end()) {
      //LOG(INFO)<<L"it != name_socket_map_.end() "<<ASCIIToWide(name);
      return true;
    }

    return false;
  }

  void ChannelHost::SendProtobufMsg(std::string const& name, codec::MessagePtr const & p) {
    DCHECK_RLOG(CommonThread::CurrentlyOn(id()));
    LOG(INFO) << name<<" "<<p->GetTypeName();

    auto it = name_socket_map_.find(name);
    if (it != name_socket_map_.end()) {
      DCHECK(it->second->IsConnected());
      if (it->second->IsConnected()) {
        std::vector<char> out;
        codec::ProtobufCodec::FillEmptyBuffer(*p.get(), &out);
        scoped_refptr<net::IOBufferWithSize> buffer = new net::IOBufferWithSize(out.size());
        memcpy(buffer->data(), out.data(), out.size());

        if (last_write_status_ < 0) {
          LOG(ERROR) << "ChannelHost::SendProtobufMsg fatal error!! " << last_write_status_;
        }
        //int result = 
        it->second->Write(buffer, buffer->size(), base::Bind(&ChannelHost::OnWriteComplete, base::Unretained(this), it->second));
        //last_write_status_ = result;
/*
        if (result > 0 || result == net::ERR_IO_PENDING) {
          //OnWriteComplete(result);

        } else {
          LOG(ERROR)<<"it->second->WriteImpl "<<result;
          DCHECK_RLOG(false);
        }*/
      } else {
        DCHECK_RLOG(false);
      }
    } else {
      DCHECK_RLOG(false);
    }

  }


  void ChannelHost::SendTLVMsg(std::string const& name, codec::TLVMessagePtr const & p) {
    DCHECK_RLOG(CommonThread::CurrentlyOn(id()));
    NSMap::iterator it = name_socket_map_.find(name);
    if (it != name_socket_map_.end()) {
      DCHECK(it->second->IsConnected());
      if (it->second->IsConnected()) {
        codec::TransferBuffer out;
        codec::TLVCodec::FillEmptyBuffer(*p.get(), &out);

        //数据总长度+单元个数，至少8个字节==
        DCHECK_RLOG(out.buffer().size() > 8);
        if (out.buffer().size() > 8) {
          uint32 one = *((uint32*)&out.buffer()[0]);
          uint32 two = *((uint32*)&out.buffer()[4]);
          //LOG(INFO)<<L"ChannelHost::SendTLVMsg total "<<one<<L"number"<<two;
        }


        scoped_refptr<net::IOBufferWithSize> buffer = new net::IOBufferWithSize(out.buffer().size());
        memcpy(buffer->data(), &out.buffer()[0], out.buffer().size());


        //int result = 
        it->second->Write(buffer, buffer->size(), base::Bind(&ChannelHost::OnWriteComplete, base::Unretained(this), it->second));
        /*
                if (result > 0 || result == net::ERR_IO_PENDING) {
                  //OnWriteComplete(result);
                  if (result > 0) {
                    DCHECK_RLOG(result == buffer->size());
                    if (result != buffer->size()) {
                      LOG(ERROR)<<L"it->second->WriteImpl "<<result<<L" "<<buffer->size();
                    }
                  } else {
                    LOG(ERROR)<<L"it->second->WriteImpl ERR_IO_PENDING";
                  }

                } else {
                  DCHECK(false);
                }*/
      } else {
        DCHECK(false);
      }
    } else {
      //DCHECK(false);
    }
  }


  void ChannelHost::RegisterProtobufHandler(std::string const & name, const codec::ProtobufMessageCallback& messageCb) {
    DCHECK_RLOG(CommonThread::CurrentlyOn(id()));
    //DCHECK(protobuf_callback_.is_null());
    protobuf_callback_map_[name] = messageCb;
  }

  void ChannelHost::RegisterTLVHandler(std::string const & name, const codec::TLVMessageCallback& messageCb) {
    //DCHECK(tlv_callback_.is_null());
    tlv_callback_map_[name] = messageCb;
  }

  void ChannelHost::StartKeepAlive(std::string const &name, std::string const &target_name) {
    DCHECK_RLOG(CommonThread::CurrentlyOn(id()));

    alive_check_list_[name] = target_name;
    bool alive = HasConnection(name);
    if (alive != true) {
      InitializeClient(name, target_name);
      LOG(INFO) << "ChannelHost::StartKeepAlive " << name << " " << target_name;
    }
  }

  void ChannelHost::StopKeepAlive(std::string const &name) {
    DCHECK_RLOG(CommonThread::CurrentlyOn(id()));
    auto it = alive_check_list_.find(name);
    if (it != alive_check_list_.end()) {
      alive_check_list_.erase(it);
    }
  }




  //   alive_check_.reset(
  //     new VoidVoidCallback(
  //     base::Bind(&ChannelHost::CheckAlive, base::Unretained(this))));
  //   CommonThread::PostDelayedTask(id(),
  //     FROM_HERE,
  //     alive_check_->callback(),
  //     5 * 1000);
  // }


  void ChannelHost::RegisterConnectStatusHandler(std::string const &name, std::function<void(std::string const &, bool)> const &func) {
    connect_map_[name] = func;
  }


}  // namespace extensions
