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

#include "../../../phone_spi/adbtool/adb_define.h"

namespace gfx {
class ImageSkia;
}

namespace views {
class Checkbox;
class Event;
class TableView;

namespace examples {
  class InstallApkListTable;
  class ApkIRStatusModel : public ui::TableModel,
    public TableViewObserver {
  public:
    ApkIRStatusModel(InstallApkListTable *p)
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
    InstallApkListTable *table_;
  };

class CTPTabbedPane;

class InstallApkListTable : public CTPViewBase,
                     public ui::TableModel,
                     public TableViewObserver,
                     public ButtonListener,
                     public ThreadMessageFilter,
                     public content::NotificationObserver,
                     //public CCTableView::Delegate,
                     public ComboboxListener {
 public:


  InstallApkListTable(CTPTabbedPane *p, std::wstring const &);
  virtual ~InstallApkListTable();
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


  /*virtual bool GetCellColors(
    CCTableView* who,
    int model_row,
    int column,
    CCTableView::ItemColor* foreground,
    CCTableView::ItemColor* background,
    LOGFONT* logfont) OVERRIDE;*/


  
  void OnMarginRate(PointerWrapper<CThostFtdcInstrumentMarginRateField> const & p);
  void OnUpdatePackageList(PointerWrapper<std::vector<phone_module::ApkInstallInfo>> const & p);
  void OnUpdateApkIRStatus(PointerWrapper<phone_module::ApkIRStatus> const & p);


  gfx::ImageSkia GetIcon2(int row);
  string16 GetText2(int row, int column_id);
  int RowCount2();
  
 private:

  
   ApkIRStatusModel model_apk_ir_;
   std::vector<phone_module::ApkIRStatus> apk_ir_data_;
   std::map<int, int> apk_ir_data_map_;
   TableView* table_apk_ir_;

  TableView* table_;
  std::vector<phone_module::ApkInstallInfo> data_;
  
  

  TextButton* get_apk_list_;
  TextButton* check_update_apk_list_;
  TextButton* install_apk_list_all_device_;
  TextButton* install_apk_list_all_device_force_;
  TextButton* clear_table_;
  TextButton* clear_apk_ir_table_;

  gfx::ImageSkia* alive_;
  gfx::ImageSkia* die_;


  content::NotificationRegistrar registrar_;
  CTPTabbedPane* pane_;
  std::string const bc_;


  
  void EnsureVisible();


  DISALLOW_COPY_AND_ASSIGN(InstallApkListTable);
};

}  // namespace examples
}  // namespace views


