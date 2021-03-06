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
  class AutoInstallApkListTable;
  class ApkIRStatusModel2 : public ui::TableModel,
    public TableViewObserver {
  public:
    ApkIRStatusModel2(AutoInstallApkListTable *p)
      :table_(p) {}
    // ui::TableModel:
    virtual int RowCount() OVERRIDE;
    virtual string16 GetText(int row, int column_id) OVERRIDE;
    virtual gfx::ImageSkia GetIcon(int row) OVERRIDE;
    virtual void SetObserver(ui::TableModelObserver* observer) OVERRIDE {}

    // TableViewObserver:
    virtual void OnSelectionChanged() OVERRIDE;
    virtual void OnDoubleClick() OVERRIDE {}
    virtual void OnMiddleClick() OVERRIDE {}
    virtual void OnKeyDown(ui::KeyboardCode virtual_keycode) OVERRIDE {}
    virtual void OnTableViewDelete(TableView* table_view) OVERRIDE {}
    virtual void OnTableView2Delete(TableView2* table_view) OVERRIDE {}

  private:
    AutoInstallApkListTable *table_;
  };

class CTPTabbedPane;

class AutoInstallApkListTable : public CTPViewBase,
                     public ui::TableModel,
                     public TableViewObserver,
                     public ButtonListener,
                     public ThreadMessageFilter,
                     public content::NotificationObserver,
                     //public CCTableView::Delegate,
                     public ComboboxListener {
 public:
   static int g_current_mode_;

  AutoInstallApkListTable(CTPViewBase *p, bool );
  virtual ~AutoInstallApkListTable();
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
  void OnFirstTableSelectionChanged();

  void InnerUpdateInstallApkDigest(phone_module::InstallDigest & digest);
  void InnerOnDeviceUpdate(phone_module::DevicesList const & list);
  /*virtual bool GetCellColors(
    CCTableView* who,
    int model_row,
    int column,
    CCTableView::ItemColor* foreground,
    CCTableView::ItemColor* background,
    LOGFONT* logfont) OVERRIDE;*/


  
  void OnMarginRate(PointerWrapper<CThostFtdcInstrumentMarginRateField> const & p);
  void OnUpdatePackageList(PointerWrapper<std::vector<phone_module::ApkInstallInfo>> const & p);
  void OnUpdateInstallApkDigest(PointerWrapper<phone_module::InstallDigest> const & p);
  void OnApkUpdateInfoToString(std::wstring const & s);

  


  gfx::ImageSkia GetIcon2(int row);
  string16 GetText2(int row, int column_id);
  int RowCount2();
  
 private:

  
   ApkIRStatusModel2 model_apk_ir_;
   std::vector<phone_module::InstallDigest> install_digest_data_;
   std::map<std::wstring, int> install_digest_map_;
   TableView* table_apk_ir_;

  TableView* table_;
  std::vector< phone_module::FailedTuple> failed_list_;
  std::wstring current_serial_number_;
  //std::vector<phone_module::ApkInstallInfo> data_;
  
  

  TextButton* auto_mode_;
  bool auto_install_mode_;
  //TextButton* check_update_apk_list_;
  //TextButton* install_apk_list_all_device_;
  //TextButton* install_apk_list_all_device_force_;
  //TextButton* clear_table_;
  //TextButton* clear_apk_ir_table_;
  Label* status_label_;

  gfx::ImageSkia* alive_;
  gfx::ImageSkia* die_;


  content::NotificationRegistrar registrar_;
  CTPViewBase* pane_;
  bool const total_auto_;//标明当前UI是局部自动化模式的装包还是全局自动化的UI
  
  std::wstring GetAutoModeText(bool auto_mode);
  
  void EnsureVisible();


  DISALLOW_COPY_AND_ASSIGN(AutoInstallApkListTable);
};

}  // namespace examples
}  // namespace views


