#pragma once
#include <set>


#include "base/callback.h"
#include "base/synchronization/lock.h"
#include "base/bind.h"
#include "base/threading/thread.h"
#include "base/threading/non_thread_safe.h"
#include "base/memory/scoped_ptr.h"
#include "ipc/ipc_message.h"


#include "../include/ctp_define.h"
#include "common_thread.h"
#include "common_util.h"



class PHONE_COMMON_EXPORT ThreadMessageFilter : public base::NonThreadSafe{
public:
  ThreadMessageFilter(bool const);
  virtual ~ThreadMessageFilter();
  virtual bool OnMessageReceived(IPC::Message const & message) { return false; }
private:
  bool directly_listen_;
};

class PHONE_COMMON_EXPORT ThreadMessageDispatcherImpl : public base::NonThreadSafe {
public:
  static ThreadMessageDispatcherImpl* current();
  static void DispatchHelper(CommonThread::ID const, IPC::Message const* message);
  static ThreadMessageDispatcherImpl* Create();
  
  virtual ~ThreadMessageDispatcherImpl();

  // NotificationService:
  bool Dispatch(IPC::Message const* message);

  void AddObserver(ThreadMessageFilter* observer);

  void RemoveObserver(ThreadMessageFilter* observer);

  void OpenPerformanceCheck();

  
  
private:
  ThreadMessageDispatcherImpl();

private:
  std::set<ThreadMessageFilter*> observer_list_;
  scoped_ptr<common_util::PerformanceThreadCheck> performance_check_;

  DISALLOW_COPY_AND_ASSIGN(ThreadMessageDispatcherImpl);
};
