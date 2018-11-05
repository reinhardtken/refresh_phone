#pragma once

#include <string>
#include <vector>
#include "base/basictypes.h"
#include "base/memory/scoped_ptr.h"
#include "base/at_exit.h"
#include "base/command_line.h"
#include "base/message_loop.h"
#include "base/memory/scoped_ptr.h"
#include "base/synchronization/waitable_event.h"
#include "base/time.h"
#include "base/threading/thread_restrictions.h"
#include "base/threading/thread.h"
#include "ipc/ipc_sync_channel.h"
//#include "ipc/ipc_message.h"

#include "../include/ctp_define.h"
#include "thread_message_filter.h"

class CommandLine;
class MessageLoop;
class IPC::Message;

class PHONE_COMMON_EXPORT CommonIPCClient : 
    public base::Thread, 
    public IPC::Listener, 
    public IPC::Sender,
    public base::RefCountedThreadSafe<CommonIPCClient> {
public:
  CommonIPCClient(const std::string& name);
  ~CommonIPCClient();

  virtual bool Send(IPC::Message* msg) OVERRIDE;
  virtual void OnChannelError() OVERRIDE;
  virtual void OnChannelConnected(int32 peer_pid) OVERRIDE;

  void AddMessageFilter(ThreadMessageFilter* tf) {
    message_filters_.push_back(tf);
  }
  void ClearMessageFilter() {
    message_filters_.clear();
  }


  void AsynGetFlashCookies();
  

  //virtual void Run(MessageLoop* message_loop) OVERRIDE;
private:
   // PlatformThread::Delegate methods:
  virtual void Init() OVERRIDE;
  virtual void CleanUp() OVERRIDE;

  virtual bool OnMessageReceived(const IPC::Message& message) OVERRIDE;

  scoped_ptr<IPC::ChannelProxy> channel_;
  base::WaitableEvent shutdown_event_;
  std::string channel_name_;

  std::vector<ThreadMessageFilter*> message_filters_;
  


  //message
  void OnHelloAck();
};

typedef scoped_refptr<CommonIPCClient> CommonIPCClientPtr;

