// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "common_thread.h"

#include "base/time.h"
#include "base/message_loop.h"
#include "base/message_loop_proxy_impl.h"
#include "base/threading/thread_restrictions.h"

// Friendly names for the well-known threads.
static const char* common_thread_names[CommonThread::ID_COUNT] = {
  "",  // UI (name assembled in common_main.cc).
  "Common_IOThread",  // IO
  "Common_DBThread",  // DB
  "Common_FileThread",  // FILE
  "Common_ProcessLauncherThread",  // PROCESS_LAUNCHER
  "Common_CTPThread", //CTP
  "Common_OMQThread", //omq
};

// An implementation of MessageLoopProxy to be used in conjunction
// with BrowserThread.
class CommonThreadMessageLoopProxy : public base::MessageLoopProxyImpl {
public:
  explicit CommonThreadMessageLoopProxy(CommonThread::ID identifier)
    : id_(identifier) {
  }
  virtual bool PostTask(const tracked_objects::Location& from_here,
    const base::Closure& task) {
      return CommonThread::PostTask(id_, from_here, task);
  }

  virtual bool PostDelayedTask(const tracked_objects::Location& from_here,
    const base::Closure& task, int64 delay_ms) {
      return CommonThread::PostDelayedTask(id_, from_here, task, delay_ms);
  }

  virtual bool PostNonNestableTask(const tracked_objects::Location& from_here,
    const base::Closure& task) {
      return CommonThread::PostNonNestableTask(id_, from_here, task);
  }

  virtual bool PostNonNestableDelayedTask(
    const tracked_objects::Location& from_here,
    const base::Closure& task,
    int64 delay_ms) {
      return CommonThread::PostNonNestableDelayedTask(id_, from_here, task,
        delay_ms);
  }

  virtual bool BelongsToCurrentThread() {
    return CommonThread::CurrentlyOn(id_);
  }

private:
  CommonThread::ID id_;
  DISALLOW_COPY_AND_ASSIGN(CommonThreadMessageLoopProxy);
};

base::Lock CommonThread::lock_;

CommonThread* CommonThread::common_threads_[ID_COUNT];

CommonThread::CommonThread(CommonThread::ID identifier)
  : Thread(common_thread_names[identifier]),
  identifier_(identifier) {
    Initialize();
}

CommonThread::CommonThread(ID identifier, MessageLoop* message_loop)
  : Thread(message_loop->thread_name().c_str()),
  identifier_(identifier) {
    set_message_loop(message_loop);
    Initialize();
}

void CommonThread::Initialize() {
  base::AutoLock lock(lock_);
  DCHECK(identifier_ >= 0 && identifier_ < ID_COUNT);
  DCHECK(common_threads_[identifier_] == NULL);
  common_threads_[identifier_] = this;
}

CommonThread::~CommonThread() {
  // Stop the thread here, instead of the parent's class destructor.  This is so
  // that if there are pending tasks that run, code that checks that it's on the
  // correct CommonThread succeeds.
  Stop();

  base::AutoLock lock(lock_);
  common_threads_[identifier_] = NULL;
#ifndef NDEBUG
  // Double check that the threads are ordered correctly in the enumeration.
  for (int i = identifier_ + 1; i < ID_COUNT; ++i) {
    DCHECK(!common_threads_[i]) <<
      "Threads must be listed in the reverse order that they die";
  }
#endif
}

// static
bool CommonThread::IsWellKnownThread(ID identifier) {
  base::AutoLock lock(lock_);
  return (identifier >= 0 && identifier < ID_COUNT &&
    common_threads_[identifier]);
}

// static
bool CommonThread::CurrentlyOn(ID identifier) {
  // We shouldn't use MessageLoop::current() since it uses LazyInstance which
  // may be deleted by ~AtExitManager when a WorkerPool thread calls this
  // function.
  // http://crbug.com/63678
  base::ThreadRestrictions::ScopedAllowSingleton allow_singleton;
  base::AutoLock lock(lock_);
  DCHECK(identifier >= 0 && identifier < ID_COUNT);
  return common_threads_[identifier] &&
    common_threads_[identifier]->message_loop() == MessageLoop::current();
}

// static
bool CommonThread::IsMessageLoopValid(ID identifier) {
  base::AutoLock lock(lock_);
  DCHECK(identifier >= 0 && identifier < ID_COUNT);
  return common_threads_[identifier] &&
    common_threads_[identifier]->message_loop();
}

// static
bool CommonThread::PostTask(ID identifier,
  const tracked_objects::Location& from_here,
  const base::Closure& task) {
    return PostTaskHelper(identifier, from_here, task, 0, true);
}

// static
bool CommonThread::PostDelayedTask(ID identifier,
  const tracked_objects::Location& from_here,
  const base::Closure& task,
  int64 delay_ms) {
    return PostTaskHelper(identifier, from_here, task, delay_ms, true);
}

// static
bool CommonThread::PostNonNestableTask(
  ID identifier,
  const tracked_objects::Location& from_here,
  const base::Closure& task) {
    return PostTaskHelper(identifier, from_here, task, 0, false);
}

// static
bool CommonThread::PostNonNestableDelayedTask(
  ID identifier,
  const tracked_objects::Location& from_here,
  const base::Closure& task,
  int64 delay_ms) {
    return PostTaskHelper(identifier, from_here, task, delay_ms, false);
}

// static
bool CommonThread::GetCurrentThreadIdentifier(ID* identifier) {
  // We shouldn't use MessageLoop::current() since it uses LazyInstance which
  // may be deleted by ~AtExitManager when a WorkerPool thread calls this
  // function.
  // http://crbug.com/63678
  base::ThreadRestrictions::ScopedAllowSingleton allow_singleton;
  MessageLoop* cur_message_loop = MessageLoop::current();
  for (int i = 0; i < ID_COUNT; ++i) {
    if (common_threads_[i] &&
      common_threads_[i]->message_loop() == cur_message_loop) {
        *identifier = common_threads_[i]->identifier_;
        return true;
    }
  }

  return false;
}

// static
scoped_refptr<base::MessageLoopProxy>
  CommonThread::GetMessageLoopProxyForThread(
  ID identifier) {
    scoped_refptr<base::MessageLoopProxy> proxy(
      new CommonThreadMessageLoopProxy(identifier));
    return proxy;
}

// static
bool CommonThread::PostTaskHelper(
  ID identifier,
  const tracked_objects::Location& from_here,
  const base::Closure& task,
  int64 delay_ms,
  bool nestable) {
    DCHECK(identifier >= 0 && identifier < ID_COUNT);
    // Optimization: to avoid unnecessary locks, we listed the ID enumeration in
    // order of lifetime.  So no need to lock if we know that the other thread
    // outlives this one.
    // Note: since the array is so small, ok to loop instead of creating a map,
    // which would require a lock because std::map isn't thread safe, defeating
    // the whole purpose of this optimization.
    ID current_thread;
    bool guaranteed_to_outlive_target_thread =
      GetCurrentThreadIdentifier(&current_thread) &&
      current_thread >= identifier;

    if (!guaranteed_to_outlive_target_thread)
      lock_.Acquire();

    MessageLoop* message_loop = common_threads_[identifier] ?
      common_threads_[identifier]->message_loop() : NULL;
    if (message_loop) {
      if (nestable) {
        message_loop->PostDelayedTask(from_here, task, base::TimeDelta::FromMilliseconds(delay_ms));
      } else {
        message_loop->PostNonNestableDelayedTask(from_here, task, base::TimeDelta::FromMilliseconds(delay_ms));
      }
    }

    if (!guaranteed_to_outlive_target_thread)
      lock_.Release();

    return !!message_loop;
}
