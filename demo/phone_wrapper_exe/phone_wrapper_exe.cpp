// ctp_wrapper_exe.cpp : Defines the entry point for the console application.
//

#include <windows.h>

#include "base/at_exit.h"
#include "base/command_line.h"
#include "base/logging.h"
#include "base/utf_string_conversions.h"
#include "content/public/common/content_switches.h"

#include "../phone_common/common/common_logging_new.h"
#include "../phone_common/common/common_platform.h"
#include "../phone_common/common/common_main.h"
#include "../phone_common/common/command_parser.h"
#include "../phone_common/common/common_exception_handler.h"
#include "../phone_common/common/common_common.h"

#include "../phone_wrapper_exe/phone_window.h"


#include <iostream>




int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine, int nCmdShow) 
{
  int argc = 0;
  char** argv = NULL;

  exception_handler()->SetExceptionHandler();

  common_platform::LowLevelInit(hInstance);

  //Console();

  base::AtExitManager exit_manager;
  CommandLine::Init(argc, argv);


  //最终多进程架构切换完成时，这段代码要去掉==
#if !defined(_DEBUG)
  // CommandLine& change_command_line = *CommandLine::ForCurrentProcess();
  // std::string const process_type = change_command_line.GetSwitchValueASCII(switches::kProcessType);
  // if (process_type.size() == 0) {
  //   change_command_line.AppendSwitchASCII(switches::kProcessType, switches::kProcessTypeMQTU);
  // }
#endif
  //==================


  const CommandLine& command_line = *CommandLine::ForCurrentProcess();
  CtpMainDelegate delegate;
  CommonMainParams cp(command_line, &delegate);


  logging::OldFileDeletionState file_state = logging::APPEND_TO_OLD_LOG_FILE;
  logging::InitCommonLogging(command_line, file_state);

  int exit_code = CommonMain(cp);

  logging::CleanupCommonLogging();
  common_platform::LowLevelShutdown();

  return exit_code;
}



//-20181029 有时候出，有时候不出，没搞懂
//af32b22735eb8c06d5b192bfdeb3d3cf6260b9c8   the commit try to solve the problem but not work on my mbp, don't know why
//https://blog.csdn.net/leehong2005/article/details/8613072
#ifdef _UNICODE
 #if defined _M_IX86
 #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
 #elif defined _M_IA64
 #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
 #elif defined _M_X64
 #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
 #else
 #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
 #endif
 #endif