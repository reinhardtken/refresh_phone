#include "common_process.h"


#include "base/file_path.h"
#include "base/file_util.h"
#include "base/process_util.h"


#include "common_thread.h"
#include "common_ipc_server.h"

//////////////////////////////////////////////////////////////////////////
CommonProcess* g_common_process = NULL;

//////////////////////////////////////////////////////////////////////////
CommonProcess::CommonProcess(const CommandLine& command_line)
  :created_io_thread_(false),
  created_launcher_thread_(false),
  created_db_thread_(false),
  created_file_thread_(false),
  created_ctp_thread_(false),
  created_omq_thread_(false),
  command_line_(command_line) {
  g_common_process = this;
  
}

CommonProcess::~CommonProcess()
{
  //注意顺序,倒序=
  omq_thread_.reset();
  ctp_thread_.reset();
  launcher_thread_.reset();
  file_thread_.reset();
  db_thread_.reset();
  io_thread_.reset();

  g_common_process = NULL;
}

void CommonProcess::CreateMM() {
  CommonThread::PostTask(CommonThread::CTP,
    FROM_HERE,
    base::Bind(&CommonProcess::CreateMMOnCTP, base::Unretained(this), command_line_));
}



void CommonProcess::CreateMMOnCTP(const CommandLine& command_line) {
  DCHECK(CommonThread::CurrentlyOn(CommonThread::CTP));
  mm_impl_ = new core_module::ModuleManagerImpl(command_line);
  mm_impl_->Construct();
}

void CommonProcess::DestroyMM() {
  CommonThread::PostTask(CommonThread::CTP,
    FROM_HERE,
    base::Bind(&CommonProcess::DestroyMMOnCTP, base::Unretained(this)));
}



void CommonProcess::DestroyMMOnCTP() {
  DCHECK(CommonThread::CurrentlyOn(CommonThread::CTP));
  mm_impl_->Destruct();
  //删除后，coremodule也就析构，ThreadMessageDispatcherImpl也析构了。正确的方法应该是ThreadMessageDispatcherImpl生命周期不受其管理才对
  //delete mm_impl_;
  //mm_impl_ = NULL;
}

void CommonProcess::CreateIoThread()
{
  DCHECK(!created_io_thread_ && io_thread_.get() == NULL);
  created_io_thread_ = true;

  scoped_ptr<base::Thread> thread(new CommonThread(CommonThread::IO));
  base::Thread::Options options;
  options.message_loop_type = MessageLoop::TYPE_IO;
  if (!thread->StartWithOptions(options))
    return;
  io_thread_.swap(thread);
}

base::Thread* CommonProcess::io_thread()
{
  DCHECK(CalledOnValidThread());
  if (!created_io_thread_)
    CreateIoThread();
  return io_thread_.get();
}

void CommonProcess::CreateFileThread()
{
  DCHECK(!created_file_thread_ && file_thread_.get() == NULL);
  created_file_thread_ = true;

  scoped_ptr<base::Thread> thread(new CommonThread(CommonThread::FILE));
  base::Thread::Options options;
  options.message_loop_type = MessageLoop::TYPE_IO;
  if (!thread->StartWithOptions(options))
    return;
  file_thread_.swap(thread);
}

base::Thread* CommonProcess::file_thread()
{
  DCHECK(CalledOnValidThread());
  if (!created_file_thread_)
    CreateFileThread();
  return file_thread_.get();
}


void CommonProcess::CreateCTPThread()
{
	DCHECK(!created_ctp_thread_ && ctp_thread_.get() == NULL);
	created_ctp_thread_ = true;

	scoped_ptr<base::Thread> thread(new CommonThread(CommonThread::CTP));
	base::Thread::Options options;
	options.message_loop_type = MessageLoop::TYPE_IO;
	if (!thread->StartWithOptions(options))
		return;
	ctp_thread_.swap(thread);
}

base::Thread* CommonProcess::ctp_thread()
{
	DCHECK(CalledOnValidThread());
	if (!created_ctp_thread_)
		CreateCTPThread();
	return ctp_thread_.get();
}

void CommonProcess::CreateOMQThread()
{
  DCHECK(!created_omq_thread_ && omq_thread_.get() == NULL);
  created_omq_thread_ = true;

  scoped_ptr<base::Thread> thread(new CommonThread(CommonThread::OMQ));
  base::Thread::Options options;
  options.message_loop_type = MessageLoop::TYPE_IO;
  if (!thread->StartWithOptions(options))
    return;
  omq_thread_.swap(thread);
}

base::Thread* CommonProcess::omq_thread()
{
  DCHECK(CalledOnValidThread());
  if (!created_omq_thread_)
    CreateOMQThread();
  return omq_thread_.get();
}

void CommonProcess::CreateDbThread()
{
  DCHECK(!created_db_thread_ && db_thread_.get() == NULL);
  created_db_thread_ = true;

  scoped_ptr<base::Thread> thread(new CommonThread(CommonThread::DB));
  base::Thread::Options options;
  options.message_loop_type = MessageLoop::TYPE_IO;
  if (!thread->StartWithOptions(options))
    return;
  db_thread_.swap(thread);
}

base::Thread* CommonProcess::db_thread()
{
  DCHECK(CalledOnValidThread());
  if (!created_db_thread_)
    CreateDbThread();
  return db_thread_.get();
}

void CommonProcess::reset_dbthread(){
  DCHECK(CalledOnValidThread());

  db_thread_.reset();
  created_db_thread_ = false;
  CreateDbThread();
}

void CommonProcess::CreateLauncherThread()
{
  DCHECK(!created_launcher_thread_ && launcher_thread_.get() == NULL);
  created_launcher_thread_ = true;

  scoped_ptr<base::Thread> thread(new CommonThread(CommonThread::PROCESS_LAUNCHER));
  base::Thread::Options options;
  options.message_loop_type = MessageLoop::TYPE_DEFAULT;
  if (!thread->StartWithOptions(options))
    return;
  launcher_thread_.swap(thread);
}

base::Thread* CommonProcess::launcher_thread()
{
  DCHECK(CalledOnValidThread());
  if (!created_launcher_thread_)
    CreateLauncherThread();
  return launcher_thread_.get();
}

// CommonData* CommonProcess::common_data()
// {
//   if(!common_data_.get()){
//     common_data_ = new CommonData;
//   }
// 
//   return common_data_.get();
// }
//////////////////////////////////////////////////////////////////////////