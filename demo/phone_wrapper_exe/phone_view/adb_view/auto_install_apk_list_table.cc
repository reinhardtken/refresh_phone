// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "auto_install_apk_list_table.h"

#include <vector>

#include "base/string_util.h"
#include "base/string_tokenizer.h"
#include "base/string_number_conversions.h"
#include "base/sys_string_conversions.h"
#include "base/stringprintf.h"
#include "third_party/skia/include/core/SkBitmap.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "ui/gfx/image/image_skia.h"
#include "ui/views/controls/button/checkbox.h"
#include "ui/views/layout/grid_layout.h"
#include "ui/views/controls/textfield/textfield.h"
#include "ui/base/theme_provider.h"

#include "grit/theme_resources.h"

#include "../../../phone_common/include/ctp_notification_types.h"
#include "../tabbed_pane_example.h"

namespace {

}


namespace views {
namespace examples {
  
  int ApkIRStatusModel2::RowCount() {
    return table_->RowCount2();
  }

  string16 ApkIRStatusModel2::GetText(int row, int column_id) {

    return table_->GetText2(row, column_id);
    return string16();
  }


  gfx::ImageSkia ApkIRStatusModel2::GetIcon(int row) {
    return table_->GetIcon2(row);
  }


  void ApkIRStatusModel2::OnSelectionChanged() {
    table_->OnFirstTableSelectionChanged();
  }

  std::wstring AutoInstallApkListTable::GetAutoModeText(bool auto_mode) {
    if (total_auto_) {
      if (auto_mode) {
        return L"停止";
      } else {
        return L"启动";
      }
    } else {
      if (auto_mode) {
        return L"禁用自动安装";
      } else {
        return L"启用自动安装";
      }
    }
  }
  //=================================================
  int AutoInstallApkListTable::g_current_mode_;
AutoInstallApkListTable::AutoInstallApkListTable(CTPViewBase *p, bool b)
  :CTPViewBase(L"Table"),
  pane_(p),
  total_auto_(b),
  table_(NULL),
  ALLOW_THIS_IN_INITIALIZER_LIST(model_apk_ir_(this)),
  auto_install_mode_(false),
  ThreadMessageFilter(true) {

  
  registrar_.Add(this, phone_module::NOTIFICATION_PHONE_TRANSFER_APK_UPDATE_INFO,
    content::NotificationService::AllSources());
  registrar_.Add(this, phone_module::NOTIFICATION_PHONE_TRANSFER_INSTALL_APK_DIGEST,
    content::NotificationService::AllSources());
  registrar_.Add(this, phone_module::NOTIFICATION_PHONE_TRANSFER_DEVICES_LIST,
    content::NotificationService::AllSources());

}

AutoInstallApkListTable::~AutoInstallApkListTable() {
  // Delete the view before the model.
  delete table_;
  table_ = NULL;

  delete table_apk_ir_;
  table_apk_ir_ = NULL;

}

void AutoInstallApkListTable::OnSelectedIndexChanged(Combobox* combobox) {

}

void AutoInstallApkListTable::CreateExampleView(View* container) {
  ui::ThemeProvider* tp = container->GetThemeProvider();
  alive_ = tp->GetImageSkiaNamed(IDR_UPDATE_UPTODATE);
  die_ = tp->GetImageSkiaNamed(IDR_UPDATE_FAIL);



  GridLayout* layout = new GridLayout(container);
  container->SetLayoutManager(layout);

  int index = 0;
  //====================================
  std::vector<ui::TableColumn> columns2;
  columns2.push_back(ui::TableColumn(0, L"手机",
    ui::TableColumn::LEFT, 150));
  columns2.push_back(ui::TableColumn(1, L"model",
    ui::TableColumn::LEFT, 150));
  columns2.push_back(ui::TableColumn(2, L"总数",
    ui::TableColumn::LEFT, 100));

  columns2.push_back(ui::TableColumn(3, L"成功总数",
    ui::TableColumn::LEFT, 100));

  columns2.push_back(ui::TableColumn(4, L"失败总数",
    ui::TableColumn::LEFT, 100));

  columns2.push_back(ui::TableColumn(5, L"总耗时",
    ui::TableColumn::LEFT, 100));

  columns2.push_back(ui::TableColumn(6, L"失败包",
    ui::TableColumn::LEFT, 600));
  
  

  table_apk_ir_ = new TableView(&model_apk_ir_, columns2, TEXT_ONLY, true, true, true);
  table_apk_ir_->SetObserver(&model_apk_ir_);
  


  ColumnSet* column_set = layout->AddColumnSet(index);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL, 1,
	  GridLayout::USE_PREF, 0, 0);
  

  layout->StartRow(0.5 /* expand */, index);
  layout->AddView(table_apk_ir_->CreateParentIfNecessary());
  //==================================
  ++index;

  std::vector<ui::TableColumn> columns;
  columns.push_back(ui::TableColumn(0, L"应用名称",
    ui::TableColumn::LEFT, 400));
  columns.push_back(ui::TableColumn(1, L"包名",
    ui::TableColumn::LEFT, 250));
  columns.push_back(ui::TableColumn(2, L"重试次数",
    ui::TableColumn::LEFT, 70));
  columns.push_back(ui::TableColumn(3, L"错误信息",
    ui::TableColumn::LEFT, 200));
  columns.push_back(ui::TableColumn(4, L"原始错误信息",
    ui::TableColumn::LEFT, 800));
  


  table_ = new TableView(this, columns, ICON_AND_TEXT, true, true, true);
  table_->SetObserver(this);


  column_set = layout->AddColumnSet(index);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL, 1,
    GridLayout::USE_PREF, 0, 0);


  layout->StartRow(0.5 /* expand */, index);
  layout->AddView(table_->CreateParentIfNecessary());
  //=================================================
  ++index;
  //check_update_apk_list_ = new TextButton(this, L" 检查网络包列表");
  //check_update_apk_list_->set_alignment(TextButton::ALIGN_CENTER);
  auto_mode_ = new TextButton(this, GetAutoModeText(auto_install_mode_));
  auto_mode_->set_alignment(TextButton::ALIGN_CENTER);
  auto_mode_->SetEnabled(false);

  column_set = layout->AddColumnSet(index);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
	  1.0f, GridLayout::USE_PREF, 0, 0);
  //column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
	 // 1.0f, GridLayout::USE_PREF, 0, 0);
  //column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
  //  1.0f, GridLayout::USE_PREF, 0, 0);
  //column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
  //  1.0f, GridLayout::USE_PREF, 0, 0);
  //column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
  //  1.0f, GridLayout::USE_PREF, 0, 0);


  layout->StartRow(0 /* expand */, index);

  layout->AddView(auto_mode_);

  //=======================================
 
  ++index;
  status_label_ = new Label();

  column_set = layout->AddColumnSet(index);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  layout->StartRow(0 /* expand */, index);

  layout->AddView(status_label_);
}

int AutoInstallApkListTable::RowCount() {
  return failed_list_.size();
}

int AutoInstallApkListTable::RowCount2() {
  return install_digest_data_.size();
}

string16 AutoInstallApkListTable::GetText(int row, int column_id) {
  if ((uint32)row >= failed_list_.size()) {
    return string16();
  }
  if (failed_list_[row].try_times > 0) {
    switch (column_id) {
    case 0: {
      return failed_list_[row].app_name;
      break;
    }
    case 1: {
      return failed_list_[row].package_name;
      break;
    }
    case 2: {
      return base::IntToString16(failed_list_[row].try_times);
      break;
    }
    case 3: {
      return failed_list_[row].user_message;
      break;
    }
    case 4: {
      return failed_list_[row].error_message;
      break;
    }
    default: {
      //DCHECK(false);
    }
    }
  } else {
    //成功的
    switch (column_id) {
    case 0: {
      return failed_list_[row].app_name;
      break;
    }
    case 1: {
      return failed_list_[row].package_name;
      break;
    }
    default: {
      //DCHECK(false);
    }
    }
  }
  
 

  return string16();
}


string16 AutoInstallApkListTable::GetText2(int row, int column_id) {

  if ((uint32)row >= install_digest_data_.size()) {
    return string16();
  }
  switch (column_id) {
  case 0: {
    return install_digest_data_[row].serial_number;
    break;
  }
  case 1: {
    return install_digest_data_[row].model;
    break;
  }
  case 2: {
    return base::IntToString16(install_digest_data_[row].total_number);
    break;
  }
  case 3: {
    return base::IntToString16(install_digest_data_[row].success_number);
    break;
  }
  case 4: {
    return base::IntToString16(install_digest_data_[row].failed_number);
    break;
  }
  case 5: {
    std::wstring tmp = base::IntToString16(install_digest_data_[row].time_cost);
    tmp.append(L" 秒");
    return tmp;
    break;
  }
  case 6: {
    std::wstring tmp;
    for (auto it = install_digest_data_[row].failed_list.begin(); it != install_digest_data_[row].failed_list.end(); ++it) {
      if (it->try_times > 0) {
        tmp.append(it->package_name).append(L",");
      }
    }
    
    return tmp;
    break;
  }

  default: {
    //DCHECK(false);
  }
  }


  return string16();
}


gfx::ImageSkia AutoInstallApkListTable::GetIcon(int row) {
  if ((int)failed_list_.size() > row) {
    return failed_list_[row].try_times > 0 ? *die_ : *alive_;
  }
  return *die_;
}

gfx::ImageSkia AutoInstallApkListTable::GetIcon2(int row) {
  //if ((uint32)row < apk_ir_data_.size()) {
  //  return apk_ir_data_[row].error_code == phone_module::ERROR_CODE_OK ? *alive_ : *die_;
  //}
  return gfx::ImageSkia();
}


void AutoInstallApkListTable::SetObserver(ui::TableModelObserver* observer) {}

void AutoInstallApkListTable::OnSelectionChanged() {

}

void AutoInstallApkListTable::OnDoubleClick() {
  /*if (table_->FirstSelectedRow() >= 0) {
    PrintStatus("Double Click: %s",
                UTF16ToASCII(GetText(table_->FirstSelectedRow(), 0)).c_str());
  }*/
}

void AutoInstallApkListTable::OnMiddleClick() {}

void AutoInstallApkListTable::OnKeyDown(ui::KeyboardCode virtual_keycode) {}

void AutoInstallApkListTable::OnTableViewDelete(TableView* table_view) {}

void AutoInstallApkListTable::OnTableView2Delete(TableView2* table_view) {}




void AutoInstallApkListTable::ButtonPressed(Button* sender, const ui::Event& event) {
	if (sender == auto_mode_) {
    bool old = auto_install_mode_;
    if (auto_install_mode_) {
      auto_install_mode_ = false;
    } else {
      auto_install_mode_ = true;
    }
    auto_mode_->SetText(GetAutoModeText(auto_install_mode_));
    if (total_auto_) {
      ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::CTP, new U2L_TotalAutoCmd(auto_install_mode_));
    } else {
      ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::CTP, new U2L_AutoApkInstallCmd(auto_install_mode_));
    }
	} 
 // else if (sender == install_apk_list_all_device_force_ ||
 //   sender == install_apk_list_all_device_) {
 //   std::wstring type = L"all";
 //   if (sender == install_apk_list_all_device_force_) {
 //     type = L"all force";
 //   }
 //   std::vector<phone_module::ApkInstallInfo> * date = new std::vector<phone_module::ApkInstallInfo>(data_);
 //   PointerWrapper<std::vector<phone_module::ApkInstallInfo>> tmp(date);
	//	ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::CTP, new U2L_ApkInstallCmd(type, tmp));
	//} else if (sender == clear_table_) {
 //   data_.clear();
 //   table_->OnModelChanged();
 // } else if (sender == clear_apk_ir_table_) {
 //   apk_ir_data_.clear();
 //   apk_ir_data_map_.clear();
 //   table_apk_ir_->OnModelChanged();
 // }
}



void AutoInstallApkListTable::Observe(int type,
  const content::NotificationSource& source,
  const content::NotificationDetails& details) {
  if (type == phone_module::NOTIFICATION_PHONE_TRANSFER_APK_UPDATE_INFO) {
    status_label_->SetText(content::Details<std::wstring const>(details)->c_str());
  } else if (type == phone_module::NOTIFICATION_PHONE_TRANSFER_INSTALL_APK_DIGEST) {
    InnerUpdateInstallApkDigest(*content::Details<phone_module::InstallDigest>(details).ptr());
  } else if (type == phone_module::NOTIFICATION_PHONE_TRANSFER_DEVICES_LIST) {
    InnerOnDeviceUpdate(*content::Details<phone_module::DevicesList>(details).ptr());
  }
  
}


void AutoInstallApkListTable::InnerOnDeviceUpdate(phone_module::DevicesList const & list) {
  //phone_module::DevicesList & list = *p.get();
  //pane_->OnUpdateDevicesList(p);
  //根据设备信息更新安装信息
  for (auto i = list.begin(); i != list.end(); ++i) {
    auto it = install_digest_map_.find((*i).serial_no);
    if (it == install_digest_map_.end()) {
      phone_module::InstallDigest digest;
      digest.serial_number = (*i).serial_no;
      digest.model = (*i).model;
      install_digest_data_.push_back(digest);
      install_digest_map_.insert(std::make_pair(digest.serial_number, install_digest_data_.size() - 1));
      table_apk_ir_->OnItemsAdded(install_digest_data_.size() - 1, 1);
      if (install_digest_data_.size()) {
        table_apk_ir_->Select(install_digest_data_.size() - 1);
      }
    }
  }
}

bool AutoInstallApkListTable::OnMessageReceived(IPC::Message const & msg) {

	//if (msg.routing_id() == MSG_ROUTING_CONTROL) {
	//	// Dispatch control messages.
	//	bool msg_is_ok = false;
	//	IPC_BEGIN_MESSAGE_MAP_EX(AutoInstallApkListTable, msg, msg_is_ok)

	//		//IPC_MESSAGE_HANDLER(L2U_ApkInstallInfo, OnUpdatePackageList)
 //     //IPC_MESSAGE_HANDLER(L2U_InstallApkDigest, OnUpdateInstallApkDigest)
 //     //IPC_MESSAGE_HANDLER(L2U_ApkTotalAutoModeInfoToString, OnApkUpdateInfoToString)
 //     //IPC_MESSAGE_HANDLER(L2U_DevicesList, OnDeviceUpdate)
 //     

	//		//IPC_MESSAGE_UNHANDLED_ERROR()
	//		IPC_END_MESSAGE_MAP_EX()

	//		if (msg_is_ok) {
	//			return true;
	//		}
	//}

  // Dispatch incoming messages to the appropriate RenderView/WidgetHost.
  //RenderWidgetHost* rwh = render_widget_hosts_.Lookup(msg.routing_id());

  return false; 

}


//TaskManagerTableModel
void AutoInstallApkListTable::EnsureVisible() {

}



void AutoInstallApkListTable::Selected() {
  
}

void AutoInstallApkListTable::OnMarginRate(PointerWrapper<CThostFtdcInstrumentMarginRateField> const & p) {

}

void AutoInstallApkListTable::OnUpdatePackageList(PointerWrapper<std::vector<phone_module::ApkInstallInfo>> const & p) {
	//data_.swap(*p.get());
	//table_->OnModelChanged();
}

void AutoInstallApkListTable::InnerUpdateInstallApkDigest(phone_module::InstallDigest & digest) {
  auto it = install_digest_map_.find(digest.serial_number);
  if (it != install_digest_map_.end()) {
    install_digest_data_[it->second] = digest;
    table_apk_ir_->OnItemsChanged(it->second, 1);
  } else {
    install_digest_data_.push_back(digest);
    install_digest_map_.insert(std::make_pair(digest.serial_number, install_digest_data_.size() - 1));
    table_apk_ir_->OnItemsAdded(install_digest_data_.size() - 1, 1);
    if (install_digest_data_.size()) {
      table_apk_ir_->Select(install_digest_data_.size() - 1);
    }
  }

  if (current_serial_number_ == digest.serial_number) {
    failed_list_ = digest.failed_list;
    table_->OnModelChanged();
  }
}

void AutoInstallApkListTable::OnUpdateInstallApkDigest(PointerWrapper<phone_module::InstallDigest> const & p) {
  phone_module::InstallDigest & digest = *p.get();
  //当前是手动模式，本实例是手动实例
  if (g_current_mode_ == 1 && total_auto_ == false) {
    InnerUpdateInstallApkDigest(digest);
  } else {
    content::NotificationService::current()->Notify(
      phone_module::NOTIFICATION_PHONE_TRANSFER_INSTALL_APK_DIGEST,
      content::Source<AutoInstallApkListTable>(this),
      content::Details<phone_module::InstallDigest const>(&digest));
  }
  
  
  
}

void AutoInstallApkListTable::OnFirstTableSelectionChanged() {
  int select = table_apk_ir_->FirstSelectedRow();
  if (select >= 0 && select < (int)install_digest_data_.size()) {
    current_serial_number_ = install_digest_data_[select].serial_number;
    failed_list_ = install_digest_data_[select].failed_list;
    table_->OnModelChanged();
  } else {
    current_serial_number_.clear();
    failed_list_.clear();
    table_->OnModelChanged();
  }
}

void AutoInstallApkListTable::OnApkUpdateInfoToString(std::wstring const & s) {
  //当前是手动模式，本实例是手动实例
  if (g_current_mode_ == 1 && total_auto_ == false) {
    status_label_->SetText(s);
  } else {
    content::NotificationService::current()->Notify(
      phone_module::NOTIFICATION_PHONE_TRANSFER_APK_UPDATE_INFO,
      content::Source<AutoInstallApkListTable>(this),
      content::Details<std::wstring const>(&s));
  }
  
}

}  // namespace examples
}  // namespace views
