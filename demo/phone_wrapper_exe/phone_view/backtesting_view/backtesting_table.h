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
#include "ui/views/controls/button/checkbox.h"

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

class BacktestingView : public CTPViewBase,
                     //public ui::TableModel,
                     //public TableViewObserver,
                     public ButtonListener,
                     public ThreadMessageFilter,
                     public content::NotificationObserver,
                     //public CCTableView::Delegate,
                     public ComboboxListener {
 public:
  BacktestingView(CTPTabbedPane *p, std::string const &, int32 const id);
  virtual ~BacktestingView();
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
  
 private:

   void OnInitProgress(double const index, double const total, double const high_index, double const high_total);
   void OnExpectInitProgress(double const index, double const total);
   void OnResultSave2DB(double const index, double const total);
   void OnRunProgress(PointerWrapper<backtesting::DataCenter> const &p);
   

  content::NotificationRegistrar registrar_;
  CTPTabbedPane* p_;
  std::string const bc_;

  

  ProgressBar * init_bar_;
  ProgressBar * backtesting_bar_;
  ProgressBar * expect_bar_;
  TextButton* init_button_;
  TextButton* expect_button_;
  TextButton * prop_expect_button_;
  
  Label * type_;
  Label* level_;
  Label* start_;
  Label* end_;
  

  Label* total_;
  Label* high_level_total_;
  Label* cost_time_;

  Label* expect_total_;
  Label* expect_cost_time_;

  Label * principal_;
  Label * max_profit_;
  Label * min_profit_;
  Label * profit_;

  Label * buy_times_;
  Label * sell_times_;
  Label * stop_times_;
  Label * stop_win_times_;
  Label * sxf_;
  Label * pound_total_;
  Label * max_successive_deficit_period_;
  Label * max_no_newhigh_period_;

  Label * mean_value_;
  Label * standard_deviation_;
  Label * sharp_radio_;

  Label * net_profit_;
  Label * net_deficit_;
  Label * max_deviation_;

  Label* max_retracement_;
  Label* progress_;
  Label* run_cost_time_;
  Label* run_c_cost_time_;

  TextButton* backtesting_start_button_;
  TextButton* backtesting_pause_button_;
  TextButton* backtesting_giveup_button_;
  //Checkbox* allow_execute_ms_button_;
  //TextButton* query_position_button_;
  base::Time start_time_;
  base::Time start_time2_;

  Textfield* result_2_db_tablename_;
  Label * result_2_db_label_;
  TextButton* result_2_db_button_;
  //TextButton* result_2_pic_;
  Checkbox* result_2_pic_;
  ProgressBar * result_2_db_bar_;

  int32 const id_;

  DISALLOW_COPY_AND_ASSIGN(BacktestingView);
};

}  // namespace examples
}  // namespace views


