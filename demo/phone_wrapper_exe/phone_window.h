#pragma once

#include <atlbase.h>
#include <atlapp.h>
#include <atlctrls.h>

#include "base/memory/scoped_ptr.h"
#include "base/scoped_native_library.h"
#include "base/i18n/icu_util.h"
#include "ui/base/ui_base_paths.h"
#include "ui/base/win/window_impl.h"
#include "content/public/browser/notification_registrar.h"
#include "content/public/browser/notification_observer.h"
#include "content/browser/notification_service_impl.h"
#include "content/public/common/content_switches.h"
#include "ui/base/resource/resource_bundle.h"


//#include "log_daemon.h"
#include "../phone_common/common/common_thread.h"
//#include "profiler_data.h"
#include "../phone_common/include/ctp_define.h"
#include "../phone_common/common/common_main.h"
#include "../phone_common/common/thread_message_filter.h"
#include "../phone_common/include/ctp_messages.h"
#include "../phone_common/common/command_parser.h"
#include "../phone_common/common/common_common.h"
#include "./phone_view/examples_window.h"


#include "custom_list_view.h"
//////////////////////////////////////////////////////////////////////////

class CTPWindow : public ui::WindowImpl,
  public ThreadMessageFilter,
  public content::NotificationObserver {
public:
  static bool Start(CommandLine const & cmdline);
  CTPWindow();
  virtual ~CTPWindow();
  std::wstring Title() { return std::wstring(L"ctp window"); }
  
  //ProfilerData::Delegate
  virtual void OnAction(const std::wstring& time_ms,const std::wstring& delta_ms,
    const std::wstring& pid,const std::wstring& tid,
    const std::wstring& action,const std::wstring& data);

  virtual void Observe(int type,
    const content::NotificationSource& source,
    const content::NotificationDetails& details) OVERRIDE;

  virtual bool OnMessageReceived(IPC::Message const & message) OVERRIDE;

  void LoginLogoutPressTest(std::string const & bc);







  void OnMessage(phone_module::NotifyMessage const & m);
  void InitONFileThread();

  enum{
    kCommandTdLogin=1000,
    kCommandTdLogout,
    kCommandMdConnect,
    kCommandMdDisonnect,
    kCommandTdInit,
    kCommandDumpOpen,
    kCommandRunLow,
    kCommandLimitedJob,
    kCommandDebugRun,
    kCommandParserDump,
    kCommandListLog,
    kCommandListProc,
    kCommandListPos,
    kCommandTestBegin=2000,
    kCommandTestEnd=2100,
  };

  BEGIN_MSG_MAP(CTPWindow)
    MESSAGE_HANDLER(WM_CLOSE, OnClose)
    MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
    MESSAGE_HANDLER(WM_NCDESTROY,OnFinalMessage)
    MESSAGE_HANDLER(WM_CREATE,OnCreate)
    MESSAGE_HANDLER(WM_SIZE,OnSize)
    MESSAGE_HANDLER(WM_ERASEBKGND,OnEraseBkgnd)
    MESSAGE_HANDLER(WM_PAINT,OnPaint)
    MESSAGE_HANDLER(WM_COPYDATA,OnCopyData)
    MESSAGE_HANDLER(WM_TIMER, OnTimer)
    //MESSAGE_HANDLER(WM_SETCURSOR,OnSetCursor)
    COMMAND_HANDLER(kCommandTdLogin,BN_CLICKED,OnCommandTdLogin)
    COMMAND_HANDLER(kCommandTdLogout,BN_CLICKED,OnCommandTdLogout)
    
    COMMAND_HANDLER(kCommandMdConnect,BN_CLICKED,OnCommandMdConnect)
    COMMAND_HANDLER(kCommandTdInit,BN_CLICKED,OnCommandTdInit)
    COMMAND_HANDLER(kCommandDumpOpen,BN_CLICKED,OnCommandDumpOpen)
    COMMAND_HANDLER(kCommandRunLow,BN_CLICKED,OnCommandRunLow)
    COMMAND_HANDLER(kCommandLimitedJob,BN_CLICKED,OnCommandLimitedJob)
    COMMAND_HANDLER(kCommandDebugRun,BN_CLICKED,OnCommandDebugRun)
    COMMAND_HANDLER(kCommandParserDump,BN_CLICKED,OnCommandParserDump)
    COMMAND_RANGE_CODE_HANDLER(kCommandTestBegin,kCommandTestEnd,BN_CLICKED,OnCommandTest)
    CHAIN_MSG_MAP_MEMBER(list_mq_)
    CHAIN_MSG_MAP_MEMBER(list_trade_)
    CHAIN_MSG_MAP_MEMBER(list_pos_)
  END_MSG_MAP()

private:
  LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
  LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
  LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
  LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
  LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
  LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
  LRESULT OnFinalMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
  LRESULT OnCopyData(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
  LRESULT OnTimer(UINT msg, WPARAM wp, LPARAM lp, BOOL& handled);
  //LRESULT OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
  LRESULT OnCommandTdLogin(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
  LRESULT OnCommandTdLogout(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
  
  LRESULT OnCommandMdConnect(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
  LRESULT OnCommandMdDisconnect(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

  LRESULT OnCommandTdInit(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
  LRESULT OnCommandDumpOpen(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
  LRESULT OnCommandRunLow(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
  LRESULT OnCommandLimitedJob(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
  LRESULT OnCommandDebugRun(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
  LRESULT OnCommandParserDump(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
  LRESULT OnCommandTest(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
  
private:
  void BuildAppButton();
  //ProfilerData* GetProfilerData();

private:
  CButton btn_td_login_,btn_md_connect_,btn_dump_full_,btn_dump_open_;
  CButton btn_runlow_,btn_limitedjob_,btn_debugrun_,btn_parser_dump_;
  std::vector<CButton*> btn_test_;
  MyListView list_mq_;
  MyListView list_trade_;
  MyListView list_pos_;



  //base::ScopedNativeLibrary ctp_dll_;
  /*
  HWPtr hw_;
  InitTdPtr init_td_;
  LoginTdPtr login_td_;
  LoginAndJoinTdPtr login_and_join_td_;
  
  InitMdPtr init_md_;
  LoginMdPtr login_md_;
  LoginAndJoinMdPtr login_and_join_md_;
  */
  //scoped_ptr<LogDaemon> log_server_;

  //scoped_ptr<content::NotificationServiceImpl> notification_service_on_filethread_;
  void OnMessageBroadcast(int32 const type, int32 const id, base::Time const & time, PointerWrapper<void> const & p);
  void OnMessageBroadcastInner(int32 const type, int32 const id, base::Time const & time, std::map<std::string, std::string> & map_value);
  void OnConsoleCommand(std::vector<std::string> const & v);
  void OnConsoleCommandProtoBuf(std::string const &v);
  void OnTradeUnitCreated(std::string const& bc, int32 const routing_id);
  void OnClearList(int32 const );

  bool OverrideThreadForMessage(IPC::Message const& message,
    CommonThread::ID* thread);
  
  //content::NotificationRegistrar registrar_;
  int32 routing_id_;
  int32 routing_id() { return ++routing_id_; }

  //int32 temp_routing_id_;
  std::map<std::string, int32> bc_2_id_;
};



struct CtpMainDelegate : public CommonMainDelegate {
  virtual bool ShowUI(CommandLine const & c) {
    INITCOMMONCONTROLSEX config;
    config.dwSize = sizeof(config);
    config.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&config);

    //不调用下面的InitSharedInstanceWithLocale无法成功
    CHECK(icu_util::Initialize());
    //注册路径支持，不然会找不到pak文件=
    ui::RegisterPathProvider();
    ui::ResourceBundle::InitSharedInstanceWithLocale("zh-CN", NULL);

    //=========================================================
    //guard 一个ui，mq，tu，test另一个ui
    views::examples::ShowExamplesWindow(views::examples::QUIT_ON_CLOSE, c);
    return true;
    //return CTPWindow::Start(c);
  }
};