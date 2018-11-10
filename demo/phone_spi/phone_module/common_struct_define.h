#pragma  once

#include <list>

#include "base/time.h"

#include "../ctp_api/ThostFtdcTraderApi.h"



namespace db_sqlite {
class SQLitePersistentCTPStoreBackendEx;
}

namespace phone_module {

  enum PYTHON_ERROR_CODE {
    ERROR_CODE_OK = 0,
    //ERROR_CODE_OK = 0
    //ERROR_CODE_PULL_JSON_FAILED = 1
    //ERROR_CODE_PARSE_JSON_FAILED = 2
    //ERROR_CODE_DOWNLOAD_APK_FAILED = 3
    //ERROR_CODE_MD5_APK_FAILED = 4
    //ERROR_CODE_SAVE_JSON_FILE_FAILED = 5
    //ERROR_CODE_SAVE_APK_FILE_FAILED = 6

    //ERROR_CODE_REMOVE_JSON_FILE_FAILED = 7
    //ERROR_CODE_REMOVE_APK_DIR_FAILED = 8

    //ERROR_CODE_UNKNOWN = 100000
  };

  enum PACKAGE_TYPE {
    PACKAGE_INSTALL = 0,//只安装
    PACKAGE_REMOVE = 1,//只卸载
    PACKAGE_BOTH = 2,//若存在，先卸载后安装
  };

enum BuySellEnum {
  BuyOrder = THOST_FTDC_D_Buy,
  SellOrder = THOST_FTDC_D_Sell,
};


enum TLVLongShortEnum {
  LongDirection = 1,
  ShortDirection = -1,
}; 

/*
enum OpenCloseEnum {
  OpenOrder = THOST_FTDC_OF_Open,
  CloseTodayOrder = THOST_FTDC_OF_CloseToday,
  CloseYesterdayOrder = THOST_FTDC_OF_CloseYesterday,
};*/


enum OpenCloseEnum {
  OpenOrder = 1,
  CloseOrder = 2,

  ManualTodayClose = 7,
  ManualYestodayClose = 8,
};

enum OpenCloseCTPEnum {
  OpenCTPOrder = THOST_FTDC_OF_Open,
  CloseCTPTodayOrder = THOST_FTDC_OF_CloseToday,
  CloseCTPYesterdayOrder = THOST_FTDC_OF_CloseYesterday,
};


enum MQLevel {
  MQBASIC = 0,//半秒
  MQ1S,
  MQ5S,
  MQ1M,
  MQ5M,
  MQ15M,
  MQ1H,
  MQ1D,

  MQ_UNKNOWN,
  MQ_ALL = MQ_UNKNOWN,//用于ui显示=
};



enum PlaySoundEnum {
  PLAYSOUND_LOGIN,
  PLAYSOUND_LOGOUT,
  PLAYSOUND_ORDER,
  PLAYSOUND_WITHDRAW,
  PLAYSOUND_CLINCH,

  PLAYSOUND_BT_FINISH,
  PLAYSOUND_UNKNOWN,
};


struct DepthMQ {
  DepthMQ(base::Time const & t, std::string const &s, CThostFtdcDepthMarketDataField const *pp)
  :time(t),
  time_string(s),
  p(pp) {}

  base::Time time;
  std::string const &time_string;
  CThostFtdcDepthMarketDataField const *p;
};

struct LevelQuotationInfo {
LevelQuotationInfo()
  :high(0),
  low(0),
  open(0),
  close(0),
  volumn(0),
  number(0) {}
LevelQuotationInfo(double h,
  double l,
  double o,
  double c,
  int32 v,
  int32 n)
  :high(h),
  low(l),
  open(o),
  close(c),
  volumn(v),
  number(n) {}


  double high;
  double low;
  double open;
  double close;
  int32 volumn;
  int32 number;
  
};

struct LevelQuotationInfoBig {
LevelQuotationInfoBig()
  :high(0),
  low(0),
  open(0),
  close(0),
  volumn(0),
  number(0),
  level(MQ_UNKNOWN),
  ready_4_second_data(false) {}

  LevelQuotationInfoBig(MQLevel const l, std::string const &t)
    :high(0),
    low(0),
    open(0),
    close(0),
    volumn(0),
    number(0),
    level(l),
    type(t),
    ready_4_second_data(false) {}
  double high;
  double low;
  double open;
  double close;
  int32 volumn;
  int32 number;
  
  MQLevel level;
  base::Time time;
  std::string type;
  std::string time_string;
  bool ready_4_second_data;
};

struct OHLCT {
  base::Time ot;
  double op;
  base::Time ht;
  double hp;
  base::Time lt;
  double lp;
  base::Time ct;
  double cp;
};


struct OHLC {
  double o;
  double h;
  double l;
  double c;
OHLC()
  :o(0),
  h(0),
  l(0),
  c(0) {}
};


struct OHLCL {
  double o;
  double h;
  double l;
  double c;
  MQLevel level;
  OHLCL()
    :o(0),
    h(0),
    l(0),
    c(0),
    level(MQ_UNKNOWN) {}
};



struct LevelQuotationInfoTradeUse {
  OHLCL ohlc;
  OHLCL ohlc_day;
  base::Time time;
  std::string type;
  std::string time_string;
};



struct TradeUnitInfo {
  TradeUnitInfo()
    :real_account(false),
    login_all(false),
    id(-1) {}

  std::string bc;
  std::string alias;
  std::string broker;
  std::string account;
  std::string server_ip;
  std::string instrument_list;
  std::map<std::string, uint32> volumn_list;
  std::map<std::string, double> stop_list;
  std::map<std::string, double> stop_win_list;
  std::map<std::string, uint32> auto_close_list;
  std::map<std::string, std::string> type_2_msname_list;
  std::vector<std::string> strategy;
  std::string order_execute;
  std::string status;//normal,lookon,forbidden
  std::string mode;//模式，typemanager模式表明有资金管理和仓位管理参与，那么就不会出现平今不足，如果是justexecute，那么就只有交易执行模块参与
  volatile double money;
  int32 id;
  //  [8/28/2015 ken] 这个字段意义不大了，因为将来新增账户都是外部配置的，无从在代码内部写死
  bool real_account;//是测试帐号，还是真实帐号，根据连接的ip决定==

  bool login_all;
  std::string ps;
  std::string common_name;

  //回测和实盘自算手续费的时候要使用
  std::map<std::string, double> fee_list;

  //  [8/31/2015 ken]
  //有些账户要求降低回撤，要求每手资金占用上升，hc要有一个乘数，key是策略名
  std::map<std::string, double> hc_lever_list;

  //用于回测===
  std::map<std::string, uint32> hands_of_money;
  std::map<std::string, double> margin_rate;
  std::map<std::string, double> origin_money;
  
  std::map<std::string, double> allow_diff;//回测终值与excel的终值允许的计算误差==
  std::map<std::string, double> pound_cost;//冲击成本==

  bool jump_close_and_open;//回测，是否避开15:14行情，在15：13平仓，避开9：15行情，在16开仓==

  void Dump() const;
  // bool operator < (Info const & r) const {
  //   return id < r.id;
  // }
};


//因为要支持多个帐号，所以把各个帐号公共的信息提炼出来
struct TradeUnitInfoCommon {
  TradeUnitInfoCommon()
    :login_all(false) {}

  std::string name;
  std::string instrument_list;
  std::map<std::string, uint32> volumn_list;
  std::map<std::string, double> stop_list;
  std::map<std::string, double> stop_win_list;
  std::map<std::string, uint32> auto_close_list;
  std::map<std::string, std::string> type_2_msname_list;
  std::vector<std::string> strategy;
  std::string order_execute;
  std::string status;//normal,lookon,forbidden
  std::string mode;//模式，typemanager模式表明有资金管理和仓位管理参与，那么就不会出现平今不足，如果是justexecute，那么就只有交易执行模块参与
  
  bool login_all;  

//回测和实盘自算手续费的时候要使用
  std::map<std::string, double> fee_list;

  //  [8/31/2015 ken]
  //有些账户要求降低回撤，要求每手资金占用上升，hc要有一个乘数
  std::map<std::string, double> hc_lever_list;
};



struct MarketStrategyOrder {
  MarketStrategyOrder() : fix_price(false) {}
  base::Time time;//广播这个策略那个点的时间=
  std::string time_string;//用于显示=
  //std::string oa_name;//策略的名字=
  std::string ms_name;//策略发生器的名字=
  std::string os_name;
  std::string type;//品种=
  MQLevel level;
  TThostFtdcPriceType price;//价格=
  double limit_percent;
  bool action;
  int buy_clean_sell;
  //==================================================
  uint32 volumn;//数量=
  TThostFtdcOrderPriceTypeType order_price_type;///报单价格条件=
  TThostFtdcDirectionType direction;//方向，多空=
  TThostFtdcCombOffsetFlagType open_close;//开平=
  
  //新加=
  
  TThostFtdcPriceType buy_limit_price;
  TThostFtdcPriceType sell_limit_price;
  double position_cash;
  double force_close_percent;
  bool auto_close;
  

  //===============
  bool fix_price;
  double origin_price;
  uint32 try_times;
};

struct MarketStrategyOrderBig : public MarketStrategyOrder {
  

  void CopyFromBase(MarketStrategyOrder const & r) {
    //safe??
    MarketStrategyOrder * p = reinterpret_cast<MarketStrategyOrder*>(this);
    *p = r;

    //fix_price = false;
  }
  std::string order_ref;
  std::string older_ref;

  //==================
  // move 2 base
  /*bool fix_price;
  double origin_price;
  uint32 try_times;*/
};

struct PositionData {
  PositionData()
    :price_index(0),
    stop_price(0),
    stop_win_price(0),
    has_makeup_field(false),
    fee(0) {
      ZeroMemory(&field, sizeof(field));
  }
  CThostFtdcInvestorPositionField field;
  base::Time time;//广播这个策略那个点的时间=
  std::string time_string;//用于显示=
  std::string key;
  double price_index;//购买价格对应的指数==
  double stop_price;//根据止损算出的stop价格==
  double stop_win_price;//根据止盈算出的stop价格==
  double fee;//手续费？开着程序就是累加？？==
  bool has_makeup_field;//编造field的数据应该只做一次，不能重复
  std::string ba;
};


struct DeviceData {
  DeviceData(std::wstring const & sn)
    :serial_no(sn),
    time(base::Time::Now()) {
    Init();
  }

  DeviceData()
    :serial_no(L"无"),
    time(base::Time::Now()) {
    Init();
  }

  void Init() {
    if (serial_no.size() == 0) {
      serial_no = L"无";
    }
    base::Time::Exploded e;
    time.LocalExplode(&e);
    time_string = base::StringPrintf(L"%02d:%02d:%02d", e.hour, e.minute, e.second);
  }
  std::wstring serial_no;
  base::Time time;//时间=
  std::wstring time_string;//用于显示=
  std::wstring info;
};


struct StatusInfo {
  StatusInfo(std::wstring const & sn)
    :serial_no(sn),
    time(base::Time::Now()) {
    Init();
  }

  StatusInfo()
    :serial_no(L"无"),
    time(base::Time::Now()) {
    Init();
  }

  void Init() {
    if (serial_no.size() == 0) {
      serial_no = L"无";
    }
    base::Time::Exploded e;
    time.LocalExplode(&e);
    time_string = base::StringPrintf(L"%02d:%02d:%02d", e.hour, e.minute, e.second);
  }
  std::wstring serial_no;
  base::Time time;//时间=
  std::wstring time_string;//用于显示=
  std::wstring op;
  std::wstring result;
};



struct ApkIRStatus {
  ApkIRStatus(std::wstring const & sn)
    :serial_no(sn),
    time(base::Time::Now()) {
    Init();
  }

  ApkIRStatus()
    :serial_no(L"无"),
    time(base::Time::Now()) {
    Init();
  }

  void Init() {
    if (serial_no.size() == 0) {
      serial_no = L"无";
    }
    base::Time::Exploded e;
    time.LocalExplode(&e);
    time_string = base::StringPrintf(L"%02d:%02d:%02d", e.hour, e.minute, e.second);
  }
  std::wstring serial_no;
  base::Time time;//时间=
  std::wstring time_string;//用于显示=
  std::wstring op;
  std::wstring package_name;
  std::wstring percent;
  std::wstring result;
  int error_code;
  std::wstring key;
  std::wstring get_key() {
    if (key.size() == 0) {
      key = serial_no + L"-" + package_name;
    }
    return key;
  }
};


struct ApkUpdateInfo {
  ApkUpdateInfo(int const s)
    :time(base::Time::Now()),
    success(s) {
    Init();
  }

  ApkUpdateInfo()
    :time(base::Time::Now()),
    success(-1) {
    Init();
  }

  void Init() {
    base::Time::Exploded e;
    time.LocalExplode(&e);
    time_string = base::StringPrintf(L"%02d:%02d:%02d", e.hour, e.minute, e.second);
  }

  base::Time time;//时间=
  std::wstring time_string;//用于显示=
  std::wstring op;
  std::wstring package_name;
  std::wstring info;
  int success;
};


struct ApkInstallInfo {
  std::wstring package_name;
  std::wstring md5;
  std::wstring name;
  std::wstring brief;
  double price;

  PACKAGE_TYPE type;
};

struct AdbDevice {
  std::wstring serial_no;
  std::wstring state;
  std::wstring model;
  std::wstring product;
  std::wstring device;
};

typedef std::vector<AdbDevice> DevicesList;




struct StopPriceData {
  std::map<std::string, double> data;
};




//报单及报单答复记录队列，存到db可以用于评估报单执行效率，优化报单策略
  class OrderFootprints {
  public:
    OrderFootprints() {}


    struct OneRecord {
      OneRecord()
        :type(OrderUnknown),
        time(base::Time::Now()) {
          ZeroMemory(&order_request, sizeof(order_request));
          ZeroMemory(&order_response, sizeof(order_response));
          ZeroMemory(&action_request, sizeof(action_request));
          ZeroMemory(&trade_response, sizeof(trade_response));
          ZeroMemory(&common_filed, sizeof(common_filed));
          base::Time::Exploded e;
          time.LocalExplode(&e);
          time_string = base::StringPrintf("%d:%d:%d:%d", e.hour, e.minute, e.second, e.millisecond);
        }

      static OneRecord *StuffSperator(std::string const & type, base::Time const & ms, base::Time const & oss, base::Time const & oso, std::string const & msos) {
        OneRecord * record = new OneRecord;
        record->type = OrderSperator;
        record->type_pinzhong = type;
        record->msos = msos;
        record->ms_arrive = ms;
        record->os_start = oss;
        record->os_over = oso;
        return record;
      }

      static OneRecord *StuffOrderRequest(CThostFtdcInputOrderField const & filed, std::string const & msos) {
        OneRecord * record = new OneRecord;
        record->type = OrderRequest;
        record->order_request = filed;
        record->msos = msos;
        return record;
      }

      static OneRecord *StuffOrderResponse(CThostFtdcOrderField const & filed, std::string const & msos) {
        OneRecord * record = new OneRecord;
        record->type = OrderResponse;
        record->order_response = filed;
        record->msos = msos;
        return record;
      }

      static OneRecord *StuffOrderResponseError(CThostFtdcInputOrderField const & filed, CThostFtdcRspInfoField const & common, std::string const & msos) {
        OneRecord * record = new OneRecord;
        record->type = OrderResponseError;
        record->order_request = filed;
        record->common_filed = common;
        record->msos = msos;
        return record;
      }

      static OneRecord *StuffActionRequest(CThostFtdcInputOrderActionField const & filed, std::string const & msos) {
        OneRecord * record = new OneRecord;
        record->type = ActionRequest;
        record->action_request = filed;
        record->msos = msos;
        return record;
      }

      static OneRecord *StuffActionResponseError(CThostFtdcInputOrderActionField const & filed, CThostFtdcRspInfoField const & common, std::string const & msos) {
        OneRecord * record = new OneRecord;
        record->type = ActionResponseError;
        record->action_request = filed;
        record->common_filed = common;
        record->msos = msos;
        return record;
      }

      static OneRecord *StuffTradeResponse(CThostFtdcTradeField const & filed, std::string const & msos) {
        OneRecord * record = new OneRecord;
        record->type = TradeResponse;
        record->trade_response = filed;
        record->msos = msos;
        return record;
      }

      enum OneRecordEnum {
        OrderUnknown,
        OrderSperator,
        OrderRequest,//order_request字段有效
        OrderResponse,//order_response字段有效
        OrderResponseError,//order_request和common_filed字段有效
        ActionRequest,//action_request字段有效
        ActionResponseError,//action_request和common_filed字段有效
        TradeResponse,//trade_response字段有效
      };
      OneRecordEnum type;
      CThostFtdcInputOrderField order_request;
      CThostFtdcOrderField order_response;
      CThostFtdcInputOrderActionField action_request;
      CThostFtdcTradeField trade_response;
      CThostFtdcRspInfoField common_filed;

      base::Time time;
      std::string time_string;
      std::string type_pinzhong;//我倒。。。==
      std::string msos;
      base::Time ms_arrive;
      base::Time os_start;
      base::Time os_over;
      //base::TimeDelta delta;
    };
    typedef std::list<std::shared_ptr<OneRecord>> List;

    void Add(std::string const & bc, OneRecord *p);

    OneRecord * Back() {
      if (record_list_.size()) {
        return record_list_.back().get();
      }

      return NULL;
    }

    OneRecord const * Back() const {
      if (record_list_.size()) {
        return record_list_.back().get();
      }
    }

    void Clear() {
      record_list_.clear();
    }

    void Dump(db_sqlite::SQLitePersistentCTPStoreBackendEx * p);

  private:
    List record_list_;


    //DISALLOW_COPY_AND_ASSIGN(OrderFootprints);
  };

}

namespace backtesting {

  struct Digest {
    double percent;
    uint32 total;
    uint32 received_number;
    bool finished;
    uint32 index;

    std::string type;
    phone_module::MQLevel level;
    base::Time start;
    base::Time end;
    uint32 number;

  uint32 expect_total;

  base::Time in_ctp_module;//收到r的广播==
  base::Time start_time;
  base::TimeDelta c_cost_time;//c模块的总耗时==
  base::TimeDelta cost_time;//总耗时==  

  //  [8/8/2016 liuqingping]
  // 全天的开高低收
  phone_module::OHLC ohlc_day;
  base::Time ohlc_day_time;
  base::Time ohlc_before_day_time;//前一天最后一个高开低收的时间
  bool new_day;
  //for debug
  base::Time ohlc_day_close_time;
  };

  struct DataCenter {
      
      Digest digest;
      int32 id;
      //trade 
      double origin_money;//本金==
      double pre_open_money;//本次交易之前，最后一次平仓时的cash==
      double pre_money;//本次交易之前的cash==
      uint32 pre_volumn;//本次交易之前的持仓量==
      double profit;//清仓时，此次交易的总获利==
      double trade_cash;//本次交易的总资金量==
      double fee_total;
      double pound_total;
      double money;//本次交易完成后的总资金量==
      double hands_of_money;//手数==
      double open_index;//开仓点数==
      double stop_lose_percent;//止损百分比==
      double stop_win_percent;//止盈百分比==
      double index_2_stop_lose;//止损点数==
      double index_2_stop_win;//止盈点数==
      double volumn;//持仓量==
      base::Time open_time;//记录开仓时间
      phone_module::BuySellEnum direction;
      double max_money;
      double min_money;
      uint32 stop_lose_times;
      uint32 stop_win_times;
      uint32 buy_times;
      uint32 sell_times;
      double max_retracement;
      uint32 max_successive_deficit_period;//最大连续亏损周期数目==
      uint32 successive_deficit_period;//
      uint32 max_no_newhigh_period;//最大净值无新高周期数目==
      uint32 no_newhigh_period;
      uint32 last_successive_deficit_period_index;
      uint32 last_no_newhigh_period_index;

      double net_profit;//如果是赚钱的，就计入这个，是累计净赚==
      double net_deficit;//如果是亏的，就计入这个，是累计净亏==
      bool jump_close_and_open;

      


      
      double allow_diff;
      double max_diff;

      double pound_cost;
      double margin_rate_buy;
      double margin_rate_sell;
      double fee_rate;

      std::vector<double> *money_list;//这个要传到ui线程，所以搞指针，不传，不用==
      std::vector<double> *diff_list;

      double sharp_radio;
      double mean_value;//平均值
      double standard_deviation;

      std::vector<std::pair<double, std::string>> top_deficit_vector;
      std::vector<std::pair<double, std::string>> top_profit_vector;
    };
}






namespace order_strategy_name {
  extern const char kStop[];
  //extern const char kDefaultOS[];
  extern const char kPursuePrice[];
  extern const char kFixPrice[];
}


namespace order_execute_error_strings {
  extern const char kNoError[];
  extern const char kPartOK[];
  extern const char kNoExecute[];
  extern const char kZeroVolumn[];
  extern const char kZeroVolumnC4O[];
  extern const char kZeroVolumnMax[];
  extern const char kMaxVolumn[];
  extern const char kMSTimeout[];
  extern const char kParamsError[];
  extern const char kAddQueue[];
  extern const char kStartTrade[];
  extern const char kTradeFailed[];
  extern const char kTradeTimeout[];
  extern const char kTradeException[];


  //==============================
  extern const char kGiveupOrderAgain[];
  extern const char kNotQueueing[];
}