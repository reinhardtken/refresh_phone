#include "common_main_parts.h"

#include <windows.h>
#include <tchar.h>
#include <shlobj.h>

#include "base/at_exit.h"
#include "base/command_line.h"
#include "base/message_loop.h"
#include "base/memory/scoped_ptr.h"
#include "base/time.h"
#include "base/threading/thread_restrictions.h"
#include "base/win/scoped_com_initializer.h"
#include "base/path_service.h"
#include "base/file_path.h"
#include "base/utf_string_conversions.h"

#include "content/public/common/content_switches.h"

//#include "common_logging.h"
#include "common_common.h"
//#include "common_window.h"
#include "common_thread.h"
#include "common_process.h"
#include "result_codes.h"
#include "common_ui.h"
#include "command_parser.h"
#include "pref_service.h"
#include "paths.h"
//#include "elevate.h"
#include "phone_common/system_monitor/removable_device_notifications_window_win.h"

namespace {

  HANDLE CreateMutexWrapper(CommonMainParams params) {
    HANDLE hMutex = NULL;
    std::string process_type = params.command_line.GetSwitchValueASCII(switches::kProcessType);
    //目前c++有至多四个（一个tu实例，将来考虑改多个）进程，guard，mq，tu，test

    if (process_type.size() == 0) {
      hMutex = CreateMutex(NULL, TRUE, L"phone-{0C731CBE-D8C1-4988-94FF-11E1D7839D55}");
    } else {
      CHECK(false);
    } 
    //else if (process_type == switches::kProcessTypeMQ) {
    //  hMutex = CreateMutex(NULL, TRUE, L"ctp-{2707F163-1D51-4FAB-9CA0-6D86AF2C8137}");
    //} else if (process_type == switches::kProcessTypeTU || process_type == switches::kProcessTypeMQTU) {
    //  hMutex = CreateMutex(NULL, TRUE, L"ctp-{3AFB3935-CCCD-499F-AD03-08D556D1045C}");
    //} else if (process_type == switches::kProcessTypeTest) {
    //  hMutex = CreateMutex(NULL, TRUE, L"ctp-{E7DC2015-66EF-45A3-8CB8-A305939BEE96}");
    //} else if (process_type == switches::kProcessTypeBacktesting) {
    //  hMutex = CreateMutex(NULL, TRUE, L"ctp-{FB1B23BB-8029-4C7F-AA36-81A99F8AECF3}");
    //}

    return hMutex;
  }

  //pref===========================================
    void RegisterLocalState(PrefService* local_state) {
    //server

  local_state->RegisterStringPref(prefs::kCheckUpdateApkList, "127.0.0.1:7904");
    
  }

  void CreatePrefService(std::string const &process_type, CommonThread::ID const id) {
    DCHECK(CommonThread::CurrentlyOn(id));
    common::RegisterPathProvider();
    FilePath local_state_path;
    PathService::Get(common::CTP_DATA, &local_state_path);
    std::wstring filename_tail = ASCIIToWide(process_type);
    if (filename_tail.size() == 0) {
      filename_tail = ASCIIToWide(switches::kProcessTypeGuard);
    }
    std::wstring filename = L"local_state_";
    filename.append(filename_tail);
    local_state_path = local_state_path.Append(filename);
    

    common_util::g_pref_service = PrefService::CreatePrefService(local_state_path, NULL, NULL,
      false);

    RegisterLocalState(common_util::g_pref_service);
  }

  void DestroyPrefService(CommonThread::ID const id) {
    DCHECK(CommonThread::CurrentlyOn(id));
    if (common_util::g_pref_service) {
      delete common_util::g_pref_service;
      common_util::g_pref_service = NULL;
    }
  }

  bool HaveCTPModule(std::string const & process_type) {
    return true;
    //if (process_type == switches::kProcessTypeMQ || 
    //  process_type == switches::kProcessTypeTU || 
    //  process_type == switches::kProcessTypeMQTU ||
    //  process_type == switches::kProcessTypeBacktesting) {
    //    return true;
    //} else {
    //  return false;
    //}
  }


}


CommonMainParts* CommonMainParts::CreateCommonMainParts(
  const CommandLine& command_line) {
    return new CommonMainParts(command_line);
}

CommonMainParts::CommonMainParts(const CommandLine& command_line)
:parsed_command_line_(command_line) {
  thread_message_dispatcher_impl_.reset(ThreadMessageDispatcherImpl::Create());
}

CommonMainParts::~CommonMainParts() {
}

void CommonMainParts::EarlyInitialization() {
  PreEarlyInitialization();

  //使用HighRes Clock
  base::Time::EnableHighResolutionTimer(true);

  PostEarlyInitialization();
}

void CommonMainParts::MainMessageLoopStart() {
  PreMainMessageLoopStart();
 
  main_message_loop_.reset(new MessageLoop(MessageLoop::TYPE_UI));
  InitializeMainThread();

  PostMainMessageLoopStart();
}

void CommonMainParts::InitializeMainThread() {
  const char* kThreadName = "Common_MainThread";
  base::PlatformThread::SetName(kThreadName);
  main_message_loop().set_thread_name(kThreadName);

  // Register the main thread by instantiating it, but don't call any methods.
  // 有点意思=
  main_thread_.reset(new CommonThread(CommonThread::UI,
    MessageLoop::current()));
}

void CommonMainParts::PreMainMessageLoopStart() {
	removable_device_notifications_window_.reset(
		chrome::RemovableDeviceNotificationsWindowWin::Create());
}

void CommonMainParts::PreMainMessageLoopRun() {
	removable_device_notifications_window_->Init();
}

//////////////////////////////////////////////////////////////////////////

namespace {
  //注意顺序，这里是柱塞调用，不能在dllmain里面=
  void CreateChildThreads(CommonProcess* process) {
    process->io_thread();
    process->db_thread();
    process->file_thread();
    process->ctp_thread();
    process->omq_thread();
    //process->launcher_thread();
  }

  void Shutdown() {
    base::ThreadRestrictions::SetIOAllowed(true);

    delete g_common_process;
    g_common_process = NULL;
  }

  void RunUIMessageLoop(CommonProcess* common_process) {
    base::ThreadRestrictions::SetIOAllowed(false);
    MessageLoopForUI::current()->Run();
  }

}

//////////////////////////////////////////////////////////////////////////


int CommonMain(CommonMainParams params)
{

  //test
  //CommandParser parser = CommandParser::FromString(L"login testaccount -accout=hello -passowrd=world");
  //FilePath cmd = parser.GetCommand();
  //parser = CommandParser::FromString(L"logout -accout=\"hello\" ");
  //cmd = parser.GetCommand();

  //互斥=
  HANDLE hMutex = CreateMutexWrapper(params);
  if (hMutex == NULL) {
    ::MessageBox(NULL, L"ctp启动失败", L"创建互斥失败", MB_OK);
    return 0;
  } else {
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
      ::MessageBox(NULL, L"ctp启动失败", L"唯一实例已经存在", MB_OK);
      CloseHandle(hMutex);
      return 0;
    }
  }
     
  


  //main thread
  scoped_ptr<CommonMainParts> parts(CommonMainParts::CreateCommonMainParts(params.command_line));
  parts->EarlyInitialization();
  parts->MainMessageLoopStart();

  //sub threads
  scoped_ptr<CommonProcess> common_process(new CommonProcess(params.command_line));
  DCHECK(g_common_process);
  CommonUI common_ui;
  CreateChildThreads(common_process.get());

  //create modulemanager
  std::string const process_type = params.command_line.GetSwitchValueASCII(switches::kProcessType);

  //PrefService
  //因为存在线程问题，约定有ctp的模块只能在ctp线程访问pref
  //没有ctp模块只能在ui访问pref
  CommonThread::ID id;
  //if (process_type.size() ==0 || process_type == switches::kProcessTypeGuard || process_type == switches::kProcessTypeTest) {
  //  id = CommonThread::UI;
  //  CreatePrefService(process_type, id);
  //} else if (HaveCTPModule(process_type)) {
  //  id = CommonThread::CTP;
  //  CommonThread::PostTask(id,
  //    FROM_HERE,
  //    base::Bind(&CreatePrefService, process_type, id));
  //}

  id = CommonThread::CTP;
  CommonThread::PostTask(id,
    FROM_HERE,
    base::Bind(&CreatePrefService, process_type, id));
  

  



  //只有mq和tu进程启动ctp模块=
  if (HaveCTPModule(process_type)) {
    common_process->CreateMM();
  }


  parts->PreMainMessageLoopRun();
	  


  

  //COM
  base::win::ScopedCOMInitializer com_initializer;
  //http://msdn.microsoft.com/en-us/library/windows/desktop/ms678405(v=vs.85).aspx
  /**
  Note  If you use CoInitialize or CoInitializeEx instead of OleInitialize to initialize COM, RegisterDragDrop will always return an E_OUTOFMEMORY error.
  */
  ::OleInitialize(NULL);

  //ui
  int result_code = ResultCodes::NORMAL_EXIT;
  if(common_ui.Start(params)){
    RunUIMessageLoop(common_process.get());
  }

  //showdown
  if (HaveCTPModule(process_type)) {
    common_process->DestroyMM();
  }

  //if (process_type.size() ==0 || process_type == switches::kProcessTypeGuard || process_type == switches::kProcessTypeTest) {
  //  id = CommonThread::UI;
  //  DestroyPrefService(id);
  //} else if (HaveCTPModule(process_type)) {
  //  id = CommonThread::CTP;
  //  CommonThread::PostTask(id,
  //    FROM_HERE,
  //    base::Bind(&DestroyPrefService, id));
  //}

  id = CommonThread::CTP;
  CommonThread::PostTask(id,
    FROM_HERE,
    base::Bind(&DestroyPrefService, id));
  

  ignore_result(common_process.release());
  Shutdown();


  ::OleUninitialize();

  //互斥=
  if(NULL != hMutex){
    CloseHandle(hMutex);
  }

  return result_code;
}





//////////////////////////////////////////////////////////////////////////