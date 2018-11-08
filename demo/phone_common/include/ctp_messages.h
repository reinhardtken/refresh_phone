#pragma once

#include <string>
#include <map>
#include <utility>


#include "base/time.h"

#include "ipc/ipc_message_macros.h"

#include "../../phone_spi/phone_module/common_struct_define.h"
#include "../../phone_spi/ctp_api/ThostFtdcUserApiStruct.h"     
//#include "../../phone_spi/order_execute/order_deque.h"
#include "../../phone_spi/adbtool/adb_define.h"
//相当于scoped_ptr，=
//WriteParam导致放弃生命所有权，这样marsh的时候，关联的指针对象不释放，在Unmarsh的时候重新掌管起生命周期=
//只用于跨线程的message传递，不能用于跨进程的情形=
//不能用于多线程并发=
//问题：如果这个message没处理，内存就泄漏了。毕竟如果td被干掉了，消息就可能没处理，而这几乎是必然会发生的事情
//替代方案,对于PointerWrapper，有个mock的处理handle，用于释放内存，但这样就ugly了，因为代码写在两个地方，很难确保没有遗漏
template<typename T>
class PointerWrapper {
public:
  COMPILE_ASSERT(std::is_pointer<T>::value == false &&
    std::is_array<T>::value == false, must_value_not_pointer);

  PointerWrapper(T * t)
    :v(t) {}
  PointerWrapper()
    :v(NULL) {}


  /*T& operator*() {
    return *v;
  }*/
  T* operator->() {
    return v;
  }
  T* get() const { return v; }
  T* release() { 
    T * temp;
    temp = v;
    v = NULL;
    return temp; 
  }
  void reset(T* p = NULL) { v = p; }

  ~PointerWrapper() {
    if (v) {
      delete v;
      v = NULL;
    }
  }
private:
  T * v;
};

namespace IPC {
  template <typename T>
  struct ParamTraits<PointerWrapper<T> > {
    COMPILE_ASSERT(sizeof(int *)  == sizeof(int), int_size_not_equal_intpointer_size);
    typedef PointerWrapper<T> param_type;
    static void Write(Message* m, const param_type& p) {
      WriteParam(m, reinterpret_cast<int>((const_cast<param_type*>(&p))->release()));
    }
    static bool Read(const Message* m, PickleIterator* iter, param_type* p) {
      int temp;
      if (!ReadParam(m, iter, &temp))
        return false;

      //p = new PointerWrapper<T>;
      p->reset(reinterpret_cast<T *>(temp));
      return true;
    }
    static void Log(const param_type& p, std::string* l) {
    }
  };
}



#define IPC_MESSAGE_START TestMsgStart


IPC_MESSAGE_CONTROL0(CTP_START)//broker+account
//IPC_SYNC_MESSAGE_CONTROL0_0(SyncChannelTestMsg_NoArgs)
//IPC_SYNC_MESSAGE_ROUTED0_0(SyncRoutedTestMsg_NoArgs)
//IPC_MESSAGE_CONTROL0(ChannelTestMsg_NoArgs)
//IPC_MESSAGE_ROUTED0(RoutedTestMsg_NoArgs)

//time delta action data pid tid
//IPC_MESSAGE_CONTROL5(ProfilerAction,
//                     int64,int64,int64,
                     //string16,string16)

//pid
//IPC_MESSAGE_CONTROL1(ProfilerChannel,
//                     int)



//ctp
IPC_MESSAGE_CONTROL1(CTP_CreateTradeUnit,
                    std::string)//broker+account

IPC_MESSAGE_CONTROL2(CTP_TradeUnitCreated,
                    std::string,//broker+account
                    int32)//routing_id


IPC_MESSAGE_CONTROL1(CTP_TradeUnitCreatedNew,
                    PointerWrapper<phone_module::TradeUnitInfo>)


IPC_MESSAGE_CONTROL1(CTP_DestroyTradeUnit,
                    int32)//routine_id


IPC_MESSAGE_CONTROL1(CTP_ClearList,
                    int32)//0 mq, 1 order, 2 pos


IPC_MESSAGE_CONTROL4(CTP_MessageBroadcast,
                    int32,//ui show type
                    int32,//NotificationType
                    base::Time,
                    PointerWrapper<void>)



IPC_MESSAGE_CONTROL4(CTP_MQLevel2UI,
                    int32,//ui show type
                    int32,//NotificationType
                    base::Time,
                    PointerWrapper<phone_module::LevelQuotationInfoBig>)

IPC_MESSAGE_CONTROL2(CTP_OHLC2UI,
                  std::string,
                  PointerWrapper<phone_module::OHLCT>)

IPC_MESSAGE_CONTROL2(CTP_MQConnectStatus2UI,
                    std::string,//name
                    bool)

IPC_MESSAGE_CONTROL1(CTP_MS2UI,
                    PointerWrapper<phone_module::MarketStrategyOrder>)

//这个要用router，先跑通
IPC_MESSAGE_CONTROL2(CTP_Position2UI,
                    std::string ,
                    PointerWrapper<phone_module::PositionData>)

IPC_MESSAGE_CONTROL2(CTP_StopPrice2UI,
                    std::string ,
                    PointerWrapper<phone_module::StopPriceData>)

//IPC_MESSAGE_CONTROL2(CTP_Cash2UI,
//                    std::string ,
//                    PointerWrapper<CThostFtdcTradingAccountField>)


IPC_MESSAGE_CONTROL2(CTP_Order2UI,
                    std::string ,
                    PointerWrapper<phone_module::OrderFootprints::OneRecord>)

//IPC_MESSAGE_CONTROL2(CTP_OrderResult2UI,
//                    std::string ,
//                    PointerWrapper<order_execute::OrderUnit2UI>)

IPC_MESSAGE_CONTROL0(CTP_MQ_Login_Success_2UI)
IPC_MESSAGE_CONTROL2(CTP_MQ_Login_Failed_2UI,
                    uint32,
                    std::string)
IPC_MESSAGE_CONTROL0(CTP_MQ_Logout_2UI)


//==================================================
IPC_MESSAGE_CONTROL1(CTP_MQLevelParam,
                    std::string)//type
//can not compile i do not know why...
//PointerWrapper<std::map<std::string,
                    //                            std::string>>
                    //PointerWrapper<std::map<std::string,
                    //                            std::string,
                    //                            std::less<std::string>,
	                  //                            std::allocator<std::pair<const std::string, std::string> > > > >)


    
//struct CThostFtdcRspInfoField;                 
//IPC_MESSAGE_CONTROL5(CTP_NetWork_Callback,
//                    int32,//routing id
//                    int32,
//                    //tricky
//                    PointerWrapper<void>,
//                    PointerWrapper<CThostFtdcRspInfoField>,
//                    std::vector<int>)//there is no IPC_MESSAGE_CONTROL6,why std::pair<bool,int>also can't work??
//                    //int,
//                    //bool)


//tu
IPC_MESSAGE_ROUTED1(CTP_TU_Login,
                        std::string)
IPC_MESSAGE_ROUTED1(CTP_TU_Logout,
                        std::string)
IPC_MESSAGE_ROUTED1(CTP_TU_Login_Success_2UI,
                        std::string)
IPC_MESSAGE_ROUTED3(CTP_TU_Login_Failed_2UI,
                        std::string,
                        uint32,
                        std::string)

IPC_MESSAGE_ROUTED1(CTP_TU_Logout_Success_2UI,
                        std::string)
IPC_MESSAGE_ROUTED1(CTP_TU_Logout_Failed_2UI,
                        std::string)
IPC_MESSAGE_ROUTED1(CTP_TU_Disconnect_2UI,
                        std::string)
IPC_MESSAGE_ROUTED2(CTP_TU_Allow_MS,
                        std::string,
                        bool)
IPC_MESSAGE_ROUTED1(CTP_TU_Query_Position,
                        std::string)
//mq
IPC_MESSAGE_ROUTED0(CTP_MQLogin)//routine_id must be zero
IPC_MESSAGE_ROUTED0(CTP_MQLogout)

IPC_MESSAGE_ROUTED1(CTP_MarginRate2UI,
              PointerWrapper<CThostFtdcInstrumentMarginRateField>)
IPC_MESSAGE_ROUTED1(CTP_CommissionRate2UI,
              PointerWrapper<CThostFtdcInstrumentCommissionRateField>)
//msm
//IPC_MESSAGE_ROUTED0(CTP_MSM_Connect)
//IPC_MESSAGE_ROUTED0(CTP_MSM_Disconnect)

//dc
IPC_MESSAGE_ROUTED0(CTP_DC_QryPositionList)


IPC_MESSAGE_ROUTED0(CTP_TU_Open)
IPC_MESSAGE_ROUTED1(CTP_TU_Close,
                        uint32)//数量

IPC_MESSAGE_ROUTED4(CTP_TU_OpenCloseNew,
                        std::string,//品种
                        int32,//开平
                        int32,//多空
                        uint32)//数量

IPC_MESSAGE_ROUTED0(CTP_TU_QryTypeList)

IPC_MESSAGE_CONTROL2(CTP_TU_TypeList,
                        std::string,
                        std::vector<std::string>)
//gdc

//mq
//IPC_MESSAGE_ROUTED1(CTP_GDC_MQ,
//                    PointerWrapper<CThostFtdcDepthMarketDataField>)


IPC_MESSAGE_CONTROL2(CTP_BacktestingCreated,
            std::string,
            int32)

IPC_MESSAGE_CONTROL1(CTP_BacktestingInit,
          int32)
IPC_MESSAGE_CONTROL0(CTP_BacktestingStart)
IPC_MESSAGE_CONTROL0(CTP_BacktestingPause)
IPC_MESSAGE_CONTROL0(CTP_BacktestingGiveup)
IPC_MESSAGE_CONTROL0(CTP_BacktestingDefaultExpectInit)
IPC_MESSAGE_CONTROL0(CTP_BacktestingPropExpectInit)
IPC_MESSAGE_CONTROL2(CTP_BacktestingResult2DB,
                    std::string,
                    bool)

IPC_MESSAGE_ROUTED4(CTP_BacktestingInitProgress2UI,
                    double,
                    double,
                    double,
                    double)
IPC_MESSAGE_ROUTED1(CTP_BacktestingRunProgress2UI,
                    PointerWrapper<backtesting::DataCenter>)
IPC_MESSAGE_ROUTED2(CTP_BacktestingExpectInitProgress2UI,
                    double,
                    double)
IPC_MESSAGE_ROUTED2(CTP_BacktestingResult2DBProgress2UI,
                    double,
                    double)



IPC_MESSAGE_CONTROL1(CTP_PlaySound,
                    uint32)


IPC_MESSAGE_CONTROL0(CTP_LoginAll)

//console
IPC_MESSAGE_CONTROL0(CTP_CONSOLE_Hello)
IPC_MESSAGE_CONTROL0(CTP_CONSOLE_HelloAck)

IPC_MESSAGE_CONTROL1(CTP_CONSOLE_Command,
                    std::vector<std::string>)
IPC_MESSAGE_CONTROL1(CTP_CONSOLE_CommandProtoBuf,
                    std::string)

IPC_MESSAGE_CONTROL1(CTP_CONSOLE_CommandRsp,
                    std::vector<std::string>)

IPC_MESSAGE_CONTROL0(CTP_QUIT_R)




IPC_MESSAGE_ROUTED1(CTP_CONSOLE_RoutedCommand,
                    std::vector<std::string>)

IPC_MESSAGE_ROUTED1(CTP_CONSOLE_RoutedCommandRsp,
                    std::vector<std::string>)


//phone begin here ========================================
//U2L ui to logic
//L2U logic to ui

IPC_MESSAGE_CONTROL1(U2L_Reconnect, std::string)//设备id
IPC_MESSAGE_CONTROL1(U2L_Refresh, std::string)//设备id
IPC_MESSAGE_CONTROL2(L2U_AdbInfo, std::string, std::wstring)//设备id
IPC_MESSAGE_CONTROL1(U2L_DeviceChange, int)//设备id


IPC_MESSAGE_CONTROL2(L2U_PackageList, std::wstring, 
	PointerWrapper<std::vector<ANDROID_PACKAGE_T>>)

IPC_MESSAGE_CONTROL0(U2L_GetLocalInstallApkList)//获取本地包列表
IPC_MESSAGE_CONTROL0(U2L_RemoveLocalInstallApkList)//删除本地包列表
IPC_MESSAGE_CONTROL0(U2L_GetNetInstallApkList)//获取服务器包列表
IPC_MESSAGE_CONTROL2(L2U_GetInstallApkListRsp, std::wstring, std::vector<std::wstring>)//设备id


IPC_MESSAGE_CONTROL0(U2L_CheckUpdateApkList)
IPC_MESSAGE_CONTROL1(L2U_DeviceUpdate,
  PointerWrapper<phone_module::DeviceData>)
IPC_MESSAGE_CONTROL1(L2U_StatusInfo,
  PointerWrapper<phone_module::StatusInfo>)
IPC_MESSAGE_CONTROL1(L2U_ApkUpdateInfo,
  PointerWrapper<phone_module::ApkUpdateInfo>)
IPC_MESSAGE_CONTROL1(L2U_ApkInstallInfo,
  PointerWrapper<std::vector<phone_module::ApkInstallInfo>>)
IPC_MESSAGE_CONTROL1(U2L_ApkInstallCmd, PointerWrapper<std::vector<phone_module::ApkInstallInfo>>)//设备id
IPC_MESSAGE_CONTROL0(U2L_LaunchPY)//  
IPC_MESSAGE_CONTROL1(L2U_DevicesList,
  PointerWrapper<phone_module::DevicesList>)
                          

//must last one
IPC_MESSAGE_ROUTED0(CTP_LAST)