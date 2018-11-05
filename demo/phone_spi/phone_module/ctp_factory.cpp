/**
* im.cpp
* this file implement the im module
*
* @version 1.0 (06 ,04, 2008)
* @author reinhardt ken
*
*/

#include "../../phone_common/include/ctp_type.h"
#include "../../phone_common/include/ctp_define.h"
#include "../../phone_common/include/moduleimpl.h"

#include "phone_module.h"


IModule * g_ImModule = NULL;
IModuleFactory * g_IModuleFactory=NULL;



  IModuleFactory* GetModuleFactory()
  {
    if( g_IModuleFactory == NULL)
    {
      g_IModuleFactory = new ModuleFactoryImpl<phone_module::CTPModule>();
      g_IModuleFactory->QueryIModulePoint(1, (IModule*&)g_ImModule);
    }

    return g_IModuleFactory;
  }

  void ReleaseModuleFactory(IModuleFactory *p)
  {
    if( g_IModuleFactory != NULL)
    {
      delete g_IModuleFactory;
      g_IModuleFactory = NULL;
    }
  }
