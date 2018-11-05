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

#include "../../../phone_common/include/ctp_notification_types.h"
#include "../tabbed_pane_example.h"

namespace {

}


namespace views {
namespace examples {


InstallApkListTable::InstallApkListTable(CTPTabbedPane *p, std::wstring const &) 
  :CTPViewBase("Table"),
  pane_(p),
  table_(NULL),
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

}

void InstallApkListTable::OnSelectedIndexChanged(Combobox* combobox) {

}

void InstallApkListTable::CreateExampleView(View* container) {

  GridLayout* layout = new GridLayout(container);
  container->SetLayoutManager(layout);


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


  ColumnSet* column_set = layout->AddColumnSet(0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL, 1,
	  GridLayout::USE_PREF, 0, 0);
  

  layout->StartRow(1 /* expand */, 0);
  layout->AddView(table_->CreateParentIfNecessary());

  //=================================================
  //check_update_apk_list_ = new TextButton(this, L" 检查网络包列表");
  //check_update_apk_list_->set_alignment(TextButton::ALIGN_CENTER);
  get_apk_list_ = new TextButton(this, L" 获取本地包列表");
  get_apk_list_->set_alignment(TextButton::ALIGN_CENTER);
  install_apk_list_ = new TextButton(this, L" 安装/删除包列表");
  install_apk_list_->set_alignment(TextButton::ALIGN_CENTER);
  clear_table_ = new TextButton(this, L"清除显示");
  clear_table_->set_alignment(TextButton::ALIGN_CENTER);


  column_set = layout->AddColumnSet(1);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
	  1.0f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
	  1.0f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);


  layout->StartRow(0 /* expand */, 1);

  layout->AddView(get_apk_list_);
  layout->AddView(install_apk_list_);
  layout->AddView(clear_table_);
}

int InstallApkListTable::RowCount() {
  return data_.size();
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



gfx::ImageSkia InstallApkListTable::GetIcon(int row) {
  DCHECK(false);
  return gfx::ImageSkia();
  //return row % 2 ? gfx::ImageSkia(icon1_) : gfx::ImageSkia(icon2_);
}

void InstallApkListTable::SetObserver(ui::TableModelObserver* observer) {}

void InstallApkListTable::OnSelectionChanged() {
  /*if (table_->FirstSelectedRow() >= 0) {
    PrintStatus("Selected: %s",
                UTF16ToASCII(GetText(table_->FirstSelectedRow(), 0)).c_str());
  }*/
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
	} else if (sender == install_apk_list_) {

    std::vector<phone_module::ApkInstallInfo> * date = new std::vector<phone_module::ApkInstallInfo>(data_);
    PointerWrapper<std::vector<phone_module::ApkInstallInfo>> tmp(date);
		ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::CTP, new U2L_ApkInstallCmd(tmp));
	} else if (sender == clear_table_) {
    data_.clear();
    table_->OnModelChanged();
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
  
  uint32 const old_size = map_.size();
  std::shared_ptr<CThostFtdcInstrumentMarginRateField> sp(new CThostFtdcInstrumentMarginRateField(*p.get()));
  map_[p.get()->InstrumentID] = sp;
  uint32 const new_size = map_.size();
  if (old_size != new_size) {
    list_.push_back(sp);
  }
  

  if (old_size == new_size) {
    table_->OnItemsChanged(0, map_.size());
  } else {
    table_->OnModelChanged();
  }
}

void InstallApkListTable::OnUpdatePackageList(PointerWrapper<std::vector<phone_module::ApkInstallInfo>> const & p) {
	data_.swap(*p.get());
	table_->OnModelChanged();
}

}  // namespace examples
}  // namespace views
