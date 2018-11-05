#pragma once

#include "base/basictypes.h"
#include "base/memory/scoped_ptr.h"

#include "../include/ctp_define.h"
#include "thread_message_filter.h"

class CommandLine;
class MessageLoop;
class CommonThread;

namespace chrome {
	class RemovableDeviceNotificationsWindowWin;
}

class PHONE_COMMON_EXPORT CommonMainParts {
public:
  static CommonMainParts* CreateCommonMainParts(const CommandLine& command_line);
  virtual ~CommonMainParts();

  // Parts to be called by |CommonMain()|.
  void EarlyInitialization();
  void MainMessageLoopStart();
  void PreMainMessageLoopRun();

protected:
  explicit CommonMainParts(const CommandLine& command_line);

  MessageLoop& main_message_loop() const {
    return *main_message_loop_;
  }

  // Methods to be overridden to provide platform-specific code; these
  // correspond to the "parts" above.
  virtual void PreEarlyInitialization() {}
  virtual void PostEarlyInitialization() {}
  virtual void PreMainMessageLoopStart();
  

  virtual void PostMainMessageLoopStart() {}

private:
  // Methods for |EarlyInitialization()| ---------------------------------------

  // Methods for |MainMessageLoopStart()| --------------------------------------

  void InitializeMainThread();

  // Members initialized on construction ---------------------------------------

  const CommandLine& parsed_command_line_;

  // Members initialized in |MainMessageLoopStart()| ---------------------------
  scoped_ptr<MessageLoop> main_message_loop_;
  scoped_ptr<CommonThread> main_thread_;

  scoped_ptr<ThreadMessageDispatcherImpl> thread_message_dispatcher_impl_;

  scoped_ptr<chrome::RemovableDeviceNotificationsWindowWin>
	  removable_device_notifications_window_;

  // Initialized in SetupMetricsAndFieldTrials.
  DISALLOW_COPY_AND_ASSIGN(CommonMainParts);
};
