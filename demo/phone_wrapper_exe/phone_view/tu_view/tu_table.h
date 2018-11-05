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

  struct OrderData {
    std::string type;
    base::Time utc;
    std::string utc_string;
    int32 operation;
    std::string order_ref;
    int32 status;
    int32 submit_status;
    std::string msg;
    double price;
    int32 volumn;
    int32 oc;
    int32 bs;
    std::string trade_time;
    std::string ms_arrive;
    std::string os_start;
    std::string os_over;
    std::string msos;
  };


  

  /*class StringComboboxModel : public ui::ComboboxModel {
  public:
    StringComboboxModel() {}
    virtual ~StringComboboxModel() {}

    // Overridden from ui::ComboboxModel:
    virtual int GetItemCount() const OVERRIDE { return data_.size(); }
    virtual string16 GetItemAt(int index) OVERRIDE {
      return ASCIIToUTF16(data_[index]);
    }

    void Add(std::string const &d) {
      data_.push_back(d);
    }
    std::string Get(uint32 const i) {
      DCHECK(data_.size() > i);
      return data_[i];
    }

  private:
    std::vector<std::string> data_;

    DISALLOW_COPY_AND_ASSIGN(StringComboboxModel);
  };*/

class TUTable;




class OrderModel : public ui::TableModel,
  public TableViewObserver {
public:
  OrderModel(TUTable *p)
    :table_(p) {}
  // ui::TableModel:
  virtual int RowCount() OVERRIDE;
  virtual string16 GetText(int row, int column_id) OVERRIDE;
  virtual gfx::ImageSkia GetIcon(int row) OVERRIDE;
  virtual void SetObserver(ui::TableModelObserver* observer) OVERRIDE {}

  // TableViewObserver:
  virtual void OnSelectionChanged() OVERRIDE {}
  virtual void OnDoubleClick() OVERRIDE {}
  virtual void OnMiddleClick() OVERRIDE {}
  virtual void OnKeyDown(ui::KeyboardCode virtual_keycode) OVERRIDE {}
  virtual void OnTableViewDelete(TableView* table_view) OVERRIDE {}
  virtual void OnTableView2Delete(TableView2* table_view) OVERRIDE {}

private:
  TUTable *table_;
};




class CTPTabbedPane;

class TUTable : public CTPViewBase,
                     public ui::TableModel,
                     public TableViewObserver,
                     public ButtonListener,
                     public ThreadMessageFilter,
                     public content::NotificationObserver,
                     public CCTableView::Delegate,
                     public ComboboxListener {
 public:
   //所有品种，所有策略=
   
   typedef std::map<std::string, SkColor> ColorMap;
   typedef std::vector<std::shared_ptr<OrderData>> OrderDataList;
   //typedef std::vector<phone_module::MarketStrategyOrder const*> DataListIndex;
   //所有品种，某个策略，索引=
   //typedef std::map<std::string, DataListIndex> StrategyMap;
   //某个品种，所有策略，索引=
   //typedef std::map<std::string, DataListIndex> TypeMap;
   //某个品种，某个策略，索引=
   //typedef std::map<std::pair<std::string, std::string>, DataListIndex> StrategyTypeMap;


  TUTable(CTPTabbedPane *p, std::string const &);
  virtual ~TUTable();
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


  virtual bool GetCellColors(
    CCTableView* who,
    int model_row,
    int column,
    CCTableView::ItemColor* foreground,
    CCTableView::ItemColor* background,
    LOGFONT* logfont) OVERRIDE;


  
  void OnOrder(PointerWrapper<phone_module::OrderFootprints::OneRecord> const & p);
  void OnCash(PointerWrapper<CThostFtdcTradingAccountField> const & p);
  void OnTypeList(std::vector<std::string> const &);

  
 private:

   
   friend OrderModel;
  // The table to be tested.
  OrderModel model_order_;
  TableView* table_order_;




  Checkbox* column1_visible_checkbox_;
  Checkbox* column2_visible_checkbox_;
  Checkbox* column3_visible_checkbox_;
  Checkbox* column4_visible_checkbox_;

  TextButton * clear_order_button_;
  Label * cash_label_;
  Label * balance_label_;
  Label * position_profit_label_;

  //手动=
  Combobox* manual_type_checkbox_;
  Combobox* manual_direction_checkbox_;
  Combobox* manual_oc_checkbox_;
  //Combobox* manual_close_day_checkbox_;
  StringComboboxModel manual_type_checkbox_model_;
  StringComboboxModel manual_direction_checkbox_model_;
  StringComboboxModel manual_oc_checkbox_model_;
  //StringComboboxModel manual_close_day_checkbox_model_;
  Textfield* manual_number_;
  BigTextButton * manual_button_;

  //Textfield* manual_direction_text_;
  //Textfield* manual_oc_text_;
  
  
  //StringComboboxModel type_combobox_model_;
  //StringComboboxModel strategy_combobox_model_;
  //Combobox* strategy_combobox_;
  //Combobox* type_combobox_;
  //SkBitmap icon1_;
  //SkBitmap icon2_;

  

  OrderDataList order_data_;
  //StrategyMap strategy_map_;
  //TypeMap type_map_;
  //StrategyTypeMap strategy_type_map_;
  content::NotificationRegistrar registrar_;
  CTPTabbedPane* pane_;
  std::string const bc_;


  
  
  void OnInitParam(std::string const & s);


  string16 GetColumnText(int id, phone_module::PositionData const & info);
  string16 GetColumnTextOrder(int id, OrderData const & info);
  void EnsureVisible();


  bool OverrideThreadForMessage(IPC::Message const& message,
    CommonThread::ID* thread);

  DISALLOW_COPY_AND_ASSIGN(TUTable);
};

}  // namespace examples
}  // namespace views


