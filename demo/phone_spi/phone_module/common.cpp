#include "common.h"


#include "base/string_number_conversions.h"
#include "base/string_util.h"
#include "content/browser/notification_service_impl.h"

#include "../../phone_common/include/ctp_define.h"
#include "../../phone_common/include/ctp_notification_types.h"
#include "../../phone_common/include/ctp_messages.h"
#include "../../phone_common/common/thread_message_filter.h"
#include "../../phone_common/common/common_util.h"
#include "../../phone_common/common/pref_service.h"
#include "../../phone_common/common/common_common.h"
#include "../../phone_common/channel/codec/codec_util.h"


PrefService * g_pref_service = NULL;

namespace phone_module {
  
  


	void NotifyMessageFunction(MessageType2UI const type, uint32 const id, SSMap const &m) {
		NotifyMessage nm;
    nm.type = type;
		nm.message_id = id;
		nm.msg_map = m;
		nm.time = base::Time::Now();
		content::NotificationService::current()->Notify(
			id,
			content::NotificationService::AllSources(),
			content::Details<NotifyMessage>(&nm));


    if (id > phone_module::NOTIFICATION_CTP_START &&
      id < phone_module::NOTIFICATION_CTP_END) {
/*
        ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::UI,
          new CTP_MessageBroadcast(type, id, base::Time::Now(), PointerWrapper<void>(new SSMap(m))));*/

        //to log 
        std::string s = base::StringPrintf("[type]=%d [id]=%s : %s", type, WideToASCII(NotificationTypeString(id)).c_str(), common_util::SSMap2String(m).c_str());
        LOG(INFO)<</*base::SysNativeMBToWide*/(s);
    }
	}

namespace ctp_prefs {
  //pref
  const char kMQBroker[] = "CTPM.MQ.broker";
  const char kMQAccount[] = "CTPM.MQ.account";
  const char kMQTHDCServer[] = "CTPM.MQ.server";
  const char kMQInstrmentList[] = "CTPM.MQ.instrument_list";
  const char kMQPassword[] = "CTPM.MQ.password";
  const char kMQMockMQ[] = "CTPM.MQ.mock_mq";
  const char kDBCreateTableListType[] = "CTPM.DB.create_table_list_type";
  const char kDBCreateTableListLevel[] = "CTPM.DB.create_table_list_level";
  const char kMQMemoryDB2File[] = "CTPM.DB.memorydb_2_file";
  const char kTUS[] = "CTPM.TUS";
  //tu的公共配置
  const char kTU_PROPS[] = "CTPM.TU_PROPS";
  //const char kTestAccountCash[] = "CTPM.TEST_ACCOUNT.cash";
  
}




std::string MakeupKey(std::string const & type, BuySellEnum const direction) {
  if (type.size() == 6 || type.size() == 5 || type == "CASH") {

    //G401, m1401
    if (type.size() == 5) {
      if (direction == BuyOrder) {
        return type + "_" + "L";
      } else {
        return type + "_" + "S";
      }
    } else if (type == "CASH") {
      DCHECK_RLOG(false);
      return "CASH+++";
    } else {
      if (direction == BuyOrder) {
        return type + "L";
      } else {
        return type + "S";
      }
    }
  } else {
    CHECK(false);
  }

  return "";
}

std::string MakeupKey(std::string const & type, TThostFtdcPosiDirectionType const direction) {
  if (direction == THOST_FTDC_PD_Long) {
    return MakeupKey(type, BuyOrder);
  } else if (direction == THOST_FTDC_PD_Short) {
    return MakeupKey(type, SellOrder);
  } else {
    DCHECK_RLOG(false);
  }

  return "";
}

std::string MakeupKey(std::string const & type, TLVLongShortEnum const direction) {
  if (direction == LongDirection) {
    return MakeupKey(type, BuyOrder);
  } else if (direction == ShortDirection) {
    return MakeupKey(type, SellOrder);
  } else {
    DCHECK_RLOG(false);
  }

  return "";
}

std::string Key2Type(std::string const & key) {
  if (key == "CASH+++") {
    return "CASH";
  } else {
    if (key.find("_") != std::string::npos) {
      return key.substr(0, 5);
    } else {
      return key.substr(0, 6);
    }
  }
}

TThostFtdcDirectionType DirectionEnum2TThostFtdcDirectionType(BuySellEnum const d) {
  return d;
}

BuySellEnum TThostFtdcPosiDirectionType22DirectionEnum(TThostFtdcPosiDirectionType const d) {
  DCHECK_RLOG(d == THOST_FTDC_PD_Long || d == THOST_FTDC_PD_Short);
  if (d == THOST_FTDC_PD_Long) {
    return BuyOrder;
  } else if (d == THOST_FTDC_PD_Short) {
    return SellOrder;
  } else {
    CHECK(false);
    return SellOrder;
  }
}


TThostFtdcDirectionType TThostFtdcDirectionType2CloseDirection(TThostFtdcDirectionType const d) {
  if (d == THOST_FTDC_D_Buy) {
    return THOST_FTDC_D_Sell;
  } else if (d == THOST_FTDC_D_Sell) {
    return THOST_FTDC_D_Buy;
  } else {
    CHECK(false);
  }

  return -1;
}

BuySellEnum TThostFtdcDirectionType2DirectionEnum(TThostFtdcDirectionType const d) {
  DCHECK_RLOG(d == THOST_FTDC_D_Buy || d == THOST_FTDC_D_Sell);
  return (BuySellEnum)d;
}


void scoped_ptr_malloc_donothing(void*) {
  //donothing
}


//因为存在杠杆，需要计算市场点数和真实开销之间的关系==
double MarketPrice2CostCash(std::string const & type, double const price, double const margin_rate) {
  //If的300，其余10
  uint32 const value = phone_module::GetPointFromType(type);
/*
  if (phone_module::IsSpecialType(type)) {
    value = 300;
  } else {
    value = 10;
  }*/
  double cost = price * (value * margin_rate);
  DCHECK_RLOG(cost != 0);
  return cost;
}

double CostCash2MarketPrice(std::string const & type, double const cash, double const margin_rate) {
  //If的300，其余10
  uint32 const value = phone_module::GetPointFromType(type);
/*
  if (phone_module::IsSpecialType(type)) {
    value = 300;
  } else {
    value = 10;
  }*/
  if (margin_rate != 0) {
    double const price = cash / (value * margin_rate);
    DCHECK_RLOG(price != 0);
    return price;
  } else {
    DCHECK_RLOG(false);
    return 0;
  }
}

double ProfitPerHand(std::string const & type, phone_module::BuySellEnum const direction, double const old_index, double const new_index) {
  uint32 const value = phone_module::GetPointFromType(type);
/*
  if (phone_module::IsSpecialType(type)) {
    value = 300;
  } else {
    value = 10;
  }*/

  double const profit_per_hand = value * (new_index - old_index);
  if (direction == phone_module::BuyOrder) {
    return profit_per_hand;
  } else if (direction == phone_module::SellOrder) {
    return 0 - profit_per_hand;
  } else {
    DCHECK_RLOG(false);
    return 0;
  }

}

double FeePerHand(std::string const & type, double const index, double const percent) {
  uint32 const value = phone_module::GetPointFromType(type);
/*
  if (phone_module::IsSpecialType(type)) {
    value = 300;
  } else {
    value = 10;
  }*/

  return value * index * percent;
}

double PoundCostPerHand(std::string const & type, double const percent) {
  uint32 const value = phone_module::GetPointFromType(type);
/*
  if (phone_module::IsSpecialType(type)) {
    value = 300;
  } else {
    value = 10;
  }*/

  return value * percent;
}


bool IsBacktestingMode(std::string const & process_type) {
  if (process_type == switches::kProcessTypeBacktesting) {
    return true;
  } else {
    return false;
  }
}

BuySellEnum BuySellEnumReverse(BuySellEnum const old) {
  if (old == BuyOrder) {
    return SellOrder;
  } else if (old == SellOrder) {
    return BuyOrder;
  } else {
    CHECK(false);
  }

  return BuyOrder;
}




std::string Enum2String(phone_module::MQLevel const l) {
    switch(l) {
      case phone_module::MQ1S: {
        return "MQ1S";
      }
      case phone_module::MQ5S: {
        return "MQ5S";
      }
      case phone_module::MQ1M: {
        return "MQ1M";
      }
      case phone_module::MQ5M: {
        return "MQ5M";
      }
      case phone_module::MQ15M: {
        return "MQ15M";
      }
      case phone_module::MQ1H: {
        return "MQ1H";
      }
      case phone_module::MQ1D: {
        return "MQ1D";
      }
      default: {
        DCHECK_RLOG(false);
        return "MQUNKNOWN";
      }
    }
  }

  phone_module::MQLevel const String2Enum(std::string const &from) {
    std::string s = from;
    std::transform(s.begin(), s.end(), s.begin(), ::toupper); 
    if (s =="MQ1S") {
      return phone_module::MQ1S;
    } else if (s =="MQ5S") {
      return phone_module::MQ5S;
    } else if (s =="MQ1M") {
      return phone_module::MQ1M;
    } else if (s =="MQ5M") {
      return phone_module::MQ5M;
    } else if (s =="MQ15M") {
      return phone_module::MQ15M;
    } else if (s =="MQ1H") {
      return phone_module::MQ1H;
    } else if (s =="MQ1D") {
      return phone_module::MQ1D;
    } else {
      DCHECK_RLOG(false);
      return phone_module::MQ_ALL;
    }
  }


  void TradeUnitInfo::Dump() const {
    LOG(INFO)<<"<TradeUnitInfo----begin===========================================>";
    LOG(INFO)<<"bc: "<<bc;
    LOG(INFO)<<"alias: "<<alias;
    LOG(INFO)<<"broker: "<<broker;
    LOG(INFO)<<"account: "<<account;
    LOG(INFO)<<"server_ip: "<<server_ip;
    LOG(INFO)<<"instrument_list: "<<instrument_list;
    LOG(INFO)<<"volumn_list: "<<common_util::ToString(volumn_list);
    LOG(INFO)<<"stop_list: "<<common_util::ToString(stop_list);
    LOG(INFO)<<"stop_win_list: "<<common_util::ToString(stop_win_list);
    LOG(INFO)<<"auto_close_list: "<<common_util::ToString(auto_close_list);
    LOG(INFO)<<"type_2_msname_list: "<<common_util::ToString(type_2_msname_list);
    LOG(INFO)<<"strategy: "<<common_util::ToString(strategy);
    LOG(INFO)<<"order_execute: "<<order_execute;
    LOG(INFO)<<"status: "<<status;
    LOG(INFO)<<"mode: "<<mode;
    LOG(INFO)<<"money: "<<money;
    LOG(INFO)<<"id: "<<id;
    LOG(INFO)<<"real_account: "<<real_account;
    LOG(INFO)<<"hands_of_money: "<<common_util::ToString(hands_of_money);
    LOG(INFO)<<"margin_rate: "<<common_util::ToString(margin_rate);
    LOG(INFO)<<"origin_money: "<<common_util::ToString(origin_money);
    LOG(INFO)<<"fee_list: "<<common_util::ToString(fee_list);
    LOG(INFO)<<"allow_diff: "<<common_util::ToString(allow_diff);
    LOG(INFO)<<"pound_cost: "<<common_util::ToString(pound_cost);
    LOG(INFO)<<"jump_close_and_open: "<<(jump_close_and_open == true ? true : false);
    LOG(INFO)<<"<TradeUnitInfo----end===========================================>";
  }



  bool DiscardLevelMQ(std::string const & type, std::string const & time_string, base::Time const & time, phone_module::MQLevel const level) {
    if (level == phone_module::MQ5M) {
      std::string search = "09:00:00";
      if (phone_module::IsSpecialType(type)) {
        search = "09:15:00";
      }
      
      
      if (time_string.find(search) != std::string::npos) {
        LOG(WARNING)<<"DiscardLevelMQ "<<level<<" "<<type<<" "<<time_string<<" "<<time.ToInternalValue();
      }

      if (EndsWith(time_string, search, true)) {
        LOG(WARNING)<<"DiscardLevelMQ "<<level<<" "<<type<<" "<<time_string<<" "<<time.ToInternalValue();
        return true;
      }

    }

    return false;
  }



  bool IsSpecialType(std::string const &type) {
    if (StartsWithASCII(type, "IF", true) == true ||
      StartsWithASCII(type, "IH", true) == true ||
      StartsWithASCII(type, "IC", true) == true) {
        return true;
    }

    return false;
  }

  bool IsSpecialTypeIgnoreCase(std::string const &type) {
    if (StartsWithASCII(type, "IF", false) == true ||
      StartsWithASCII(type, "IH", false) == true ||
      StartsWithASCII(type, "IC", false) == true) {
        return true;
    }

    return false;
  }

  int GetPointFromType(std::string const &type) {
    if (StartsWithASCII(type, "IF", false) == true ||
      StartsWithASCII(type, "IH", false) == true) {
        return 300;
    } else if (StartsWithASCII(type, "IC", false)) {
      return 200;
    } else {
      return 10;
    }
  }

}




namespace order_strategy_name {
  const char kStop[] = "os.stop";
  //const char kDefaultOS[] = "os.default";
  const char kPursuePrice[] = "os.pursueprice";
  const char kFixPrice[] = "os.fixprice";
}


namespace order_execute_error_strings {
  const char kNoError[] = "执行：完全交易";
  const char kPartOK[] = "执行：部分交易";
  const char kNoExecute[] = "执行：一手都未交易";
  const char kZeroVolumn[] = "未执行：目标交易手数为零";
  const char kZeroVolumnC4O[] = "未执行：为开仓平仓，没有反向持仓";
  const char kZeroVolumnMax[] = "未执行：持仓已达上限";
  const char kMaxVolumn[] = "未执行：持仓达到上限";
  const char kMSTimeout[] = "未执行：超过执行时间上限";
  const char kParamsError[] = "未执行：参数错误";
  const char kAddQueue[] = "准备：入队";
  const char kStartTrade[] = "执行：开始";
  const char kTradeFailed[] = "未执行：交易错误";
  const char kTradeTimeout[] = "未执行：交易超时";
  const char kTradeException[] = "异常！！！";
  const char kGiveupOrderAgain[] = "执行：策略放弃再次报单";
  const char kNotQueueing[] = "执行：NotQueueing";
}


namespace RCommand {
  
  const char CommandMS[] = "MS_DEMO";
  const char CommandPSP[] = "R2CTP.Position.StopPrice";

  const char NotifyMarketQuotation[] = "MQ";
  const char NotifyMarketQuotationV2[] = "MQ.V2";
  const char NotifyPositionCost[] = "CTP2R.Position.Cost";
  const char NotifyOrderStop[] = "Order.Stop";
  const char NotifyOrderStopWin[] = "Order.StopWin";
}





namespace version {
  const char kVersion[] = "1.0.1.140"; 
}