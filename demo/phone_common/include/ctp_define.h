/**
* Imdefine.h
* this file define the im module's interface
*
* @version 1.0 (06 ,04, 2008)
* @author reinhardt ken
*
*/
#pragma  once
#include <xstring>
#include <map>
#include <set>
#include <vector>


#include "base/time.h"


#include "ctp_type.h"
#include "imodule.h"






#if defined(PHONE_SPI)
#define PHONE_SPI_EXPORT __declspec(dllexport)
#else
#define PHONE_SPI_EXPORT __declspec(dllimport)
#endif

#if defined(PHONE_COMMON)
#define PHONE_COMMON_EXPORT __declspec(dllexport)
#else
#define PHONE_COMMON_EXPORT __declspec(dllimport)
#endif


extern "C" {
/*
  PHONE_SPI_EXPORT void HelloWorld();
  PHONE_SPI_EXPORT void InitTrader();
  PHONE_SPI_EXPORT void LoginTrader();
  PHONE_SPI_EXPORT void LoginAndJoinTrader();
  PHONE_SPI_EXPORT void InitMd();
  PHONE_SPI_EXPORT void LoginMd();
  PHONE_SPI_EXPORT void LoginAndJoinMd();*/


  PHONE_SPI_EXPORT IModuleFactory*  GetModuleFactory();
  PHONE_SPI_EXPORT void  ReleaseModuleFactory( IModuleFactory*);
}


extern "C" {
  //test
/*
  typedef void (*HWPtr)();
  //td
  typedef void (*InitTdPtr)();
  typedef void (*LoginTdPtr)();
  typedef void (*LoginAndJoinTdPtr)();
  //md
  typedef void (*InitMdPtr)();
  typedef void (*LoginMdPtr)();
  typedef void (*LoginAndJoinMdPtr)();*/

}



#include <string>
#include <xstring>
//#include "Windows.h"

enum ModuleID
{
  //MODULE_ID_HISTORY_SURRENDER=-2,
  MODULE_ID_INVALID=-1,
  MODULE_ID_CORE=0X00,
  MODULE_ID_CTP=0X01,
  

  MODULE_ID_END,//
};


namespace phone_module {

	typedef std::map<std::string, std::string> SSMap;
	typedef SSMap::iterator SSMapIt;
	typedef SSMap::const_iterator SSMapCIt;

  struct Message {
    SSMap head;
  };


  enum MessageType2UI{
    IMPORTANT,
    UIERROR,
    NORMAL,
    CONSOLECMD,
    ORDER,
    MARKET_QUOTATION,
    POSITION,
    FROM_R,
  };

	struct NotifyMessage {
    uint32 type;
		uint32 message_id;
		base::Time time;
		SSMap msg_map;
	}; 

}



#define DCHECK_RLOG(x) \
  DCHECK(x);\
  if (!(x)) {\
  LOG(ERROR)<<__FILE__<<__LINE__; \
  }




