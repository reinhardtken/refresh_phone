/**
* ModuleManager.h
* this file define the ModuleManager class
*
* @version 1.0 (06 ,03, 2008)
* @author reinhardt ken
*
*/
#pragma once



#include "../include/ctp_type.h"
#include "../include/imodulemanager.h"
#include "../include/moduleimpl.h"
#include "../common/thread_message_filter.h"


#include <list>
#include <map>
#include <xstring>
#include <vector>



//#include <boost/bimap.hpp>



#include "base/scoped_native_library.h"
#include "base/location.h"
#include "base/command_line.h"
#include "base/memory/scoped_ptr.h"
#include "content/browser/notification_service_impl.h"
#include "content/public/browser/notification_registrar.h"
#include "content/public/browser/notification_observer.h"
//#include "base/platform_thread.h"



namespace core_module {
  class CoreModule : public ModuleImpl,
                                public content::NotificationObserver {

  public:
    CoreModule(CommandLine const& _CommandLine);
    //int32 Invoke(ServiceValue const value, Param lparam);
    void Load(IModuleManager *p );
    void Unload();

    // Overridden from content::NotificationObserver:
    virtual void Observe(int type,
      const content::NotificationSource& source,
      const content::NotificationDetails& details) OVERRIDE;

  private:

    friend class ModuleManagerImpl;

    CommandLine const& m_CommandLine;


    scoped_ptr<content::NotificationServiceImpl> notification_service_on_ctp_thread_;
    scoped_ptr<ThreadMessageDispatcherImpl> thread_message_dispatcher_impl_;

    content::NotificationRegistrar registrar_;
  };

}
