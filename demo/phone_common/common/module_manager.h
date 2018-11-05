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
#include "../include/ctp_define.h"
#include "../include/imodulemanager.h"
#include "../include/moduleimpl.h"
#include "core_module.h"


#include <list>
#include <map>
#include <xstring>
#include <vector>



//#include <boost/bimap.hpp>



#include "base/scoped_native_library.h"
#include "base/location.h"
#include "base/command_line.h"
//#include "base/platform_thread.h"



namespace core_module {


class PHONE_COMMON_EXPORT ModuleManagerImpl :IModuleManager {

public:
	ModuleManagerImpl(CommandLine  const &);
	~ModuleManagerImpl();


		/////////////////////////////interface////////////////////////////////////////////////////////////////
		//virtual int32 CallService(Param const , Param);
		//void CallServiceWrapper(Param const , Param);

		void Construct();
		void Destruct();


private:


	
private:
	//����ÿ��ģ���get_module�������˷�������һ��i_moduleָ�룬ͨ����ָ�����
	//module_name���������õ���ģ�����ֺ�i_moduleָ��洢��module_container_m�С�
		void Init();
	void destroy();



private:
	typedef IModuleFactory *  (*GetModuleFactoryFunc)();
	typedef void  (*ReleaseModuleFactoryFunc)(IModuleFactory*);
	struct ModuleInterface
	{
		ModuleInterface():m_GetModuleFactoryFunc(NULL),m_ReleaseModuleFactoryFunc(NULL){}
		ModuleInterface(GetModuleFactoryFunc  p1,ReleaseModuleFactoryFunc p2,std::size_t size)
			:m_GetModuleFactoryFunc(p1),
			m_ReleaseModuleFactoryFunc(p2),
			m_IModule(size),
			m_pstIModuleFactory(NULL)
		{}
		GetModuleFactoryFunc m_GetModuleFactoryFunc;
		ReleaseModuleFactoryFunc m_ReleaseModuleFactoryFunc;
		std::vector<IModule*> m_IModule;
		IModuleFactory * m_pstIModuleFactory;
	};

	typedef std::map<std::wstring,ModuleInterface> ModuleInterfaceC;
	ModuleInterfaceC m_ModuleInterfaceC;


	typedef std::map<ModuleID, IModule*> IModulePointC;

	IModulePointC m_IModulePointC;


  CoreModule m_CoreModule;
  CommandLine  const & m_CommandLine;

};





}
