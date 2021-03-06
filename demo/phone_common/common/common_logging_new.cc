// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "build/build_config.h"

// Need to include this before most other files because it defines
// IPC_MESSAGE_LOG_ENABLED. We need to use it to define
// IPC_MESSAGE_MACROS_LOG_ENABLED so render_messages.h will generate the
// ViewMsgLog et al. functions.
#include "ipc/ipc_message.h"

// On Windows, the about:ipc dialog shows IPCs; on POSIX, we hook up a
// logger in this file.  (We implement about:ipc on Mac but implement
// the loggers here anyway).  We need to do this real early to be sure
// IPC_MESSAGE_MACROS_LOG_ENABLED doesn't get undefined.
#if defined(OS_POSIX) && defined(IPC_MESSAGE_LOG_ENABLED)
#define IPC_MESSAGE_MACROS_LOG_ENABLED
#include "content/public/common/content_ipc_logging.h"
#define IPC_LOG_TABLE_ADD_ENTRY(msg_id, logger) \
    content::RegisterIPCLogger(msg_id, logger)
#include "chrome/common/all_messages.h"
#endif

#if defined(OS_WIN)
#include <windows.h>
#endif

#include <fstream>

#include "common_logging_new.h"

#include "base/command_line.h"
#include "base/compiler_specific.h"
#include "base/debug/debugger.h"
#include "base/environment.h"
#include "base/file_path.h"
#include "base/file_util.h"
#include "base/logging.h"
#include "base/path_service.h"
#include "base/string_number_conversions.h"
#include "base/string_util.h"
#include "base/stringprintf.h"
#include "base/threading/thread_restrictions.h"
#include "base/time.h"
#include "base/utf_string_conversions.h"
#include "content/public/common/content_switches.h"
#include "chrome/common/chrome_constants.h"
#include "chrome/common/chrome_paths.h"
#include "chrome/common/chrome_switches.h"
#include "chrome/common/env_vars.h"
#include "ipc/ipc_logging.h"

#if defined(OS_WIN)
#include "base/logging_win.h"
#include <initguid.h>
#endif

#include "common_common.h"

namespace {

// When true, this means that error dialogs should not be shown.
bool dialogs_are_suppressed_ = false;

// This should be true for exactly the period between the end of
// InitChromeLogging() and the beginning of CleanupChromeLogging().
bool chrome_logging_initialized_ = false;

// Set if we called InitChromeLogging() but failed to initialize.
bool chrome_logging_failed_ = false;

// This should be true for exactly the period between the end of
// InitChromeLogging() and the beginning of CleanupChromeLogging().
bool chrome_logging_redirected_ = false;

#if defined(OS_WIN)
// {7FE69228-633E-4f06-80C1-527FEA23E3A7}
// [9/16/2013 ken] 
//// {B55A5F03-C4A1-4BE3-A515-C2BD752C1A82}


/*const GUID kChromeTraceProviderName = {
    0x7fe69228, 0x633e, 0x4f06,
        { 0x80, 0xc1, 0x52, 0x7f, 0xea, 0x23, 0xe3, 0xa7 } };*/
  const GUID kChromeTraceProviderName = { 0xb55a5f03, 0xc4a1, 0x4be3, { 0xa5, 0x15, 0xc2, 0xbd, 0x75, 0x2c, 0x1a, 0x82 } };
#endif

#if defined(USE_LINUX_BREAKPAD) || defined(OS_MACOSX)
// Pointer to the function that's called by DumpWithoutCrashing() to dump the
// process's memory.
void (*dump_without_crashing_function_)() = NULL;
#endif

// Assertion handler for logging errors that occur when dialogs are
// silenced.  To record a new error, pass the log string associated
// with that error in the str parameter.
MSVC_DISABLE_OPTIMIZE();
void SilentRuntimeAssertHandler(const std::string& str) {
  base::debug::BreakDebugger();
}
void SilentRuntimeReportHandler(const std::string& str) {
}
#if defined(OS_WIN)
// Handler to silently dump the current process when there is an assert in
// chrome.
void DumpProcessAssertHandler(const std::string& str) {
  // Get the breakpad pointer from chrome.exe
  typedef void (__cdecl *DumpProcessFunction)();
  DumpProcessFunction DumpProcess = reinterpret_cast<DumpProcessFunction>(
      ::GetProcAddress(::GetModuleHandle(L"ctp.exe"),
                       "DumpProcess"));
  if (DumpProcess)
    DumpProcess();
}
#endif  // OS_WIN
MSVC_ENABLE_OPTIMIZE();

// Suppresses error/assertion dialogs and enables the logging of
// those errors into silenced_errors_.
void SuppressDialogs() {
  if (dialogs_are_suppressed_)
    return;

  logging::SetLogAssertHandler(SilentRuntimeAssertHandler);
  logging::SetLogReportHandler(SilentRuntimeReportHandler);

#if defined(OS_WIN)
  UINT new_flags = SEM_FAILCRITICALERRORS |
                   SEM_NOGPFAULTERRORBOX |
                   SEM_NOOPENFILEERRORBOX;

  // Preserve existing error mode, as discussed at http://t/dmea
  UINT existing_flags = SetErrorMode(new_flags);
  SetErrorMode(existing_flags | new_flags);
#endif

  dialogs_are_suppressed_ = true;
}

}  // anonymous namespace

namespace switches {
  const char kSilentDumpOnDCHECK[]            = "silent-dump-on-dcheck";
}

namespace logging {

LoggingDestination DetermineLogMode(const CommandLine& command_line) {
  // only use OutputDebugString in debug mode
#ifdef NDEBUG
  bool enable_logging = true;
  const char *kInvertLoggingSwitch = switches::kEnableLogging;
  const logging::LoggingDestination kDefaultLoggingMode =
      logging::LOG_ONLY_TO_FILE;
#else
  bool enable_logging = true;
  const char *kInvertLoggingSwitch = switches::kDisableLogging;
  const logging::LoggingDestination kDefaultLoggingMode =
      logging::LOG_TO_BOTH_FILE_AND_SYSTEM_DEBUG_LOG;
#endif

  if (command_line.HasSwitch(kInvertLoggingSwitch))
    enable_logging = !enable_logging;

  logging::LoggingDestination log_mode;
  if (enable_logging) {
    // Let --enable-logging=stderr force only stderr, particularly useful for
    // non-debug builds where otherwise you can't get logs to stderr at all.
    if (command_line.GetSwitchValueASCII(switches::kEnableLogging) == "stderr")
      log_mode = logging::LOG_ONLY_TO_SYSTEM_DEBUG_LOG;
    else
      log_mode = kDefaultLoggingMode;
  } else {
    log_mode = logging::LOG_NONE;
  }
  return log_mode;
}

#if defined(OS_CHROMEOS)
namespace {
FilePath SetUpSymlinkIfNeeded(const FilePath& symlink_path, bool new_log) {
  DCHECK(!symlink_path.empty());

  // If not starting a new log, then just log through the existing
  // symlink, but if the symlink doesn't exist, create it.  If
  // starting a new log, then delete the old symlink and make a new
  // one to a fresh log file.
  FilePath target_path;
  bool symlink_exists = file_util::PathExists(symlink_path);
  if (new_log || !symlink_exists) {
    target_path = GenerateTimestampedName(symlink_path, base::Time::Now());

    // We don't care if the unlink fails; we're going to continue anyway.
    if (::unlink(symlink_path.value().c_str()) == -1) {
      if (symlink_exists) // only warn if we might expect it to succeed.
        DPLOG(WARNING) << "Unable to unlink " << symlink_path.value();
    }
    if (!file_util::CreateSymbolicLink(target_path, symlink_path)) {
      DPLOG(ERROR) << "Unable to create symlink " << symlink_path.value()
                   << " pointing at " << target_path.value();
    }
  } else {
    if (!file_util::ReadSymbolicLink(symlink_path, &target_path))
      DPLOG(ERROR) << "Unable to read symlink " << symlink_path.value();
  }
  return target_path;
}

void RemoveSymlinkAndLog(const FilePath& link_path,
                         const FilePath& target_path) {
  if (::unlink(link_path.value().c_str()) == -1)
    DPLOG(WARNING) << "Unable to unlink symlink " << link_path.value();
  if (::unlink(target_path.value().c_str()) == -1)
    DPLOG(WARNING) << "Unable to unlink log file " << target_path.value();
}

}  // anonymous namespace

FilePath GetSessionLogFile(const CommandLine& command_line) {
  FilePath log_dir;
  std::string log_dir_str;
  scoped_ptr<base::Environment> env(base::Environment::Create());
  if (env->GetVar(env_vars::kSessionLogDir, &log_dir_str) &&
      !log_dir_str.empty()) {
    log_dir = FilePath(log_dir_str);
  } else {
    PathService::Get(chrome::DIR_USER_DATA, &log_dir);
    FilePath login_profile =
        command_line.GetSwitchValuePath(switches::kLoginProfile);
    log_dir = log_dir.Append(login_profile);
  }
  return log_dir.Append(GetLogFileName(command_line).BaseName());
}

void RedirectChromeLogging(const CommandLine& command_line) {
  DCHECK(!chrome_logging_redirected_) <<
    "Attempted to redirect logging when it was already initialized.";

  // Redirect logs to the session log directory, if set.  Otherwise
  // defaults to the profile dir.
  FilePath log_path = GetSessionLogFile(command_line);

  // Creating symlink causes us to do blocking IO on UI thread.
  // Temporarily allow it until we fix http://crbug.com/61143
  base::ThreadRestrictions::ScopedAllowIO allow_io;
  // Always force a new symlink when redirecting.
  FilePath target_path = SetUpSymlinkIfNeeded(log_path, true);

  logging::DcheckState dcheck_state =
      command_line.HasSwitch(switches::kEnableDCHECK) ?
      logging::ENABLE_DCHECK_FOR_NON_OFFICIAL_RELEASE_BUILDS :
      logging::DISABLE_DCHECK_FOR_NON_OFFICIAL_RELEASE_BUILDS;

  // ChromeOS always logs through the symlink, so it shouldn't be
  // deleted if it already exists.
  if (!InitLogging(log_path.value().c_str(),
                   DetermineLogMode(command_line),
                   logging::LOCK_LOG_FILE,
                   logging::APPEND_TO_OLD_LOG_FILE,
                   dcheck_state)) {
    DLOG(ERROR) << "Unable to initialize logging to " << log_path.value();
    RemoveSymlinkAndLog(log_path, target_path);
  } else {
    chrome_logging_redirected_ = true;
  }
}

#endif  // OS_CHROMEOS

void InitCommonLogging(const CommandLine& command_line,
                       OldFileDeletionState delete_old_log_file) {
  DCHECK(!chrome_logging_initialized_) <<
    "Attempted to initialize logging when it was already initialized.";

  LoggingDestination logging_dest = DetermineLogMode(command_line);
  LogLockingState log_locking_state = LOCK_LOG_FILE;
  FilePath log_path;
#if defined(OS_CHROMEOS)
  FilePath target_path;
#endif

  // Don't resolve the log path unless we need to. Otherwise we leave an open
  // ALPC handle after sandbox lockdown on Windows.
  if (logging_dest == LOG_ONLY_TO_FILE ||
      logging_dest == LOG_TO_BOTH_FILE_AND_SYSTEM_DEBUG_LOG) {
    log_path = GetLogFileName(command_line);

#if defined(OS_CHROMEOS)
    // For BWSI (Incognito) logins, we want to put the logs in the user
    // profile directory that is created for the temporary session instead
    // of in the system log directory, for privacy reasons.
    if (command_line.HasSwitch(switches::kGuestSession))
      log_path = GetSessionLogFile(command_line);

    // On ChromeOS we log to the symlink.  We force creation of a new
    // symlink if we've been asked to delete the old log, since that
    // indicates the start of a new session.
    target_path = SetUpSymlinkIfNeeded(
        log_path, delete_old_log_file == logging::DELETE_OLD_LOG_FILE);

    // Because ChromeOS manages the move to a new session by redirecting
    // the link, it shouldn't remove the old file in the logging code,
    // since that will remove the newly created link instead.
    delete_old_log_file = logging::APPEND_TO_OLD_LOG_FILE;
#endif
  } else {
    log_locking_state = DONT_LOCK_LOG_FILE;
  }

  logging::DcheckState dcheck_state =
      command_line.HasSwitch(switches::kEnableDCHECK) ?
      logging::ENABLE_DCHECK_FOR_NON_OFFICIAL_RELEASE_BUILDS :
      logging::DISABLE_DCHECK_FOR_NON_OFFICIAL_RELEASE_BUILDS;

  bool success = InitLogging(log_path.value().c_str(),
                             logging_dest,
                             log_locking_state,
                             delete_old_log_file,
                             dcheck_state);

#if defined(OS_CHROMEOS)
  if (!success) {
    DPLOG(ERROR) << "Unable to initialize logging to " << log_path.value()
                << " (which should be a link to " << target_path.value() << ")";
    RemoveSymlinkAndLog(log_path, target_path);
    chrome_logging_failed_ = true;
    return;
  }
#else
  if (!success) {
    DPLOG(ERROR) << "Unable to initialize logging to " << log_path.value();
    chrome_logging_failed_ = true;
    return;
  }
#endif

  // Default to showing error dialogs.
  if (!CommandLine::ForCurrentProcess()->HasSwitch(switches::kNoErrorDialogs))
    logging::SetShowErrorDialogs(true);

  // we want process and thread IDs because we have a lot of things running
  logging::SetLogItems(true,  // enable_process_id
                       true,  // enable_thread_id
                       true,  // enable_timestamp
                       false);  // enable_tickcount

  // We call running in unattended mode "headless", and allow
  // headless mode to be configured either by the Environment
  // Variable or by the Command Line Switch.  This is for
  // automated test purposes.
  scoped_ptr<base::Environment> env(base::Environment::Create());
  if (env->HasVar("CHROME_HEADLESS") ||
      command_line.HasSwitch(switches::kNoErrorDialogs))
    SuppressDialogs();

  // Use a minimum log level if the command line asks for one,
  // otherwise leave it at the default level (INFO).
  if (command_line.HasSwitch(switches::kLoggingLevel)) {
    std::string log_level = command_line.GetSwitchValueASCII(
        switches::kLoggingLevel);
    int level = 0;
    if (base::StringToInt(log_level, &level) &&
        level >= 0 && level < LOG_NUM_SEVERITIES) {
      logging::SetMinLogLevel(level);
    } else {
      DLOG(WARNING) << "Bad log level: " << log_level;
    }
  }

#if defined(OS_WIN)
  // Enable trace control and transport through event tracing for Windows.
  logging::LogEventProvider::Initialize(kChromeTraceProviderName);
#endif

#ifdef NDEBUG
  if (command_line.HasSwitch(switches::kSilentDumpOnDCHECK) &&
      command_line.HasSwitch(switches::kEnableDCHECK)) {
#if defined(OS_WIN)
    logging::SetLogReportHandler(DumpProcessAssertHandler);
#endif
  }
#endif  // NDEBUG

  chrome_logging_initialized_ = true;
}

// This is a no-op, but we'll keep it around in case
// we need to do more cleanup in the future.
void CleanupCommonLogging() {
  if (chrome_logging_failed_)
    return;  // We failed to initiailize logging, no cleanup.

  DCHECK(chrome_logging_initialized_) <<
    "Attempted to clean up logging when it wasn't initialized.";

  CloseLogFile();

  chrome_logging_initialized_ = false;
  chrome_logging_redirected_ = false;
}

FilePath GetLogFileName(const CommandLine& command_line) {
  std::wstring process_type = ASCIIToWide(command_line.GetSwitchValueASCII(switches::kProcessType));
  if (process_type.size() == 0) {
    process_type = ASCIIToWide(switches::kProcessTypePhone);
  }

  FilePath log_path;
  PathService::Get(base::DIR_EXE,&log_path);
  //文件设置为ctp_data/log/daily/2013-7-13 15-11-00
  base::Time::Exploded e;
  base::Time time = base::Time::Now();
  time.LocalExplode(&e);
  std::wstring sub_dir = base::StringPrintf(L"%d-%d-%d-%ls", e.year, e.month, e.day_of_month, process_type.c_str());
  std::wstring name = base::StringPrintf(L"%d-%d-%d.log", e.hour, e.minute, e.second);
  std::wstring profile_name(L"common_log.txt");
  log_path = log_path.Append(L"ctp_data");
  log_path = log_path.Append(L"log");
  log_path = log_path.Append(sub_dir);
  file_util::CreateDirectoryW(log_path);

  log_path = log_path.Append(name);
  int i=1;
  while(file_util::PathExists(log_path)) {
    log_path=log_path.DirName();
    log_path=log_path.Append(profile_name);
    log_path=log_path.InsertBeforeExtension(base::IntToString16(i));
    i++;
  }
  return log_path;
}

//ugly...
FilePath GetLogFileName() {
  FilePath log_path;
  PathService::Get(base::DIR_EXE,&log_path);
  //文件设置为ctp_data/log/daily/2013-7-13 15-11-00
  base::Time::Exploded e;
  base::Time time = base::Time::Now();
  time.LocalExplode(&e);
  std::wstring sub_dir = base::StringPrintf(L"%d-%d-%d", e.year, e.month, e.day_of_month);
  std::wstring name = base::StringPrintf(L"%d-%d-%d.log", e.hour, e.minute, e.second);
  std::wstring profile_name(L"common_log.txt");
  log_path = log_path.Append(L"ctp_data");
  log_path = log_path.Append(L"log");
  log_path = log_path.Append(sub_dir);
  file_util::CreateDirectoryW(log_path);

  log_path = log_path.Append(name);
  int i=1;
  while(file_util::PathExists(log_path)) {
    log_path=log_path.DirName();
    log_path=log_path.Append(profile_name);
    log_path=log_path.InsertBeforeExtension(base::IntToString16(i));
    i++;
  }
  return log_path;
}


bool DialogsAreSuppressed() {
  return dialogs_are_suppressed_;
}

size_t GetFatalAssertions(AssertionList* assertions) {
  // In this function, we don't assume that assertions is non-null, so
  // that if you just want an assertion count, you can pass in NULL.
  if (assertions)
    assertions->clear();
  size_t assertion_count = 0;

  std::ifstream log_file;
  log_file.open(GetLogFileName().value().c_str());
  if (!log_file.is_open())
    return 0;

  std::string utf8_line;
  std::wstring wide_line;
  while (!log_file.eof()) {
    getline(log_file, utf8_line);
    if (utf8_line.find(":FATAL:") != std::string::npos) {
      wide_line = UTF8ToWide(utf8_line);
      if (assertions)
        assertions->push_back(wide_line);
      ++assertion_count;
    }
  }
  log_file.close();

  return assertion_count;
}

void DumpWithoutCrashing() {
#if defined(OS_WIN)
  std::string str;
  DumpProcessAssertHandler(str);
#elif defined(USE_LINUX_BREAKPAD) || defined(OS_MACOSX)
  if (dump_without_crashing_function_)
    (*dump_without_crashing_function_)();
#else
  NOTIMPLEMENTED();
#endif
}

#if defined(USE_LINUX_BREAKPAD) || defined(OS_MACOSX)
void SetDumpWithoutCrashingFunction(void (*function)()) {
  dump_without_crashing_function_ = function;
}
#endif

FilePath GenerateTimestampedName(const FilePath& base_path,
                                 base::Time timestamp) {
  base::Time::Exploded time_deets;
  timestamp.LocalExplode(&time_deets);
  std::string suffix = base::StringPrintf("_%02d%02d%02d-%02d%02d%02d",
                                          time_deets.year,
                                          time_deets.month,
                                          time_deets.day_of_month,
                                          time_deets.hour,
                                          time_deets.minute,
                                          time_deets.second);
  return base_path.InsertBeforeExtensionASCII(suffix);
}

}  // namespace logging
