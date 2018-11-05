#pragma once

#include <string>
#include <vector>
#include <map>
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
#include "base/sequenced_task_runner_helpers.h"
#include "ipc/ipc_sync_channel.h"
#include "content/public/browser/notification_observer.h"
#include "content/public/browser/notification_registrar.h"
#include "content/public/browser/browser_thread.h"

#include "../include/ctp_define.h"
#include "thread_message_filter.h"

class CommandLine;
class MessageLoop;
class IPC::Message;
class Profile;

class PHONE_COMMON_EXPORT CommonIPCServer
    : public base::Thread,
      public IPC::Listener,
      public IPC::Sender,
      public base::RefCountedThreadSafe<CommonIPCServer, 
          content::BrowserThread::DeleteOnUIThread>,
      public content::NotificationObserver {

public:
  static std::string StartCommonIPCServer(CommonIPCServer * & server);
  CommonIPCServer();
  ~CommonIPCServer();
  
  std::string channel_name();
  
  bool connected();

  void WaitForIPCServer_Startup();
  
  Profile* profile();

  // content::NotificationObserver
  virtual void Observe(int type,
                       const content::NotificationSource& source,
                       const content::NotificationDetails& details) OVERRIDE;

  void AddMessageFilter(ThreadMessageFilter* tf) {
    message_filters_.push_back(tf);
  }
  void ClearMessageFilter() {
    message_filters_.clear();
  }

private:
  void get_channel_name();


  void TrySuicide();
private:
   // PlatformThread::Delegate methods:
  void Init() OVERRIDE;
  void CleanUp() OVERRIDE;
  // IPC::Listener
  void OnChannelConnected(int32 peer_pid) OVERRIDE;
  void OnChannelError() OVERRIDE;
  bool OnMessageReceived(const IPC::Message& message) OVERRIDE;
  // IPC::Sender
  bool Send(IPC::Message* msg) OVERRIDE;
private:
  scoped_ptr<IPC::ChannelProxy> channel_;
  HANDLE event_startup_ipc_;
  std::string channel_name_;
  long volatile connected_;
  content::NotificationRegistrar registrar_;


  std::vector<ThreadMessageFilter*> message_filters_;



  //message
  void OnHello();
};