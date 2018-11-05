/**
* ModuleManagerImpl.cpp
* this file define the ModuleManagerImpl class
*
* @version 1.0 (06 ,03, 2008)
* @author reinhardt ken
*
*/

#include "core_module.h"

#include "../include/ctp_type.h"
#include "../include/imodulemanager.h"
#include "../include/moduleimpl.h"
#include "../include/ctp_notification_types.h"
#include "../include/ctp_messages.h"

//#include "log.h"
#include "base/string_util.h"


#include "module_manager.h"

//#include "./memorycheck/mcinterface.h"


namespace core_module {
/*
	int32 CoreModule::Invoke(ServiceValue const value,Param lparam)	{
    return 0;
	}*/





	void CoreModule::Load(IModuleManager *p)	{
		ModuleImpl::Load(p);
	}

	void CoreModule::Unload()	{
		ModuleImpl::Unload();
	}



  CoreModule::CoreModule(CommandLine const& _CommandLine)
    :m_CommandLine(_CommandLine) {
      //if (content::NotificationServiceImpl::current() == NULL)
      notification_service_on_ctp_thread_.reset(new content::NotificationServiceImpl);

      //ugly?
      //毫无疑问，这是设计缺陷
      thread_message_dispatcher_impl_.reset(ThreadMessageDispatcherImpl::Create());
      thread_message_dispatcher_impl_->OpenPerformanceCheck();

/*
      registrar_.Add(this, content::NOTIFICATION_4_CTP_MODULE,
        content::NotificationService::AllSources());*/
  }

  void CoreModule::Observe(int type,
    const content::NotificationSource& source,
    const content::NotificationDetails& details) {

      if (type > phone_module::NOTIFICATION_CTP_START &&
          type < phone_module::NOTIFICATION_CTP_END) {
            //ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::UI, new CTP_MessageBroadcast(type, base::Time::Now()));
        //switch (content::Details<phone_module::NotifyMessage>(details)->message_id) {
      }
  }


}
