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

#include "third_party/skia/include/core/SkBitmap.h"
#include "ui/base/models/table_model.h"
#include "ui/views/controls/button/button.h"
#include "ui/views/controls/table/table_view.h"
#include "ui/views/controls/table/table_view_observer.h"
#include "ui/base/models/combobox_model.h"
#include "ui/base/win/window_impl.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/views/controls/button/text_button.h"
#include "ui/views/controls/label.h"
#include "ui/views/controls/combobox/combobox.h"
#include "ui/views/controls/combobox/combobox_listener.h"
#include "ui/views/controls/textfield/textfield_controller.h"
#include "ui/views/controls/progress_bar.h"

#include "content/public/browser/notification_registrar.h"
#include "content/public/browser/notification_observer.h"
#include "content/browser/notification_service_impl.h"


//#include "log_daemon.h"
#include "../../../phone_common/common/common_thread.h"
//#include "profiler_data.h"
#include "../../../phone_common/include/ctp_define.h"
#include "../../../phone_common/common/common_main.h"
#include "../../../phone_common/common/thread_message_filter.h"
#include "../../../phone_common/include/ctp_messages.h"

#include "../../../phone_spi/phone_module/common.h"
#include "../example_base.h"
#include "../cc_table.h"
#include "../ms_table.h"

namespace gfx {
class ImageSkia;
}

namespace views {
class Checkbox;
class Event;
class TableView;

namespace examples {

  



class CTPTabbedPane;
class TUTabbedPane;

class TUControlTable : public CTPViewBase,
                     //public ui::TableModel,
                     //public TableViewObserver,
                     public ButtonListener,
                     public ThreadMessageFilter,
                     public content::NotificationObserver,
                     //public CCTableView::Delegate,
                     public ComboboxListener {
 public:
  TUControlTable(CTPTabbedPane *p, phone_module::TradeUnitInfo const &info);
  virtual ~TUControlTable();
  virtual void Selected() OVERRIDE {}
  // CTPViewBase:
  virtual void CreateExampleView(View* container) OVERRIDE;


  virtual void Observe(int type,
    const content::NotificationSource& source,
    const content::NotificationDetails& details) OVERRIDE;

  virtual bool OnMessageReceived(IPC::Message const & message) OVERRIDE;


  // ButtonListener:
  virtual void ButtonPressed(Button* sender, const ui::Event& event) OVERRIDE;

  //
  virtual void OnSelectedIndexChanged(Combobox* combobox) OVERRIDE;

  void OnStopPrice(PointerWrapper<phone_module::StopPriceData> const & p);
  
 private:
   bool RealAccountAlertOK();

  content::NotificationRegistrar registrar_;
  CTPTabbedPane* p_;
  //std::string const bc_;
  phone_module::TradeUnitInfo const & info_;

  friend class TUTabbedPane;

  //ProgressBar * connect_bar_;
  TextButton* connect_button_;
  TextButton* disconnect_button_;
  Label * login_status_;
  Checkbox* allow_execute_ms_button_;
  TextButton* query_position_button_;

  Label * server_ipandport_;
  Label * server_ipandport_content_;
  Label * stop_percent_;
  Label * stop_percent_content_;
  Label * stop_win_percent_;
  Label * stop_win_percent_content_;
  Label * strategy_;
  Label * strategy_content_;
  Label * volumn_count_;
  Label * volumn_count_content_;
  Label * type_msname_;
  Label * type_msname_content_;

  Label * new_stopprice_;
  Label * new_stopprice_content_;
  Label * old_stopprice_;
  Label * old_stopprice_content_;

  DISALLOW_COPY_AND_ASSIGN(TUControlTable);
};

}  // namespace examples
}  // namespace views


