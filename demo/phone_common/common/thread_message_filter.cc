
#include "thread_message_filter.h"

#include "base/lazy_instance.h"
#include "base/threading/thread_local.h"
#include "base/stringprintf.h"

#include "common_util.h"
#include "../include/ctp_notification_types.h"
#include "../include/ctp_define.h"
#include "../include/ctp_messages.h"

namespace {

  base::LazyInstance<base::ThreadLocalPointer<ThreadMessageDispatcherImpl> >
    lazy_tls_ptr = LAZY_INSTANCE_INITIALIZER;

}  // namespace


extern char const* MessageType2String(uint32 const type);

// static
ThreadMessageDispatcherImpl* ThreadMessageDispatcherImpl::current() {
  return lazy_tls_ptr.Pointer()->Get();
}



// static
ThreadMessageDispatcherImpl* ThreadMessageDispatcherImpl::Create() {
  return new ThreadMessageDispatcherImpl;
}



ThreadMessageDispatcherImpl::ThreadMessageDispatcherImpl() {
  DCHECK(current() == NULL);
  //DCHECK(CalledOnValidThread());
  lazy_tls_ptr.Pointer()->Set(this);
}

void ThreadMessageDispatcherImpl::OpenPerformanceCheck() {
  DCHECK(CalledOnValidThread());

  performance_check_.reset(common_util::PerformanceThreadCheck::Create());
}

void ThreadMessageDispatcherImpl::AddObserver(ThreadMessageFilter* observer) {
    DCHECK(CalledOnValidThread());
    DCHECK(observer);

    if (observer_list_.find(observer) == observer_list_.end()) {
      observer_list_.insert(observer);
    } else {
      DCHECK(false);
    }
}

void ThreadMessageDispatcherImpl::RemoveObserver(ThreadMessageFilter* observer) {
  DCHECK(CalledOnValidThread());
  DCHECK(observer);

  if (observer_list_.find(observer) != observer_list_.end()) {
    observer_list_.erase(observer);
  } else {
    DCHECK(false);
  }
}

bool ThreadMessageDispatcherImpl::Dispatch(IPC::Message const* message) {
  bool re = false;
  DCHECK(CalledOnValidThread());

  scoped_ptr<IPC::Message const> m(message);
  //performance
  {
    PERFORMANCE_MACOR;
    std::set<ThreadMessageFilter*>::iterator it = observer_list_.begin();
    for (; it != observer_list_.end(); ++it) {
      re = (*it)->OnMessageReceived(*message);
      if (re == true) {
        break;
      }
    }
  }

    
  //performance start
  if (common_util::PerformanceThreadCheck::current()) {
    base::TimeDelta delta = common_util::PerformanceThreadCheck::current()->CalculateCosts();
    if (delta.ToInternalValue() > 1000 *100) {
      std::string s = base::StringPrintf("[id] = %s :%s", MessageType2String(message->type()), common_util::PerformanceThreadCheck::current()->Dump().c_str());
      common_util::NotifyMessageFunction(phone_module::UIERROR, phone_module::NOTIFICATION_CTP_PERFORMANCE_MEMORY_DB_SAVE_MQ, s);
    }

    if (delta.ToInternalValue() > 1000 * 40) {
      //DCHECK(false);
      LOG(ERROR)<<__FILE__<<"|"<<delta.ToInternalValue()<<"|"<<message->type()<<" ";//<<common_util::MsgType2String(message->type()).c_str();
    }
    common_util::PerformanceThreadCheck::current()->Clear();
  }
  //performance end


  if (re == false && message->type() != CTP_MessageBroadcast::ID) {
    DCHECK_RLOG(false);
    LOG(ERROR)<<__FILE__<<"|"<<message->type()<<" ";//<<common_util::MsgType2String(message->type()).c_str();
  }
  return re;
  

  
}


void ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::ID const id, IPC::Message const* message) {
  if (CommonThread::CurrentlyOn(id)) {
    ThreadMessageDispatcherImpl::current()->Dispatch(message);
  } else {
    //我觉得这样有问题，我想用scoped_ptr，不过出问题也是线程退出，相当于程序退出，先不管了=
    CommonThread::PostTask(id,
      FROM_HERE,
      base::Bind(&ThreadMessageDispatcherImpl::DispatchHelper, id, base::Unretained(message)));
  }
}

ThreadMessageDispatcherImpl::~ThreadMessageDispatcherImpl() {
  DCHECK(CalledOnValidThread());

  lazy_tls_ptr.Pointer()->Set(NULL);
}



//ThreadMessageFilter//////////////////////////////////////////////////
ThreadMessageFilter::ThreadMessageFilter(bool const dl):
  directly_listen_(dl) {
  if (directly_listen_) {
    ThreadMessageDispatcherImpl::current()->AddObserver(this);
  }
}

ThreadMessageFilter::~ThreadMessageFilter() {
  if (directly_listen_) {
    ThreadMessageDispatcherImpl::current()->RemoveObserver(this);
  }
}