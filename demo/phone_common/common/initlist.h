/**
* initlist.h
* this file define the IModule initlist
*
* @version 1.0 (08 ,10, 2009)
* @author ken
*
*/

#pragma  once


#include "../include/ctp_type.h"

#include <boost/assign/list_of.hpp>
#include <boost/assign/list_inserter.hpp>
#include <boost/assert.hpp>

#include <string>
#include <vector>
#include <map>
#include <xstring>

//using namespace std;
using namespace boost::assign;


#define MODULE_NAME(X) L#X##L".dll"

struct InitList {
	InitList() {
		insert(m_stDllModuleList )
			(MODULE_NAME(phone_spi), list_of(MODULE_ID_CTP));
			//( MODULE_NAME(option), list_of(MODULE_ID_OPTION)(MODULE_ID_OPTION2) )



	}
	typedef std::vector<ModuleID>    ModuleList;
	typedef std::map<std::wstring,ModuleList>   DllModuleList;
	DllModuleList m_stDllModuleList;
};