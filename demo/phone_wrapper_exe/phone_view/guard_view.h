// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#pragma once


#include <string>
#include <list>
#include <vector>
#include <map>
#include <utility>
#include <memory>

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "base/utf_string_conversions.h"
#include "base/memory/scoped_ptr.h"
#include "base/memory/weak_ptr.h"
#include "base/scoped_native_library.h"
#include "base/i18n/icu_util.h"
#include "base/cancelable_callback.h"

#include "third_party/skia/include/core/SkBitmap.h"
#include "ui/base/models/table_model.h"
#include "ui/views/controls/button/button.h"
#include "ui/views/controls/table/table_view.h"
#include "ui/views/controls/table/table_view_observer.h"
#include "ui/base/models/combobox_model.h"
#include "ui/base/win/window_impl.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/views/controls/combobox/combobox.h"
#include "ui/views/controls/combobox/combobox_listener.h"
#include "ui/views/controls/progress_bar.h"
#include "ui/views/context_menu_controller.h"
#include "ui/views/controls/menu/menu.h"

#include "content/public/browser/notification_registrar.h"
#include "content/public/browser/notification_observer.h"
#include "content/browser/notification_service_impl.h"


//#include "log_daemon.h"
#include "../../phone_common/common/common_thread.h"
//#include "profiler_data.h"
#include "../../phone_common/include/ctp_define.h"
#include "../../phone_common/common/common_main.h"
#include "../../phone_common/common/thread_message_filter.h"
#include "../../phone_common/include/ctp_messages.h"
#include "../../phone_common/channel/codec/codec_util.h"


#include "../../phone_spi/phone_module/common.h"
#include "example_base.h"

namespace gfx {
class ImageSkia;
}

namespace channel {
  class ChannelHost;
}

namespace views {
class Checkbox;
class Event;
class TableView;

namespace examples {

  
	struct ProcessRecord;

  typedef std::vector<ProcessRecord> ProcessVector;


//============================================
class GuardView : public CTPViewBase,
                     public ui::TableModel,
                     public TableViewObserver,
                     public ButtonListener,
                     public ThreadMessageFilter,
                     public content::NotificationObserver,
                     public views::ContextMenuController,
                     public views::Menu::Delegate,
                     public ComboboxListener {
 public:

   struct AutoRun {
     std::string socket_name;
     std::string params;
     int32 rank;
     bool auto_reboot;//如果报活检查不到，是否自动重启==
   };

   struct TimingRun {
     std::string socket_name;
     std::string params;
     base::TimeDelta start;
     base::TimeDelta over;
     bool auto_reboot;//如果报活检查不到，是否自动重启==
   };

  GuardView();
  virtual ~GuardView();
  virtual void Selected() OVERRIDE;
  // CTPViewBase:
  virtual void CreateExampleView(View* container) OVERRIDE;


  virtual void Observe(int type,
    const content::NotificationSource& source,
    const content::NotificationDetails& details) OVERRIDE;

  virtual bool OnMessageReceived(IPC::Message const & message) OVERRIDE;

  // ui::TableModel:
  virtual int RowCount() OVERRIDE;
  virtual string16 GetText(int row, int column_id) OVERRIDE;
  virtual gfx::ImageSkia GetIcon(int row) OVERRIDE;
  virtual void SetObserver(ui::TableModelObserver* observer) OVERRIDE;

  // TableViewObserver:
  virtual void OnSelectionChanged() OVERRIDE;
  virtual void OnDoubleClick() OVERRIDE;
  virtual void OnMiddleClick() OVERRIDE;
  virtual void OnKeyDown(ui::KeyboardCode virtual_keycode) OVERRIDE;
  virtual void OnTableViewDelete(TableView* table_view) OVERRIDE;
  virtual void OnTableView2Delete(TableView2* table_view) OVERRIDE;

  // ButtonListener:
  virtual void ButtonPressed(Button* sender, const ui::Event& event) OVERRIDE;

  //
  virtual void OnSelectedIndexChanged(Combobox* combobox) OVERRIDE;

  virtual void ShowContextMenuForView(views::View* source,
    const gfx::Point& point) OVERRIDE;

  // views::Menu::Delegate:
  //virtual bool IsItemChecked(int id) const OVERRIDE;
  virtual void ExecuteCommand(int id) OVERRIDE;

  void GuardView::DispatchProtobufMessage(std::string const & name, codec::MessagePtr const & p, base::Time const&);
  void GuardView::DispatchTLVMessage(std::string const & name, codec::TLVMessagePtr const & p, base::Time const&);
  void GuardView::CheckAlive();

 private:
  // The table to be tested.
  TableView* table_;
  gfx::ImageSkia* alive_;
  gfx::ImageSkia* die_;

   
   scoped_ptr<ProcessVector>process_vector_;
  std::map<std::string, AutoRun> auto_run_;
  std::vector<std::pair<uint64, std::string>> auto_run_rank_;
  //自动启动的进程，在自动启动完成前，不准手动启动==
  //自动启动，且自动reboot的进程，永远不准手动启动==
  bool auto_run_finished_;
  std::map<std::string, TimingRun> timing_run_;
  std::vector<std::pair<uint64, std::string>> timing_start_rank_;//启动顺序的队列====
  std::vector<std::pair<uint64, std::string>> timing_over_rank_;//结束顺序的队列====

  
  void OnMQ(int32 const type, int32 const id, base::Time const & time, PointerWrapper<phone_module::LevelQuotationInfoBig> const & p);
  void OnInitParam(std::string const & s);
  void OnLoginSuccess();
  void OnDisConnect();

  string16 GetColumnText(int id, phone_module::LevelQuotationInfoBig const & info);
  void EnsureVisible();
  void LaunchWrapper(std::string const & socket_name);
  void Launch(uint32 const index);
  void InitLaunch(base::WeakPtr<GuardView> weak_ptr, std::vector<std::pair<uint64, std::string>> const & vec);
  base::TimeDelta NextStartTime(base::TimeDelta const &start);
  base::TimeDelta NextOverTime(base::TimeDelta const &over);
  void TimingLaunchOrTerminate(base::WeakPtr<GuardView> weak_ptr, std::vector<std::pair<uint64, std::string>> const & vec, bool start);
  void TimingMonitor();
  void NextInterval();
  bool TimingNeedStart();
  bool TimingNeedStop();
  void AutoRunFinish();
  void Terminate(std::string const & socket_name);
  bool TimingNeedInner(bool const start, base::TimeDelta const b_start_diff, base::TimeDelta const b_end_diff);

  bool OverrideThreadForMessage(IPC::Message const& message,
    CommonThread::ID* thread);


  scoped_ptr<channel::ChannelHost> channel_host_;

  typedef base::CancelableCallback<void(void)> VoidVoidCallback;
  //scoped_ptr<VoidVoidCallback> auto_run_task_;

  //scoped_ptr<VoidVoidCallback> timing_run_task_;
  scoped_ptr<VoidVoidCallback> timing_monitor_task_;

  //std::vector<base::Time> used_trading_day_;
  std::string timing_interval_;//下次定时重启进程的时间间隔，默认就是下一个工作日了，但测试的时候可以用5分钟===
  base::WeakPtrFactory<GuardView> weak_factory_;

  DISALLOW_COPY_AND_ASSIGN(GuardView);
};

}  // namespace examples
}  // namespace views


