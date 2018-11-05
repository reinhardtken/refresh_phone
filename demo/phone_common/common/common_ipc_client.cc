

#include "common_ipc_client.h"

#include <windows.h>
#include <tchar.h>
#include <algorithm>


#include "base/at_exit.h"
#include "base/command_line.h"
#include "base/message_loop.h"
#include "base/memory/scoped_ptr.h"
#include "base/synchronization/waitable_event.h"
#include "base/time.h"
#include "base/threading/thread_restrictions.h"
#include "base/threading/thread.h"
#include "ipc/ipc_sync_channel.h"
#include "ipc/ipc_message.h"
#include "base/utf_string_conversions.h"



#include "../include/ctp_messages.h"




CommonIPCClient::CommonIPCClient(const std::string& name)
    : shutdown_event_(true, false),
      channel_name_(name),
      Thread("IPCClient") {
}

CommonIPCClient::~CommonIPCClient() {
}

void CommonIPCClient::AsynGetFlashCookies() {
  
}

void CommonIPCClient::OnChannelError() {
  //QuitApplication();
}
  
void CommonIPCClient::Init() {
    channel_.reset(new IPC::ChannelProxy(
          channel_name_, IPC::Channel::MODE_CLIENT, this,
          this->message_loop_proxy()));
}

void CommonIPCClient::OnChannelConnected(int32 peer_pid) {
  Send(new CTP_CONSOLE_Hello);
}

void CommonIPCClient::CleanUp() {
  channel_->ClearIPCTaskRunner();
}

bool CommonIPCClient::OnMessageReceived(const IPC::Message& msg) {

  //优先给filter处理=
  //dispatch
  for (size_t i = 0; i < message_filters_.size(); ++i) {
    if (message_filters_[i]->OnMessageReceived(msg)) {
      return true;
    }
  }


  if (msg.routing_id() == MSG_ROUTING_CONTROL) {    
    bool msg_is_ok = false;
    IPC_BEGIN_MESSAGE_MAP_EX(CommonIPCClient, msg, msg_is_ok)
      IPC_MESSAGE_HANDLER(CTP_CONSOLE_HelloAck, OnHelloAck)
//       IPC_MESSAGE_HANDLER(CookieIpcMsg_ClrFlashCookiesAck, OnClrFlashCookies_ACK)
//       IPC_MESSAGE_HANDLER(CookieIpcMsg_GetAllWebsiteCookiesAck, OnGetWebsiteCookies_ACK)
//       IPC_MESSAGE_HANDLER(CookieIpcMsg_ClrWebsiteCookiesAck, OnClrWebsiteCookies_ACK)
//       IPC_MESSAGE_HANDLER(CookieIpcMsg_GetCrosssiteCookiesAck, OnGetCrosssiteCookies_ACK)
//       IPC_MESSAGE_HANDLER(CookieIpcMsg_ClrCrosssiteCookiesAck, OnClrCrosssiteCookies_ACK)
      //IPC_MESSAGE_UNHANDLED_ERROR()
    IPC_END_MESSAGE_MAP_EX()

    if (msg_is_ok) {
      return true;
    }
  }
  return false;
 }



bool CommonIPCClient::Send(IPC::Message* msg) {
  return channel_->Send(msg);
}


void CommonIPCClient::OnHelloAck() {

}