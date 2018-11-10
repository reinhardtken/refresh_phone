// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "install_apk_list_table.h"

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

  int ApkIRStatusModel::RowCount() {
    return table_->RowCount2();
  }

  string16 ApkIRStatusModel::GetText(int row, int column_id) {

    return table_->GetText2(row, column_id);
    return string16();
  }


  gfx::ImageSkia ApkIRStatusModel::GetIcon(int row) {
    return table_->GetIcon2(row);
  }

  //=================================================
InstallApkListTable::InstallApkListTable(CTPTabbedPane *p, std::wstring const &) 
  :CTPViewBase("Table"),
  pane_(p),
  table_(NULL),
  ALLOW_THIS_IN_INITIALIZER_LIST(model_apk_ir_(this)),
  ThreadMessageFilter(true) {


    /*if (pane_->HasID(bc_)) {
      ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::CTP,
        new CTP_TU_QryTypeList(pane_->GetID(bc_)));
    } else {
      DCHECK(false);
    }*/

}

InstallApkListTable::~InstallApkListTable() {
  // Delete the view before the model.
  delete table_;
  table_ = NULL;

  delete table_apk_ir_;
  table_apk_ir_ = NULL;

}

void InstallApkListTable::OnSelectedIndexChanged(Combobox* combobox) {

}

void InstallApkListTable::CreateExampleView(View* container) {
  ui::ThemeProvider* tp = container->GetThemeProvider();
  alive_ = tp->GetImageSkiaNamed(IDR_UPDATE_UPTODATE);
  die_ = tp->GetImageSkiaNamed(IDR_UPDATE_FAIL);



  GridLayout* layout = new GridLayout(container);
  container->SetLayoutManager(layout);

  int index = 0;
  //====================================
  std::vector<ui::TableColumn> columns2;
  columns2.push_back(ui::TableColumn(0, L"包名",
    ui::TableColumn::LEFT, 250));

  columns2.push_back(ui::TableColumn(1, L"名称",
    ui::TableColumn::LEFT, 150));

  columns2.push_back(ui::TableColumn(2, L"MD5",
    ui::TableColumn::LEFT, 100));

  columns2.push_back(ui::TableColumn(3, L"描述",
    ui::TableColumn::LEFT, 300));

  columns2.push_back(ui::TableColumn(4, L"价格",
    ui::TableColumn::LEFT, 100));

  columns2.push_back(ui::TableColumn(5, L"操作类型",
    ui::TableColumn::LEFT, 150));
  
  

  table_ = new TableView(this, columns2, TEXT_ONLY, true, true, true);
  table_->SetObserver(this);


  ColumnSet* column_set = layout->AddColumnSet(index);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL, 1,
	  GridLayout::USE_PREF, 0, 0);
  

  layout->StartRow(0.5 /* expand */, index);
  layout->AddView(table_->CreateParentIfNecessary());
  //=================================================
  ++index;

  std::vector<ui::TableColumn> columns;
  columns.push_back(ui::TableColumn(0, L"时间",
    ui::TableColumn::LEFT, 100));
  columns.push_back(ui::TableColumn(1, L"设备ID",
    ui::TableColumn::LEFT, 150));
  columns.push_back(ui::TableColumn(2, L"包名",
    ui::TableColumn::LEFT, 300));
  columns.push_back(ui::TableColumn(3, L"操作",
    ui::TableColumn::LEFT, 200));
  columns.push_back(ui::TableColumn(4, L"阶段",
    ui::TableColumn::LEFT, 70));
  columns.push_back(ui::TableColumn(5, L"进度/错误信息",
    ui::TableColumn::LEFT, 300));
  columns.push_back(ui::TableColumn(6, L"结果",
    ui::TableColumn::LEFT, 200));
  columns.push_back(ui::TableColumn(7, L"错误码",
    ui::TableColumn::LEFT, 200));

  table_apk_ir_ = new TableView(&model_apk_ir_, columns, ICON_AND_TEXT, true, true, true);
  table_apk_ir_->SetObserver(&model_apk_ir_);


  column_set = layout->AddColumnSet(index);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL, 1,
    GridLayout::USE_PREF, 0, 0);


  layout->StartRow(0.5 /* expand */, index);
  layout->AddView(table_apk_ir_->CreateParentIfNecessary());
  //=================================================
  ++index;
  //check_update_apk_list_ = new TextButton(this, L" 检查网络包列表");
  //check_update_apk_list_->set_alignment(TextButton::ALIGN_CENTER);
  get_apk_list_ = new TextButton(this, L" 获取本地包列表");
  get_apk_list_->set_alignment(TextButton::ALIGN_CENTER);
  install_apk_list_ = new TextButton(this, L"装包-单台");
  install_apk_list_->set_alignment(TextButton::ALIGN_CENTER);
  install_apk_list_all_device_ = new TextButton(this, L" 装包-所有");
  install_apk_list_all_device_->set_alignment(TextButton::ALIGN_CENTER);
  clear_table_ = new TextButton(this, L"清除包列表显示");
  clear_table_->set_alignment(TextButton::ALIGN_CENTER);
  clear_apk_ir_table_ = new TextButton(this, L"清除安装进度显示");
  clear_apk_ir_table_->set_alignment(TextButton::ALIGN_CENTER);

  column_set = layout->AddColumnSet(index);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
	  1.0f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
	  1.0f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);


  layout->StartRow(0 /* expand */, index);

  layout->AddView(get_apk_list_);
  layout->AddView(install_apk_list_);
  layout->AddView(install_apk_list_all_device_);
  layout->AddView(clear_table_);
  layout->AddView(clear_apk_ir_table_);
}

int InstallApkListTable::RowCount() {
  return data_.size();
}

int InstallApkListTable::RowCount2() {
  return apk_ir_data_.size();
}

string16 InstallApkListTable::GetText(int row, int column_id) {
  if ((uint32)row >= data_.size()) {
    return string16();
  }
  switch (column_id) {
      case 0: {
		  return data_[row].package_name;
        break;
      }
      case 1: {
        return data_[row].name;
        break;
      }
      case 2: {
        return data_[row].md5;
        break;
      }
      case 3: {
        return data_[row].brief;
        break;
      }
      case 4: {
        return ASCIIToUTF16(base::DoubleToString(data_[row].price));
        break;
      }
      case 5: {
        if (data_[row].type == phone_module::PACKAGE_BOTH) {
          return L"先删除/后安装";
        } else if (data_[row].type == phone_module::PACKAGE_INSTALL) {
          return L"仅安装";
        } else {
          return L"仅删除";
        }
        break;
      }
      default: {
        //DCHECK(false);
      }
  }
 

  return string16();
}


string16 InstallApkListTable::GetText2(int row, int column_id) {

  if ((uint32)row >= apk_ir_data_.size()) {
    return string16();
  }
  switch (column_id) {
  case 0: {
    return apk_ir_data_[row].time_string;
    break;
  }
  case 1: {
    return apk_ir_data_[row].serial_no;
    break;
  }
  case 2: {
    return apk_ir_data_[row].package_name;
    break;
  }
  case 3: {
    return apk_ir_data_[row].op;
    break;
  }
  case 4: {
    return apk_ir_data_[row].stage;
    break;
  }
  case 5: {
    /*return ASCIIToUTF16(base::DoubleToString(apk_ir_data_[row].percent));*/
    return apk_ir_data_[row].percent;
    break;
  }
  case 6: {
    return apk_ir_data_[row].result;
    break;
  }
  case 7: {
    return ASCIIToUTF16(base::IntToString(apk_ir_data_[row].error_code));
    break;
  }
  default: {
    //DCHECK(false);
  }
  }


  return string16();
}


gfx::ImageSkia InstallApkListTable::GetIcon(int row) {
  DCHECK(false);
  return gfx::ImageSkia();
  //return row % 2 ? gfx::ImageSkia(icon1_) : gfx::ImageSkia(icon2_);
}

gfx::ImageSkia InstallApkListTable::GetIcon2(int row) {
  if ((uint32)row < apk_ir_data_.size()) {
    return apk_ir_data_[row].error_code == phone_module::ERROR_CODE_OK ? *alive_ : *die_;
  }
  return gfx::ImageSkia();
}


void InstallApkListTable::SetObserver(ui::TableModelObserver* observer) {}

void InstallApkListTable::OnSelectionChanged() {

}

void InstallApkListTable::OnDoubleClick() {
  /*if (table_->FirstSelectedRow() >= 0) {
    PrintStatus("Double Click: %s",
                UTF16ToASCII(GetText(table_->FirstSelectedRow(), 0)).c_str());
  }*/
}

void InstallApkListTable::OnMiddleClick() {}

void InstallApkListTable::OnKeyDown(ui::KeyboardCode virtual_keycode) {}

void InstallApkListTable::OnTableViewDelete(TableView* table_view) {}

void InstallApkListTable::OnTableView2Delete(TableView2* table_view) {}

void InstallApkListTable::ButtonPressed(Button* sender, const ui::Event& event) {
	if (sender == get_apk_list_) {
		ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::CTP, new U2L_GetLocalInstallApkList());
	} else if (sender == install_apk_list_ ||
    sender == install_apk_list_all_device_) {
    std::wstring type = L"all";
    if (sender == install_apk_list_) {
      type = L"first";
    }
    std::vector<phone_module::ApkInstallInfo> * date = new std::vector<phone_module::ApkInstallInfo>(data_);
    PointerWrapper<std::vector<phone_module::ApkInstallInfo>> tmp(date);
		ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::CTP, new U2L_ApkInstallCmd(type, tmp));
	} else if (sender == clear_table_) {
    data_.clear();
    table_->OnModelChanged();
  } else if (sender == clear_apk_ir_table_) {
    apk_ir_data_.clear();
    apk_ir_data_map_.clear();
    table_apk_ir_->OnModelChanged();
  }
}



void InstallApkListTable::Observe(int type,
  const content::NotificationSource& source,
  const content::NotificationDetails& details) {

}

bool InstallApkListTable::OnMessageReceived(IPC::Message const & msg) {

	if (msg.routing_id() == MSG_ROUTING_CONTROL) {
		// Dispatch control messages.
		bool msg_is_ok = false;
		IPC_BEGIN_MESSAGE_MAP_EX(InstallApkListTable, msg, msg_is_ok)

			IPC_MESSAGE_HANDLER(L2U_ApkInstallInfo, OnUpdatePackageList)
      IPC_MESSAGE_HANDLER(L2U_ApkIRStatus, OnUpdateApkIRStatus)
      

			//IPC_MESSAGE_UNHANDLED_ERROR()
			IPC_END_MESSAGE_MAP_EX()

			if (msg_is_ok) {
				return true;
			}
	}

  // Dispatch incoming messages to the appropriate RenderView/WidgetHost.
  //RenderWidgetHost* rwh = render_widget_hosts_.Lookup(msg.routing_id());

  return false; 

}


//TaskManagerTableModel
void InstallApkListTable::EnsureVisible() {

}



void InstallApkListTable::Selected() {
  
}

void InstallApkListTable::OnMarginRate(PointerWrapper<CThostFtdcInstrumentMarginRateField> const & p) {

}

void InstallApkListTable::OnUpdatePackageList(PointerWrapper<std::vector<phone_module::ApkInstallInfo>> const & p) {
	data_.swap(*p.get());
	table_->OnModelChanged();
}

void InstallApkListTable::OnUpdateApkIRStatus(PointerWrapper<phone_module::ApkIRStatus> const & p) {
  phone_module::ApkIRStatus & status = *p.get();
  auto it = apk_ir_data_map_.find(status.get_key());
  if (it != apk_ir_data_map_.end()) {
    apk_ir_data_[it->second] = status;
    table_apk_ir_->OnItemsChanged(it->second, 1);
  } else {
    apk_ir_data_.push_back(status);
    apk_ir_data_map_.insert(std::make_pair(status.get_key(), apk_ir_data_.size() - 1));
    table_apk_ir_->OnItemsAdded(apk_ir_data_.size() - 1, 1);
    if (apk_ir_data_.size()) {
      table_apk_ir_->Select(apk_ir_data_.size() - 1);
    }
  }
}

}  // namespace examples
}  // namespace views
