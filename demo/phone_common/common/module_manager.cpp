/**
* ModuleManagerImpl.cpp
* this file define the ModuleManagerImpl class
*
* @version 1.0 (06 ,03, 2008)
* @author reinhardt ken
*
*/


#include "module_manager.h"
#include "initlist.h"
#include "../include/ctp_define.h"
#include "common_thread.h"

#include "windows.h"

#include <sstream>



#include "base/location.h"
#include "base/string_util.h"
#include "base/logging.h"



static InitList g_stInitList;

#define _LOG(x)




namespace core_module {
#define 	EXIST_MODULE(Y,X) ((Y=m_IModulePointC.find((ModuleID)(X)))!= m_IModulePointC.end())


ModuleManagerImpl::ModuleManagerImpl(CommandLine  const &c)
		:m_CoreModule(c),
    m_CommandLine(c) {
	DCHECK(CommonThread::CurrentlyOn(CommonThread::CTP));
}


void ModuleManagerImpl::Construct() {
  Init();
}

ModuleManagerImpl::~ModuleManagerImpl()
{
	
}

void ModuleManagerImpl::Destruct()
{
	destroy();
}

	void ModuleManagerImpl::Init() {

		
		for(InitList::DllModuleList::iterator it=g_stInitList.m_stDllModuleList.begin();it!=g_stInitList.m_stDllModuleList.end();++it)
		{
			HMODULE   exportmodule;

			IModule *i_module_=NULL;
			
			exportmodule =LoadLibrary(it->first.c_str());
			assert(exportmodule != NULL);
			if(exportmodule==NULL) {
				DCHECK(false);
				return ;
			}
			
			m_ModuleInterfaceC[it->first]=
				ModuleInterface(reinterpret_cast<GetModuleFactoryFunc >(GetProcAddress(exportmodule, "GetModuleFactory")),
										reinterpret_cast<ReleaseModuleFactoryFunc>(GetProcAddress(exportmodule, "ReleaseModuleFactory")),
										it->second.size());

			if(m_ModuleInterfaceC[it->first].m_GetModuleFactoryFunc==NULL) {
				return ;
			}
			else {
				IModuleFactory *pIModuleFactory=m_ModuleInterfaceC[it->first].m_GetModuleFactoryFunc();
				if(pIModuleFactory)
				{
					uint32 counter=0;
					bool re=pIModuleFactory->QueryIModuleCounter(counter);
					if(re==true )
					{
						m_ModuleInterfaceC[it->first].m_pstIModuleFactory=pIModuleFactory;
						if(counter<=m_ModuleInterfaceC[it->first].m_IModule.size())
						{
							bool re=pIModuleFactory->QueryIModulePoint(counter,m_ModuleInterfaceC[it->first].m_IModule[0]);
							if(re==true)
							{
								for(uint32 i=0;i<counter;i++)
								{
									m_IModulePointC[it->second[i]]=m_ModuleInterfaceC[it->first].m_IModule[i];
								}
							}
							else {
								DCHECK(false);
								return ;
							}
						}
						else {
							DCHECK(false);
							return ;
						}
					}
					else {
						DCHECK(false);
						return ;
					}
				}
				else {
					DCHECK(false);
					return ;
				}
			}
	
			
		}

		m_IModulePointC[MODULE_ID_CORE]=&(this->m_CoreModule);

		//let every module has a chance to Init itself
		for(IModulePointC::iterator it=m_IModulePointC.begin();it!=m_IModulePointC.end();++it)
		{
				it->second->Load(this);
		}

		}

	void ModuleManagerImpl::destroy()
	{

		for(IModulePointC::iterator it=m_IModulePointC.begin();it!=m_IModulePointC.end();++it)
		{
			it->second->Unload();
		}

		m_IModulePointC.clear();

		for(ModuleInterfaceC::iterator it=m_ModuleInterfaceC.begin();it!=m_ModuleInterfaceC.end();++it)
		{
			it->second.m_pstIModuleFactory->ReleaseIModulePoint(it->second.m_IModule.size(),it->second.m_IModule[0]);
			it->second.m_ReleaseModuleFactoryFunc(it->second.m_pstIModuleFactory);
		}

		m_ModuleInterfaceC.clear();

	}

}


	


#pragma warning (default:4819)
#pragma warning (default:4227)