#include "common_util.h"


#include "base/logging.h"
#include "base/StringPrintf.h"
#include "base/lazy_instance.h"
#include "base/threading/thread_local.h"
#include "base/utf_string_conversions.h"
#include "base/string_number_conversions.h"

#include "../include/ctp_notification_types.h"
#include "../include/ctp_messages.h"
#include "../common/thread_message_filter.h"
#include "../common/common_common.h"
#include "../common/blowfish.h"


namespace common_util {

  namespace {

    base::LazyInstance<base::ThreadLocalPointer<PerformanceThreadCheck> >
      lazy_tls_ptr = LAZY_INSTANCE_INITIALIZER;

  }  // namespace


  // static
  PerformanceThreadCheck* PerformanceThreadCheck::current() {
    return lazy_tls_ptr.Pointer()->Get();
  }



  // static
  PerformanceThreadCheck* PerformanceThreadCheck::Create() {
    return new PerformanceThreadCheck;
  }



  PerformanceThreadCheck::PerformanceThreadCheck() {
    DCHECK(current() == NULL);
    DCHECK(CalledOnValidThread());

    lazy_tls_ptr.Pointer()->Set(this);
  }

  void PerformanceThreadCheck::Add(std::shared_ptr<PerformanceQueueItem const> item) {
      DCHECK(CalledOnValidThread());
      DCHECK(item);

      queue_.push_back(item);
  }



  std::string PerformanceThreadCheck::Dump() {
    std::string re;
    PerformanceVector::reverse_iterator rit = queue_.rbegin();
    for(; rit != queue_.rend(); ++rit) {
      std::string s = base::StringPrintf("[f]=%s [l]=%d [e]=%s [c]=%lld ||",
        (*rit)->func_name, (*rit)->line, (*rit)->extra_info ? (*rit)->extra_info : "0", (*rit)->cost.ToInternalValue()/base::Time::kMicrosecondsPerMillisecond);
      re += s;
    }
    return re;
  }


  void PerformanceThreadCheck::Clear() {
    queue_.clear();
  }

  base::TimeDelta PerformanceThreadCheck::CalculateCosts() {
    if (queue_.size()) {
      return queue_.back()->cost;
    } else {
      return base::TimeDelta();
    }
  }

  PerformanceThreadCheck::~PerformanceThreadCheck() {
    DCHECK(CalledOnValidThread());

    lazy_tls_ptr.Pointer()->Set(NULL);
  }




  ScopedPerformanceCheck::~ScopedPerformanceCheck() {
    item_->cost = base::Time::Now() - start_;
    if (PerformanceThreadCheck::current()) {
      PerformanceThreadCheck::current()->Add(item_);
    }
  }
































  ///////////////////////////////////////////////////////////////////////
  std::string SSMap2String(phone_module::NotifyMessage const &m) {
    std::string s;
    phone_module::SSMapCIt it = m.msg_map.begin();
    for (; it != m.msg_map.end(); ++it) {
      s.append("[").append(it->first).append("]:").append(it->second).append(", ");
    }
    return s;
  }

  std::string SSMap2String(phone_module::SSMap const &m) {
    std::string s;
    phone_module::SSMapCIt it = m.begin();
    for (; it != m.end(); ++it) {
      s.append("[").append(it->first).append("]:").append(it->second).append(", ");
    }
    return s;
  }


  void NotifyMessageFunction(phone_module::MessageType2UI const type, uint32 const id, std::string const &m) {
    phone_module::SSMap ssmap;
    ssmap["msg"] = m;


    if (id > phone_module::NOTIFICATION_CTP_START &&
      id < phone_module::NOTIFICATION_CTP_END) {
        ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::UI,
          new CTP_MessageBroadcast(type, id, base::Time::Now(), PointerWrapper<void>(new phone_module::SSMap(ssmap))));

        //to log 
        std::string s = base::StringPrintf("[type]=%d [id]=%s : %s", type, WideToASCII(phone_module::NotificationTypeString(id)).c_str(), m.c_str());
        LOG(INFO)<</*base::SysNativeMBToWide*/(s);
    }
  }



  std::string GetAccout(std::string const & s) {
    if (s == "test") {
      return std::string("1044/00000056");
    } else if (s == "real") {
      return std::string("8888/200600");
    } else {
      DCHECK(false);
    }
    return std::string();
  }


  PrefService * g_pref_service;

  std::string ProcessType2ServerName(std::string const & s) {
    if (s == switches::kProcessTypeGuard || s.size() == 0) {
      return prefs::kGuardServer;
    } else if (s == switches::kProcessTypeMQ) {
      return prefs::kMQServer;
    } else if (s == switches::kProcessTypeTU) {
      return prefs::kTUServer;
    } else if (s == switches::kProcessTypeMQTU) {
      return prefs::kMQTUServer;
    } else if (s == switches::kProcessTypeBacktesting) {
      return prefs::kBacktestingServer;
    } else {
      DCHECK(false);
    }

    return "";
  }

  std::string ProcessType2ClientName(std::string const & s) {
    if (s == switches::kProcessTypeMQ) {
      return switches::kCommunicateMQAlive;
    } else if (s == switches::kProcessTypeTU) {
      return switches::kCommunicateTUAlive;
    } else if (s == switches::kProcessTypeMQTU) {
      return switches::kCommunicateMQAndTUAlive;
    } else if (s == switches::kProcessTypeTest) {
      return switches::kCommunicateTestAlive;
    } else if (s == switches::kProcessTypeBacktesting) {
      return switches::kCommunicateBacktestingAlive;
    } else {
      DCHECK(false);
    }

    return "";
  }

  std::string SocketName2ProcessType(std::string const & s) {

    if (s == switches::kCommunicateCmdAlive) {
      return switches::kProcessTypePyCmd;
    } else if (s == switches::kCommunicateMQAlive) {
      return switches::kProcessTypeMQ;
    } else if (s == switches::kCommunicateTUAlive) {
      return switches::kProcessTypeTU;
    } else if (s == switches::kCommunicateMQAndTUAlive) {
      return switches::kProcessTypeMQTU;
    } else if (s == switches::kCommunicateTestAlive) {
      return switches::kProcessTypeTest;
    } else if (s == switches::kCommunicateBacktestingAlive) {
      return switches::kProcessTypeBacktesting;
    } else {
      DCHECK_RLOG(false);
    }

    return "";
  }


  char* type_2_string[] = {
    "first",
    "CTP_CreateTradeUnit",
    "CTP_TradeUnitCreated",
    "CTP_TradeUnitCreatedNew",
    "CTP_DestroyTradeUnit",
    "CTP_ClearList",
    "CTP_MessageBroadcast",
    "CTP_MQLevel2UI",
    "CTP_MS2UI",
    "CTP_Position2UI",
    "CTP_Cash2UI",
    "CTP_Order2UI",
    "CTP_MQ_Login_Success_2UI",
    "CTP_MQ_Logout_2UI",
    "CTP_TU_Login_Success_2UI",
    "CTP_TU_Logout_2UI",
    "CTP_MQLevelParam",
    "CTP_NetWork_Callback",
    "CTP_TU_Login",
    "CTP_TU_Logout",
    "CTP_MQLogin",
    "CTP_MQLogout",
    "CTP_DC_QryPositionList",
    "CTP_TU_Open",
    "CTP_TU_Close",
    "CTP_TU_OpenCloseNew",
    "CTP_CONSOLE_Hello",
    "CTP_CONSOLE_HelloAck",
    "CTP_CONSOLE_Command",
    "CTP_CONSOLE_CommandProtoBuf",
    "CTP_CONSOLE_CommandRsp",
    "CTP_QUIT_R",
    "CTP_CONSOLE_RoutedCommand",
    "CTP_CONSOLE_RoutedCommandRsp",
    //"CTP_QUIT_R",
    //"CTP_QUIT_R",
    "last",
  };

  std::string MsgType2String(uint32 const type) {
    if (type >= CTP_START::ID && type <= CTP_LAST::ID) {
      return type_2_string[type - CTP_START::ID];
    }

    return "";
  }


  std::wstring ToString(std::vector<std::string> const & v) {
    std::wstring out;
    for (auto it = v.begin(); it != v.end(); ++it) {
      out += ASCIIToWide(*it);
      out += L", ";
    }
    return out;
  }

  std::wstring ToString(std::map<std::string, double> const & v) {
    std::wstring out;
    for (auto it = v.begin(); it != v.end(); ++it) {
      out += L"(";
      out += ASCIIToWide(it->first);
      out += L", ";
      out += ASCIIToWide(base::DoubleToString(it->second));
      out += L"), ";
    }
    return out;
  }

  std::wstring ToString(std::map<std::string, uint32> const & v) {
    std::wstring out;
    for (auto it = v.begin(); it != v.end(); ++it) {
      out += L"(";
      out += ASCIIToWide(it->first);
      out += L", ";
      out += base::UintToString16(it->second);
      out += L"), ";
    }
    return out;
  }

  std::wstring ToString(std::map<std::string, std::string> const & v) {
    std::wstring out;
    for (auto it = v.begin(); it != v.end(); ++it) {
      out += L"(";
      out += ASCIIToWide(it->first);
      out += L", ";
      out += ASCIIToWide(it->second);
      out += L"), ";
    }
    return out;
  }


  void Encode(unsigned char* key, std::vector<char> & buf, std::vector<char> & out) {
    CBlowFish blow;
    blow.Initialize(key, 16);
    int const nlength = buf.size();
    int const nDifference = nlength % 8;
    int nNewLength = nlength;
    if (nDifference != 0) {
      nNewLength = (nlength - nDifference) + 8;
    }
    out.resize(nNewLength);
    int const size = blow.Encode((unsigned char*)&buf[0], (unsigned char*)&out[0], nlength);
    DCHECK(size == nNewLength);
  }
  void Decode(unsigned char* key, std::vector<char> & buf, std::vector<char> & out) {
    CBlowFish blow;
    blow.Initialize(key, 16);
    int const nlength = buf.size();
    int const nDifference = nlength % 8;
    DCHECK(nDifference == 0);
    int nNewLength = nlength;
    if (nDifference != 0) {
      nNewLength = (nlength - nDifference) + 8;
    }
    out.resize(nNewLength);
    ZeroMemory(&out[0], nNewLength);
    blow.Decode((unsigned char*)&buf[0], (unsigned char*)&out[0], nlength);
  }

  // 1 ymd
  // 2 hms
  // 3 ymd hms
  //4 ymd hms ms
  std::string Time2String(base::Time const & time, uint32 const type) {
    base::Time::Exploded e;
    time.LocalExplode(&e);
    std::string time_string;
    if (type == 1) {
      return base::StringPrintf("%d-%d-%d", e.year, e.month, e.day_of_month);
    } else if (type == 2) {
      return base::StringPrintf("%d:%d:%d", e.hour, e.minute, e.second);
    } else if (type == 3) {
      return base::StringPrintf("%d-%d-%d %d:%d:%d", e.year, e.month, e.day_of_month, e.hour, e.minute, e.second);
    } else if (type == 4) {
      return base::StringPrintf("%d-%d-%d %d:%d:%d:%d", e.year, e.month, e.day_of_month, e.hour, e.minute, e.second, e.millisecond);
    } else {
      return base::StringPrintf("%d-%d-%d %d:%d:%d", e.year, e.month, e.day_of_month, e.hour, e.minute, e.second);
    }
  }

  std::wstring Time2StringW(base::Time const & time, uint32 const type) {
    return ASCIIToWide(Time2String(time, type));
  }

  //  [8/8/2016 liuqingping]
  //从一个具体时间得到年，年月，年月日，年月日时等
  // 1 年 ；2 年月；3 年月日；4 年月日小时 5 年月日小时分钟 6 年月日小时分钟秒钟
  base::Time Time2StrictTime(base::Time const & time, uint32 const type) {
    base::Time::Exploded e;
    time.LocalExplode(&e);
    base::Time::Exploded newe(e);
    switch (type) {
      case 1: {
        newe.month = 1;
      }
      case 2: {
        newe.day_of_month = 1;
      }
      case 3: {
        newe.hour = 0;
      }
      case 4: {
        newe.minute = 0;
      }
      case 5: {
        newe.second = 0;
      }
      case 6: {
        newe.millisecond = 0;
      }
    }
    return base::Time::FromUTCExploded(newe);
  }

}
