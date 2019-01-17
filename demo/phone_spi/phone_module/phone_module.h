#pragma once
#pragma warning (disable:4819)

#include <string>
#include <map>

#include "base/memory/scoped_ptr.h"
#include "content/public/browser/notification_observer.h"
#include "content/public/browser/notification_registrar.h"

#include "phone_common/include/ctp_type.h"
#include "phone_common/common/thread_message_filter.h"
#include "phone_common/include/ctp_messages.h"

#include "phone_common/channel/codec/codec_util.h"
#include "common.h"


class PrefService;
class CAdbInterface;


namespace channel {
  class ChannelHost;
}




namespace phone_module {

  class CTPStoreImpl;
  class PythonAdbInterface;
  class CTPModule:  public IModule,
                              //public channel::ChannelHost::Delegate,
                              public content::NotificationObserver,
                              public ThreadMessageFilter {
  public:
    friend class PythonAdbInterface;
    CTPModule();
    ~CTPModule();
    
    virtual void Load(IModuleManager*);
    virtual void Unload();
    virtual wchar_t const * const GetModuleName();
    //virtual int32 Invoke(Param const, Param lparam);

    virtual bool OnMessageReceived(IPC::Message const & message) OVERRIDE;

    virtual void Observe(int type,
      const content::NotificationSource& source,
      const content::NotificationDetails& details) OVERRIDE;

    ModuleID GetModuleID();
    IModuleManager * GetModuleManager() { return module_manager_; }

    PrefService* local_state() { return local_state_; }
    //CTPStoreImpl* DB() { return ctp_store_impl_.get(); }

    void DispatchProtobufMessage(std::string const & name, codec::MessagePtr const &, base::Time const&);
    void DispatchTLVMessage(std::string const & name, codec::TLVMessagePtr const & p, base::Time const&);

    void SendTLVMsg(codec::TLVMessagePtr const & p);

    typedef std::map<std::string, phone_module::TradeUnitInfoCommon> TUCMap;

  private:
    IModuleManager* module_manager_;
   
    //bool Process(Param const wparam, Param lparam);

    //message
    void OnCreateTradeUnit(std::string const & bc);
    void OnDestroyTradeUnit(int32 const routing_id);

    

private:



    int32 routing_id_seed_;
    int32 routing_id_seed() { return routing_id_seed_++; }


    std::set<std::string> keys_;

    
  public:
    //MarketQuotationBase* MQ() { return market_quotation_base_.get(); }
    channel::ChannelHost* channel_host() { return channel_host_.get(); }
    //backtesting::BacktestingModule* BM() { return backtesting_module_.get(); }
    void RlangHeartbeat();
    void OnPlaySound(uint32 const v);
    //void OnLoginAll();
	void ScanDevices();
	void InitConnectedDevice();
	void OnRefresh(std::wstring const & id);
	void OnDeviceChange(int const);
	void OnGetPackageList(std::wstring const &);
	void OnGetLocalInstallApkList();
  void OnGetNetInstallApkList();
	void OnInstallApkList(std::wstring const & type, PointerWrapper<std::vector<phone_module::ApkInstallInfo>> const & p);
	void OnCheckUpdateApkList();
  void OnRemoveLocalInstallApkList();
  void OnLaunchPY();
  void OnAutoInstall(bool b);
  void OnTotalAutoInstall(bool b);
  void OnInnerTotalAutoInstall(bool b);
  void OnModeChange(int mode);
  void OnGetVerifyCode(std::wstring const & phone_number);
  void OnLogin(std::wstring const & phone_number, std::wstring const & code);
  void NotifyApkCheckUpdateInfo(PointerWrapper<ApkUpdateInfo> &tmp);
  void NotifyGetApkList(PointerWrapper<std::vector< ApkInstallInfo>> &tmp);
  private:
    //scoped_ptr<MarketQuotationBase> market_quotation_base_;

    //market_strategy::MarketStrategy* MS() { return market_strategy_.get(); }
    //scoped_ptr<market_strategy::MarketStrategy> market_strategy_;

    
    //scoped_ptr<CTPStoreImpl> ctp_store_impl_;
    
    PrefService* local_state_;

    int32 server_port_;
    scoped_ptr<channel::ChannelHost> channel_host_;
    

    scoped_ptr<VoidVoidCallback> keep_connect_rlang_;

    bool python_connected_;

    FilePath exe_dir_;
    std::wstring apk_dir_;
    bool pending_total_auto_;
    bool total_param_;
    int now_mode_;

    //scoped_ptr<backtesting::BacktestingModule> backtesting_module_;
    

    //message


    void OnQuitR();

    void CloseRlangHeartbeat();
    void SendRlangHeartbeat();

    int cmd_no() {
      static int no = 1;
      return no++;
    }

    std::set<int> current_cmd_no_set_;


	//phone begin here
	private:
		scoped_ptr<CAdbInterface> adb_;
    scoped_ptr<PythonAdbInterface> py_adb_;

    content::NotificationRegistrar registrar_;

    DISALLOW_COPY_AND_ASSIGN(CTPModule);

  };


  //util
  std::string TradeUnitKey2DBKey(std::string const & k);
}

