#pragma  once
#include "base/memory/scoped_ptr.h"
#include "base/cancelable_callback.h"

#include "../../phone_common/include/ctp_define.h"
#include "../../phone_common/include/ctp_messages.h"
#include "../ctp_api/ThostFtdcTraderApi.h"
#include "common_struct_define.h"

#pragma warning(disable:4566)
class PrefService;
extern PrefService * g_pref_service;


struct NewOne {
  template<typename T>
    T * operator() (T const *t) {
      if (t)
        return new T(*t);
      else
        return NULL;
    }
};

#define CALLBACK_MACRO(x) \
if (!Delegate()) return; \
ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::CTP,\
  new CTP_NetWork_Callback(Delegate()->routing_id(),\
  x,\
  PointerWrapper<void>(),\
  PointerWrapper<CThostFtdcRspInfoField>(),\
  std::vector<int>(2)));


#define CALLBACK_MACRO4(x, special, common, re, last) \
  if (!Delegate()) return; \
  std::vector<int> temp_vector(2);\
  temp_vector[0] = re;\
  temp_vector[1] = last;\
  ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::CTP,\
  new CTP_NetWork_Callback(Delegate()->routing_id(),\
  x,\
  PointerWrapper<void>(NewOne()(special)),\
  PointerWrapper<CThostFtdcRspInfoField>(common ? (new CThostFtdcRspInfoField(*(CThostFtdcRspInfoField*)common)) : NULL),\
  temp_vector));


namespace phone_module {
typedef base::CancelableCallback<void(void)> DisConnectCallback;
typedef base::CancelableCallback<void(void)> ConnectTimeoutCallback;
typedef base::CancelableCallback<void(void)> TradeOrderTimeoutCallback;

typedef base::CancelableCallback<void(void)> VoidVoidCallback;

//const value
//2s的话，StateLoginning会因为这个太短，到不了StateLogined。故先放长到10s
static uint32 const kDelayNotifyDisConnect = 10;//5s
static uint32 const kDelayConnectTimeout = 20;//20s
static uint32 const kDelayLoginTimeout = 10;//20s
static uint32 const kTradeOrderTimeout = 10;//5s

/**
-> 程序到网络
<- 网络到程序
- 没有方向

CM CTPModule
TU TradeUnit
LSM LoginStateMachine
MQ MarketQuotation
MS MarketStrategy
RMS RandomMarketStrategy
DC DataCenter
CO Console
TIMER timer
LS LocaleState
ENV Enviroment
TSP TradeSpi
MSP MdSpi
*/

void NotifyMessageFunction(MessageType2UI const type, uint32 const id, SSMap const &m);


#define NOTIFYMESSAGEMACRO0(type, x) phone_module::SSMap m; \
  NotifyMessageFunction(type, x, m);

#define NOTIFYMESSAGEMACRO1(type, x, k1, v1) phone_module::SSMap m; \
	m[k1] = v1; \
	NotifyMessageFunction(type, x, m);

#define NOTIFYMESSAGEMACRO2(type, x, k1, v1, k2, v2) phone_module::SSMap m; \
	m[k1] = v1; \
	m[k2] = v2; \
	NotifyMessageFunction(type, x, m);

#define NOTIFYMESSAGEMACRO3(type, x, k1, v1, k2, v2, k3, v3) phone_module::SSMap m; \
	m[k1] = v1; \
	m[k2] = v2; \
	m[k3] = v3; \
	NotifyMessageFunction(type, x, m);

#define NOTIFYMESSAGEMACRO4(type, x, k1, v1, k2, v2, k3, v3, k4, v4) phone_module::SSMap m; \
	m[k1] = v1; \
	m[k2] = v2; \
	m[k3] = v3; \
	m[k4] = v4; \
	NotifyMessageFunction(type, x, m);

#define NOTIFYMESSAGEMACRO5(type, x, k1, v1, k2, v2, k3, v3, k4, v4, k5, v5) phone_module::SSMap m; \
	m[k1] = v1; \
	m[k2] = v2; \
	m[k3] = v3; \
	m[k4] = v4; \
	m[k5] = v5; \
	NotifyMessageFunction(type, x, m);

#define NOTIFYMESSAGEMACRO6(type, x, k1, v1, k2, v2, k3, v3, k4, v4, k5, v5, k6, v6) phone_module::SSMap m; \
	m[k1] = v1; \
	m[k2] = v2; \
	m[k3] = v3; \
	m[k4] = v4; \
	m[k5] = v5; \
	m[k6] = v6; \
	NotifyMessageFunction(type, x, m);

#define NOTIFYMESSAGEMACRO7(type, x, k1, v1, k2, v2, k3, v3, k4, v4, k5, v5, k6, v6, k7, v7) phone_module::SSMap m; \
	m[k1] = v1; \
	m[k2] = v2; \
	m[k3] = v3; \
	m[k4] = v4; \
	m[k5] = v5; \
	m[k6] = v6; \
	m[k7] = v7; \
	NotifyMessageFunction(type, x, m);

#define NOTIFYMESSAGEMACRO8(type, x, k1, v1, k2, v2, k3, v3, k4, v4, k5, v5, k6, v6, k7, v7, k8, v8) phone_module::SSMap m; \
	m[k1] = v1; \
	m[k2] = v2; \
	m[k3] = v3; \
	m[k4] = v4; \
	m[k5] = v5; \
	m[k6] = v6; \
	m[k7] = v7; \
	m[k8] = v8; \
	NotifyMessageFunction(type, x, m);

#define NOTIFYMESSAGEMACRO9(type, x, k1, v1, k2, v2, k3, v3, k4, v4, k5, v5, k6, v6, k7, v7, k8, v8, k9, v9) phone_module::SSMap m; \
	m[k1] = v1; \
	m[k2] = v2; \
	m[k3] = v3; \
	m[k4] = v4; \
	m[k5] = v5; \
	m[k6] = v6; \
	m[k7] = v7; \
	m[k8] = v8; \
	m[k9] = v9; \
	NotifyMessageFunction(type, x, m);

  #define NOTIFYMESSAGEMACROA(type, x, k1, v1, k2, v2, k3, v3, k4, v4, k5, v5, k6, v6, k7, v7, k8, v8, k9, v9, ka, va) phone_module::SSMap m; \
	m[k1] = v1; \
  m[k2] = v2; \
  m[k3] = v3; \
  m[k4] = v4; \
	m[k5] = v5; \
	m[k6] = v6; \
	m[k7] = v7; \
	m[k8] = v8; \
	m[k9] = v9; \
  m[ka] = va; \
	NotifyMessageFunction(type, x, m);

//#define RETURN_BOOL_IF_FALSE_LOG_INFO(X, Y) return ((X) == true ? true : LOG(INFO)<<__FILE__<<__LINE__<<(Y), false);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct NetworkCallback {
  virtual bool OnNetworkCallback(int32 const type,
    //tricky
    PointerWrapper<void> const &,
    PointerWrapper<CThostFtdcRspInfoField> const &,
    int const re,
    bool const last) {
      return false;
  }


  struct ExtraInfo {
    std::string order_ref;
  };

  virtual bool OnNetworkCallback2(int32 const type,
    //tricky
    PointerWrapper<void> const &,
    PointerWrapper<CThostFtdcRspInfoField> const &,
    int const re,
    bool const last,
    ExtraInfo const *
    ) {
      return false;
  }
};

void scoped_ptr_malloc_donothing(void*);

struct TradeAndMQDelegate {
  struct ExtraDataBase {
    virtual ~ExtraDataBase() {}
  };

  template<typename T>
  class ExtraData : ExtraDataBase {
  public:
    ExtraData(T* p)
      :p_(p) {}

    virtual ~ExtraData() {}
    T const* get() const { return p_.get(); }
  private:
    std::shared_ptr<T> p_;
  };

   
  virtual std::string const& server_and_ip() = 0;
  virtual std::string const& broker_id() = 0;
  virtual std::string const& account() = 0;
  virtual std::string const& investor_id() = 0;
  //将来关注的行情超过buf了，再弄set
  virtual std::string const & instrument_list() { return DCHECK(false), *reinterpret_cast<std::string const *>(0); }
  virtual std::set<std::string> const & position_list() { return DCHECK(false), *reinterpret_cast<std::set<std::string> const *>(0); }
  virtual std::string const &password() = 0;
  //...
  virtual std::string const order_ref() = 0;
  virtual std::string const order_ref(std::string const & key) = 0; 
  virtual int const request_id() = 0;
  // [8/27/2013 ken] 记录某个请求对应的附加数据，在查询持仓的时候，如果没持仓，测试返回的数据没有品种导致=
  //根据持续返回的逻辑失效=
  virtual void SetRequestIDExtraData(int const id, std::shared_ptr<ExtraDataBase> * p) = 0;
  virtual ExtraDataBase const* GetRequestIDExtraData(int const id) = 0;
  virtual int32 const routing_id() = 0;

  virtual std::string flow_trade_dir() = 0;
};

struct LoginStateMachineDelegate {
  virtual void Connect() = 0;
  //can't...
  virtual void DisConnect() = 0;
  virtual bool Login() = 0;
  virtual bool Logout() = 0;
};


struct TradeDelegate {
  virtual bool OrderInsert(CThostFtdcInputOrderField & order) = 0;
  virtual bool OrderAction(CThostFtdcInputOrderActionField & action) = 0;
};

struct QueryDelegate {
  virtual bool SettlementInfoConfirm() = 0;
  //query
  virtual bool QryBank() = 0;
  virtual bool QryCash() = 0;
  virtual bool QryPosition(std::string const &id) = 0;
	virtual bool QryPositionList() = 0;
	virtual bool QryInstrument(std::string const &id) = 0;
	virtual bool QryInstrumentList() = 0;
  virtual bool QryMarginRate(std::string const &id) = 0;
  virtual bool QryMarginRateList() = 0;
  virtual bool QryCommissionRate(std::string const &id) = 0;
  virtual bool QryCommissionRateList() = 0;
};

struct ThostFtdcTSInterface : public LoginStateMachineDelegate,
  public TradeDelegate, 
  public QueryDelegate {
  virtual ~ThostFtdcTSInterface() {}
  //=========================
  virtual void Connect() = 0;
  //can't...
  virtual void DisConnect() = 0;
  virtual bool Login() = 0;
  virtual bool Logout() = 0;
  //=========================
  virtual bool OrderInsert(CThostFtdcInputOrderField & order) = 0;
  virtual bool OrderAction(CThostFtdcInputOrderActionField & action) = 0;
  //=========================
  virtual bool SettlementInfoConfirm() = 0;
  //query
  virtual bool QryBank() = 0;
  virtual bool QryCash() = 0;
  virtual bool QryPosition(std::string const &id) = 0;
	virtual bool QryPositionList() = 0;
	virtual bool QryInstrument(std::string const &id) = 0;
	virtual bool QryInstrumentList() = 0;
  virtual bool QryMarginRate(std::string const &id) = 0;
  virtual bool QryMarginRateList() = 0;
  virtual bool QryCommissionRate(std::string const &id) = 0;
  virtual bool QryCommissionRateList() = 0;
  //=============================
  virtual void ReleaseDelegate() = 0;
};


struct ThostFtdcMSInterface : public LoginStateMachineDelegate {
  virtual ~ThostFtdcMSInterface() {}
  //=========================
  virtual void SubscribeMarketData() = 0;
  //=============================
  virtual void ReleaseDelegate() = 0;
};

struct NetWorkCallbackType {
  enum {
    //common
    TypeOnFrontConnected,
    TypeOnFrontDisconnected,
    TypeOnRspUserLogin,
    TypeOnRspUserLogout,
    TypeOnRspError,

    //trade
    TypeOnRspSettlementInfoConfirm,
    TypeOnRspQryInstrument,
    TypeOnRspQryTradingAccount,
    TypeOnRspQryInvestorPosition,
    TypeOnRspOrderInsert,
    TypeOnRspOrderAction,
    TypeOnHeartBeatWarning,
    TypeOnRtnOrder,
    TypeOnRtnTrade,

    //market
    TypeOnRspSubMarketData,
    TypeOnRspUnSubMarketData,
    TypeOnRtnDepthMarketData,

    //QRY
    TypeOnRspQryInstrumentMarginRate,
    TypeOnRspQryInstrumentCommissionRate,
  };
};

namespace ctp_prefs {
  //pref
  extern const char kMQBroker[];
  extern const char kMQAccount[];
  extern const char kMQTHDCServer[];
  extern const char kMQInstrmentList[];
  extern const char kMQPassword[];
  extern const char kMQMockMQ[];
  extern const char kDBCreateTableListType[];
  extern const char kDBCreateTableListLevel[]; 
  extern const char kTUS[];
  extern const char kTU_PROPS[];
  //extern const char kTestAccountCash[];
  extern const char kMQMemoryDB2File[];
}




struct MQHighLevelCalculateParams {
  MQHighLevelCalculateParams() {}
  MQHighLevelCalculateParams(base::Time const & s, base::Time const & e)
    :start(s),
    end(e) {}
  base::Time start;
  base::Time end;
};

struct QuotationInfo {
  MQLevel level;
  base::Time time;
  std::string type;
  std::string time_string;

  std::shared_ptr<CThostFtdcDepthMarketDataField> basic_data;
  std::shared_ptr<LevelQuotationInfo> level_data;
  std::shared_ptr<MQHighLevelCalculateParams> high_level_calc_params;
};


struct CTPStoreInitParams {
  std::string type;
  std::string level;
};








struct LastPrice {
  LastPrice()
    :valid(false),
    ask_one(-1),
    bid_one(-1),
    price(-1) {}

  LastPrice(base::Time const t, double const p, double const sp, double const bp)
    :valid(true),
    time(t),
    ask_one(sp),
    bid_one(bp),
    price(p) {}    


  base::Time time;
  double price;
  double ask_one;
  double bid_one;
  bool valid;
};





struct ExecuteStateMachineResponse {
  enum Result {
    SuccessTrade,
    OrderError,
    OrderTimeout,
    ForceStop,
  };

  Result result;
  std::string type;
  double price;
  uint32 volumn;
  std::string description;
  std::string force_detail;//强制停机时，具体描述强制停机的原因==
};

struct ExecuteStateMachineDelegate : public TradeDelegate {
  virtual void CreateOrderInsert(CThostFtdcInputOrderField & req) = 0;
  virtual void CreateOrderAction(CThostFtdcInputOrderActionField & req) = 0;
  virtual void OnOrder(CThostFtdcOrderField const*) = 0;
  virtual void OnTrade(CThostFtdcTradeField const*) = 0;
  virtual void OnOrderError(CThostFtdcRspInfoField const*, CThostFtdcInputOrderField const* pl) = 0;
  virtual void OnActionError(CThostFtdcRspInfoField const*) = 0;
  virtual void Notify(ExecuteStateMachineResponse const &) = 0;
  virtual std::string const & key() = 0;
  virtual std::string const & ba() = 0;
  virtual std::string const current_msos() = 0;
  virtual std::string const current_msid() = 0;
};


struct ExtraControlParams {
  enum ExtraControlParamsEnum {
    LowPrice = 0x1,
    WrongOrderRef = 0x2,
  };
  ExtraControlParamsEnum value;
};


TThostFtdcDirectionType DirectionEnum2TThostFtdcDirectionType(BuySellEnum const d);
TThostFtdcDirectionType TThostFtdcDirectionType2CloseDirection(TThostFtdcDirectionType const d);
BuySellEnum TThostFtdcDirectionType2DirectionEnum(TThostFtdcDirectionType const d);
BuySellEnum TThostFtdcPosiDirectionType22DirectionEnum(TThostFtdcPosiDirectionType const d);

double const kInvalidLimitsPrice = 0.0000001;
////////////////////////////////////////////////////////////////////////////////////
struct Value2Text {
  static char* TThostFtdcOrderSubmitStatusType2Text(TThostFtdcOrderSubmitStatusType const t) {
    ///已经提交
    if (THOST_FTDC_OSS_InsertSubmitted == t) {
      return "已经提交";
    } else if (THOST_FTDC_OSS_CancelSubmitted == t) {
      return "撤单已经提交";
    } else if (THOST_FTDC_OSS_ModifySubmitted == t) {
      return "修改已经提交";
    } else if (THOST_FTDC_OSS_Accepted == t) {
      return "已经接受";
    } else if (THOST_FTDC_OSS_InsertRejected == t) {
      return "报单已经被拒绝";
    } else if (THOST_FTDC_OSS_CancelRejected == t) {
      return "撤单已经被拒绝";
    } else if (THOST_FTDC_OSS_ModifyRejected == t) {
      return "改单已经被拒绝";
    } else {
      return "Unknown";
    }
  }

  static char* TThostFtdcOrderStatusType2Text(TThostFtdcOrderStatusType const t) {
    ///已经提交
    if (THOST_FTDC_OST_AllTraded == t) {
      return "全部成交";
    } else if (THOST_FTDC_OST_PartTradedQueueing == t) {
      return "部分成交还在队列中";
    } else if (THOST_FTDC_OST_PartTradedNotQueueing == t) {
      return "部分成交不在队列中";
    } else if (THOST_FTDC_OST_NoTradeQueueing == t) {
      return "未成交还在队列中";
    } else if (THOST_FTDC_OST_NoTradeNotQueueing == t) {
      return "未成交不在队列中";
    } else if (THOST_FTDC_OST_Canceled == t) {
      return "撤单";
    } else if (THOST_FTDC_OST_Unknown == t) {
      return "未知";
    } else if (THOST_FTDC_OST_NotTouched == t) {
      return "尚未触发";
    } else {
      return "Unknown";
    }
  }


  static char* TThostFtdcOrderTypeType2Text(TThostFtdcOrderTypeType const t) {
    ///已经提交
    if (THOST_FTDC_ORDT_Normal == t) {
      return "正常";
    } else if (THOST_FTDC_ORDT_DeriveFromQuote == t) {
      return "报价衍生";
    } else if (THOST_FTDC_ORDT_DeriveFromCombination == t) {
      return "组合衍生";
    } else if (THOST_FTDC_ORDT_Combination == t) {
      return "组合报单";
    } else if (THOST_FTDC_ORDT_ConditionalOrder == t) {
      return "条件单";
    } else if (THOST_FTDC_ORDT_Swap == t) {
      return "互换单";
    } else {
      return "Unknown";
    }
  }

static wchar_t* Operation2Text(int32 const t) {
    if (OrderFootprints::OneRecord::OrderRequest == t) {
      return L"报单";
    } else if (OrderFootprints::OneRecord::OrderResponse == t) {
      return L"报单回复";
    } else if (OrderFootprints::OneRecord::OrderResponseError == t) {
      return L"报单错误";
    } else if (OrderFootprints::OneRecord::ActionRequest == t) {
      return L"撤单";
    } else if (OrderFootprints::OneRecord::ActionResponseError == t) {
      return L"撤单错误";
    } else if (OrderFootprints::OneRecord::TradeResponse == t) {
      return L"成交回复";
    } else {
      return L"未知。。。";
    }
  }

  static char* TThostFtdcCombOffsetFlagType2Text(char const t) {
    if (THOST_FTDC_OF_Open == t) {
      return "开仓";
    } else if (THOST_FTDC_OF_Close == t) {
      return "平仓";
    } else if (THOST_FTDC_OF_ForceClose == t) {
      return "强平";
    } else if (THOST_FTDC_OF_CloseToday == t) {
      return "平今";
    } else if (THOST_FTDC_OF_CloseYesterday == t) {
      return "平昨";
    } else {
      return "Unknown";
    }
  }

};


std::string MakeupKey(std::string const & type, BuySellEnum const direction);
std::string MakeupKey(std::string const & type, TThostFtdcPosiDirectionType const direction);
std::string MakeupKey(std::string const & type, TLVLongShortEnum const direction);
std::string Key2Type(std::string const & key);

bool IsSpecialType(std::string const &s);

int GetPointFromType(std::string const &type);
//给回测用的。因为回测的type是写在配置文件里面的，外面不要用这个函数
bool IsSpecialTypeIgnoreCase(std::string const &s);

double CostCash2MarketPrice(std::string const & type, double const cash, double const margin_rate);
double MarketPrice2CostCash(std::string const & type, double const price, double const margin_rate);
double ProfitPerHand(std::string const & type, phone_module::BuySellEnum const direction, double const old_index, double const new_index);
double FeePerHand(std::string const & type, double const index, double const percent);
double PoundCostPerHand(std::string const & type, double const percent);

bool IsBacktestingMode(std::string const & process_type);

BuySellEnum BuySellEnumReverse(BuySellEnum const old);


phone_module::MQLevel const String2Enum(std::string const &s);
std::string Enum2String(phone_module::MQLevel const l);



bool DiscardLevelMQ(std::string const & type, std::string const & time_string, base::Time const & time, phone_module::MQLevel const level);

}


class TestClass {
public :
  TestClass(void (*test)() ) {
    test();
  }
};




namespace version {
  extern const char kVersion[];
}


namespace RCommand {
  //cmd表示从r到ctp
  extern const char CommandMS[];
  extern const char CommandPSP[];

//NOTIFY CTP ->R
  extern const char NotifyMarketQuotation[];
  extern const char NotifyMarketQuotationV2[];
  extern const char NotifyPositionCost[];
  extern const char NotifyOrderStop[];
  extern const char NotifyOrderStopWin[];
}





//测试帐号总有些数据字段不对，为了不影响代码稳定性同时容错，增加TEST_MODE宏，如果开启，自动做一些容错处理=
#if defined(_DEBUG)
#define TEST_MODE 1
#endif
