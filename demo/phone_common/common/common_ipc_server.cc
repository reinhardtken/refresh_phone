#include "common_ipc_server.h"

#include <windows.h>
#include <tchar.h>
#include <algorithm>

#include "base/command_line.h"
#include "base/message_loop.h"
#include "base/memory/scoped_ptr.h"
#include "base/synchronization/waitable_event.h"
#include "base/time.h"
#include "base/threading/thread_restrictions.h"
#include "base/threading/thread.h"
#include "ipc/ipc_sync_channel.h"
#include "ipc/ipc_message.h"
#include "ipc/ipc_channel.h"
#include "chrome/common/chrome_notification_types.h"

#include "content/public/browser/notification_details.h"
#include "content/public/browser/notification_source.h"

#include "../include/ctp_messages.h"
#include "common_thread.h"

//#ifdef _DEBUG
#define IPC_SERVER_TESTING
//#endif

const std::string kCookieIpcChannelBaseName = "CommonChannel";



CommonIPCServer::CommonIPCServer()
    : Thread("IPCServer"),
      event_startup_ipc_(NULL),
      connected_(0) {

  DCHECK(CommonThread::CurrentlyOn(CommonThread::UI));

  get_channel_name();

  event_startup_ipc_ = CreateEvent(NULL, TRUE, FALSE, NULL);

}

CommonIPCServer::~CommonIPCServer() {
}

std::string CommonIPCServer::channel_name() {
  return channel_name_;
}

void CommonIPCServer::get_channel_name() {
#ifdef IPC_SERVER_TESTING
  channel_name_ = kCookieIpcChannelBaseName;
#else
  channel_name_ = IPC::Channel::GenerateVerifiedChannelID(
      kCookieIpcChannelBaseName);
#endif
}

void CommonIPCServer::Observe(int type,
    const content::NotificationSource& source,
    const content::NotificationDetails& details) {
  DCHECK(CommonThread::CurrentlyOn(CommonThread::UI));
}

void CommonIPCServer::Init() {
  DCHECK(!CommonThread::CurrentlyOn(CommonThread::UI));
  channel_.reset(new IPC::ChannelProxy(
      channel_name_, IPC::Channel::MODE_SERVER, this,
      this->message_loop_proxy()));
  SetEvent(event_startup_ipc_);
 }

void CommonIPCServer::WaitForIPCServer_Startup() {
  DCHECK(CommonThread::CurrentlyOn(CommonThread::UI));
  DWORD wait = WaitForSingleObject(event_startup_ipc_, 3000);
  CHECK(wait == WAIT_OBJECT_0);
  CloseHandle(event_startup_ipc_);
  event_startup_ipc_ = NULL;
}

void CommonIPCServer::CleanUp() {
  DCHECK(!CommonThread::CurrentlyOn(CommonThread::UI));
  channel_->ClearIPCTaskRunner();
}

bool CommonIPCServer::OnMessageReceived(const IPC::Message& msg) {
  DCHECK(!CommonThread::CurrentlyOn(CommonThread::UI));

  //优先给filter处理=
  //dispatch
  for (size_t i = 0; i < message_filters_.size(); ++i) {
    if (message_filters_[i]->OnMessageReceived(msg)) {
      return true;
    }
  }


  if (msg.routing_id() == MSG_ROUTING_CONTROL) {
    bool msg_is_ok = false;
    IPC_BEGIN_MESSAGE_MAP_EX(CommonIPCServer, msg, msg_is_ok)
      IPC_MESSAGE_HANDLER(CTP_CONSOLE_Hello, OnHello)
      //IPC_MESSAGE_UNHANDLED_ERROR()
    IPC_END_MESSAGE_MAP_EX()

      if (msg_is_ok) {
        return true;
      }
  }
  return false;
}

void CommonIPCServer::OnHello() {
  Send(new CTP_CONSOLE_HelloAck);
}

bool CommonIPCServer::Send(IPC::Message* msg) {
  return channel_->Send(msg);
}

void CommonIPCServer::OnChannelError() {
  DCHECK(!CommonThread::CurrentlyOn(CommonThread::UI));
#ifdef IPC_SERVER_TESTING
  channel_->ClearIPCTaskRunner();
  channel_.reset(new IPC::ChannelProxy(
    channel_name_, IPC::Channel::MODE_SERVER, this,
      this->message_loop_proxy()));

#else

  long old_value = InterlockedCompareExchange(&connected_, 0, 1);
  DCHECK(old_value == 1);

  content::CommonThread::PostTask(
      content::CommonThread::UI,
      FROM_HERE,
      base::Bind(&CommonIPCServer::TrySuicide, this));
#endif
}

void CommonIPCServer::OnChannelConnected(int32 peer_pid) {
#ifndef IPC_SERVER_TESTING
  long old_value = InterlockedCompareExchange(&connected_, 1, 0);
  DCHECK(old_value == 0);
#endif
}

bool CommonIPCServer::connected() {  
  long old_value = InterlockedCompareExchange(&connected_, 0, 0);
  return old_value? true:false;
}

void CommonIPCServer::TrySuicide() {
  DCHECK(CommonThread::CurrentlyOn(CommonThread::UI));
}


std::string CommonIPCServer::StartCommonIPCServer(CommonIPCServer * & server) {
  DCHECK(CommonThread::CurrentlyOn(CommonThread::UI));

  server = new CommonIPCServer();
  server->StartWithOptions(
    base::Thread::Options(MessageLoop::TYPE_IO, 0));
  server->WaitForIPCServer_Startup();

  // post a delayed task to check if the child process really start
  //DCHECK(server.IsCancelled());
/*
  cookie_ipc_server_checker_.Reset(base::Bind(
    &ProfileImpl::CheckCookieIpcServer, base::Unretained(this)));*/
/*
#ifndef IPC_SERVER_TESTING
  CommonThread::PostDelayedTask(
    CommonThread::UI, FROM_HERE,
    cookie_ipc_server_checker_.callback(),
    base::TimeDelta::FromMilliseconds(10*1000));
#endif*/

  return server->channel_name();
}