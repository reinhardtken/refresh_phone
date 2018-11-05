#pragma once

#include "base/basictypes.h"
#include "base/memory/scoped_ptr.h"
#include "base/threading/thread.h"
#include "base/threading/non_thread_safe.h"
#include "base/command_line.h"
//#include "tools/common/common_data.h"

#include "module_manager.h"

#include "../include/ctp_define.h"

class CommandLine;
#pragma warning (disable:4275)
class PHONE_COMMON_EXPORT CommonProcess : public base::NonThreadSafe{
public:
  explicit CommonProcess(const CommandLine& cmdline);
  virtual ~CommonProcess();

public:
  virtual base::Thread* io_thread();
  virtual base::Thread* file_thread();
  virtual base::Thread* db_thread();
  virtual base::Thread* ctp_thread();
  virtual base::Thread* omq_thread();
  virtual base::Thread* launcher_thread();
  //virtual CommonData* common_data();
  virtual void reset_dbthread();

  void CreateMM();
  void DestroyMM();
  void CreateMMOnCTP(const CommandLine& command_line);
  void DestroyMMOnCTP();
  core_module::ModuleManagerImpl * mm() { return mm_impl_; }

private:
  void CreateIoThread();
  bool created_io_thread_;

  void CreateFileThread();
  bool created_file_thread_;

  void CreateCTPThread();
  bool created_ctp_thread_;

  void CreateOMQThread();
  bool created_omq_thread_;

  void CreateLauncherThread();
  bool created_launcher_thread_;

  void CreateDbThread();
  bool created_db_thread_;

private:
  scoped_ptr<base::Thread> io_thread_;
  scoped_ptr<base::Thread> file_thread_;
  scoped_ptr<base::Thread> db_thread_;
  scoped_ptr<base::Thread> launcher_thread_;
  scoped_ptr<base::Thread> ctp_thread_;
  scoped_ptr<base::Thread> omq_thread_;
  //scoped_refptr<CommonData> common_data_;
  CommandLine const & command_line_;
  core_module::ModuleManagerImpl *mm_impl_;

  DISALLOW_COPY_AND_ASSIGN(CommonProcess);
};

extern PHONE_COMMON_EXPORT CommonProcess* g_common_process;
