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

//#include "ms_table.h"

namespace gfx {
class ImageSkia;
}

namespace views {
class Checkbox;
class Event;
class TableView;

namespace examples {
  class MainView;

  class StatusModel : public ui::TableModel,
    public TableViewObserver {
  public:
    StatusModel(MainView *p)
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
    MainView *table_;
  };
  
  //=======================================================
  typedef std::map<std::wstring, std::shared_ptr<phone_module::DeviceData>> DeviceDataList;
  typedef std::vector<std::shared_ptr<phone_module::StatusInfo>> StatusInfoList;
  typedef std::map<std::string, SkColor> ColorMap;


  class OrderResultTable;

  class DeviceDelegate {
  public:
    virtual int size() = 0;
    virtual string16 text(int row, int column_id) = 0;
  };




  class DeviceModel : public ui::TableModel,
    public TableViewObserver {
  public:
    DeviceModel(DeviceDelegate *p)
      :delegate_(p) {}
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
    DeviceDelegate *delegate_;
  };







class CTPTabbedPane;

class MainView : public CTPViewBase,
                     public ui::TableModel,
                     public TableViewObserver,
                     public ButtonListener,
                     public ThreadMessageFilter,
                     public content::NotificationObserver,
                     public CCTableView::Delegate,
                     public DeviceDelegate,
                     public ComboboxListener {
 public:
   //所有品种，所有策略=
   
   //typedef std::vector<std::shared_ptr<OrderResultData>> OrderDataList;
   //typedef std::vector<phone_module::MarketStrategyOrder const*> DataListIndex;
   //所有品种，某个策略，索引=
   //typedef std::map<std::string, DataListIndex> StrategyMap;
   //某个品种，所有策略，索引=
   //typedef std::map<std::string, DataListIndex> TypeMap;
   //某个品种，某个策略，索引=
   //typedef std::map<std::pair<std::string, std::string>, DataListIndex> StrategyTypeMap;


  MainView(CTPTabbedPane *p, std::string const &);
  virtual ~MainView();
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


  //void OnPosition(PointerWrapper<phone_module::DeviceData> const & p);
  //void OnOrder(PointerWrapper<phone_module::OrderFootprints::OneRecord> const & p);
  //void OnCash(PointerWrapper<CThostFtdcTradingAccountField> const & p);
  void OnDeviceUpdate(PointerWrapper< phone_module::DevicesList> const & p);
  void OnStatusInfo(PointerWrapper<phone_module::StatusInfo> const & p);
  

  virtual int size() OVERRIDE;
  virtual string16 text(int row, int column_id) OVERRIDE;


  std::string const bc_;

  
 private:

   friend StatusModel;
   friend DeviceModel;
   //friend OrderModel;
  // The table to be tested.
  StatusModel model_order_;
  TableView* table_order_;

  DeviceModel model_device_;
  CCTableView* table_device_;
  ColorMap position_color_map_;
  phone_module::DevicesList device_data_;



  TextButton* clear_table_;

  TextButton * clear_order_button_;
  Label * cash_label_;
  Label * balance_label_;
  Label * position_profit_label_;
  



  StatusInfoList status_info_data_;


  content::NotificationRegistrar registrar_;
  CTPTabbedPane* pane_;
  


  
  
  void OnInitParam(std::string const & s);


  string16 GetColumnText(int id, phone_module::StatusInfo const & info);
  //string16 GetColumnTextOrder(int id, OrderResultData const & info);
  string16 GetColumnTextPosition(int id, phone_module::AdbDevice const & info);
  void EnsureVisible();


  bool OverrideThreadForMessage(IPC::Message const& message,
    CommonThread::ID* thread);

  DISALLOW_COPY_AND_ASSIGN(MainView);
};

}  // namespace examples
}  // namespace views


