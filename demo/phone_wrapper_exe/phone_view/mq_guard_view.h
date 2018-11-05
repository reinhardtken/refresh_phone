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

  
  struct ProcessRecord {
    std::wstring name;
    std::wstring description;
    bool alive;
    uint32 process_id;
    std::string socket_name;
  };

  typedef std::vector<ProcessRecord> ProcessVector;


//============================================
class MQGuardView : public CTPViewBase,
                     public ui::TableModel,
                     public TableViewObserver,
                     public ButtonListener,
                     public ThreadMessageFilter,
                     public content::NotificationObserver,
                     public views::ContextMenuController,
                     public views::Menu::Delegate,
                     public ComboboxListener {
 public:
  MQGuardView();
  virtual ~MQGuardView();
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

  void MQGuardView::DispatchProtobufMessage(std::string const & name, codec::MessagePtr const & p, base::Time const&);
  void MQGuardView::DispatchTLVMessage(std::string const & name, codec::TLVMessagePtr const & p, base::Time const&);
  void MQGuardView::CheckAlive();

 private:
  // The table to be tested.
  TableView* table_;
  gfx::ImageSkia* alive_;
  gfx::ImageSkia* die_;

  ProcessVector process_vector_;

  
  void OnMQ(int32 const type, int32 const id, base::Time const & time, PointerWrapper<phone_module::LevelQuotationInfoBig> const & p);
  void OnInitParam(std::string const & s);
  void OnLoginSuccess();
  void OnDisConnect();
  void OnMQConnect(std::string const &name, bool const connected);

  string16 GetColumnText(int id, phone_module::LevelQuotationInfoBig const & info);
  void EnsureVisible();


  bool OverrideThreadForMessage(IPC::Message const& message,
    CommonThread::ID* thread);


  scoped_ptr<channel::ChannelHost> channel_host_;
  //typedef base::CancelableCallback<void(void)> VoidVoidCallback;
  //scoped_ptr<VoidVoidCallback> alive_check_;
  

  DISALLOW_COPY_AND_ASSIGN(MQGuardView);
};

}  // namespace examples
}  // namespace views


