// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "main_view.h"

#include <vector>

#include "base/string_util.h"
#include "base/string_tokenizer.h"
#include "base/string_number_conversions.h"
#include "base/sys_string_conversions.h"
#include "base/utf_string_conversions.h"
#include "base/stringprintf.h"
#include "third_party/skia/include/core/SkBitmap.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "ui/gfx/image/image_skia.h"
#include "ui/views/controls/button/checkbox.h"
#include "ui/views/layout/grid_layout.h"
#include "ui/views/controls/textfield/textfield.h"

#include "../../../phone_common/include/ctp_notification_types.h"
#include "../ui_common.h"
//#include "tabbed_pane_example.h"

namespace {

}


namespace views {
namespace examples {

  int DeviceModel::RowCount() {
    return delegate_->size();
  }

  string16 DeviceModel::GetText(int row, int column_id) {
    return delegate_->text(row, column_id);
  }


  gfx::ImageSkia DeviceModel::GetIcon(int row) {
    DCHECK(false);
    return gfx::ImageSkia();
  }

  //=====================================================
  void MakeupDeviceColunms(std::vector<ui::TableColumn> &columns, bool const global) {

    int index = 0;
    if (global) {
      columns.push_back(ui::TableColumn(index++, L"账号",
        ui::TableColumn::LEFT, 200));
    }
    columns.push_back(ui::TableColumn(index++, L"时间",
      ui::TableColumn::LEFT, 150));

    columns.push_back(ui::TableColumn(index++, L"设备ID",
      ui::TableColumn::LEFT, 150));
    columns.push_back(ui::TableColumn(index++, L"最新信息",
      ui::TableColumn::LEFT, 500));
    
  }

  //-====================================
  int StatusModel::RowCount() {
    return table_->status_info_data_.size();
    return 0;
  }

  string16 StatusModel::GetText(int row, int column_id) {

    return table_->GetText(row, column_id);
    return string16();
  }


  gfx::ImageSkia StatusModel::GetIcon(int row) {
    DCHECK(false);
    return gfx::ImageSkia();
  }
 
  //===========================================================
  
  //================================================================

MainView::MainView(CTPTabbedPane *p, std::string const &bc) 
  :CTPViewBase("Table"),
  pane_(p),
  bc_(bc),
  table_order_(NULL),
  table_position_(NULL),
  ALLOW_THIS_IN_INITIALIZER_LIST(model_position_(this)),
  ALLOW_THIS_IN_INITIALIZER_LIST(model_order_(this)),
  ThreadMessageFilter(true) {

 
    //========================================================
    registrar_.Add(this, phone_module::NOTIFICATION_CTP_MQ_MQLEVEL_INFO,
      content::NotificationService::AllSources());

}

MainView::~MainView() {
  // Delete the view before the model.
  delete table_order_;
  table_order_ = NULL;
  delete table_position_;
  table_position_ = NULL;
}

void MainView::OnSelectedIndexChanged(Combobox* combobox) {

}

void MainView::CreateExampleView(View* container) {


  GridLayout* layout = new GridLayout(container);
  container->SetLayoutManager(layout);

  //====================================
  std::vector<ui::TableColumn> columns2;
  columns2.push_back(ui::TableColumn(0, L"时间",
    ui::TableColumn::LEFT, 100));
  columns2.push_back(ui::TableColumn(1, L"设备ID",
    ui::TableColumn::LEFT, 150));
  columns2.push_back(ui::TableColumn(2, L"操作",
    ui::TableColumn::LEFT, 200));
  columns2.push_back(ui::TableColumn(3, L"结果",
    ui::TableColumn::LEFT, 400));
  
  //----------------------------------------------------------------------
  std::vector<ui::TableColumn> columns;

  MakeupDeviceColunms(columns, false);
  //==============================================
  table_order_ = new TableView(&model_order_, columns2, TEXT_ONLY, true, true, true);
  table_order_->SetObserver(&model_order_);

  table_position_ = new CCTableView(this, &model_position_, columns, TEXT_ONLY, true, true, true);
  table_position_->SetObserver(&model_position_);


  //=========================================
 
  int index = 0;
  ColumnSet* column_set = layout->AddColumnSet(index);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL, 1,
                        GridLayout::USE_PREF, 0, 0);
  layout->StartRow(0.7f /* expand */, index);
  layout->AddView(table_order_->CreateParentIfNecessary());

  //------------------------------------------------------
  ++index;
  column_set = layout->AddColumnSet(index);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL, 1,
    GridLayout::USE_PREF, 0, 0);
  layout->StartRow(0.3f, index);
  layout->AddView(table_position_->CreateParentIfNecessary());
  //=========================================


  ++index;

  clear_table_ = new TextButton(this, L"清除显示");
  clear_table_->set_alignment(TextButton::ALIGN_CENTER);

  column_set = layout->AddColumnSet(index);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);

  layout->StartRow(0 /* expand */, index);
  layout->AddView(clear_table_);



}

int MainView::RowCount() {

  return status_info_data_.size();
  return 0;
}

string16 MainView::GetText(int row, int column_id) {
  return GetColumnText(column_id, *status_info_data_[row].get());
}

string16 MainView::GetColumnText(int id, phone_module::StatusInfo const & info) {
  switch (id) {
  case 0: {
    return info.time_string;
    break;
  }
  case 1: {
    return info.serial_no;
    break;
  }
  case 2: {
    return info.op;
    break;
  }
  case 3: {
    return info.result;
    break;
  }
  default: {
    DCHECK(false);
  }
  }
 

  return string16();
}


string16 MainView::GetColumnTextPosition(int id, phone_module::DeviceData const & info) {
  switch (id) {
      case 0: {
        return info.time_string;
        break;
      }
      case 1: {
        return info.serial_no;
        break;
      }
      case 2: {
        return info.info;
        break;
      }
      default: {
        DCHECK(false);
      }
  }
 

  return string16();
}

gfx::ImageSkia MainView::GetIcon(int row) {
  DCHECK(false);
  return gfx::ImageSkia();
  //return row % 2 ? gfx::ImageSkia(icon1_) : gfx::ImageSkia(icon2_);
}

void MainView::SetObserver(ui::TableModelObserver* observer) {}

void MainView::OnSelectionChanged() {

}

void MainView::OnDoubleClick() {

}

void MainView::OnMiddleClick() {}

void MainView::OnKeyDown(ui::KeyboardCode virtual_keycode) {}

void MainView::OnTableViewDelete(TableView* table_view) {}

void MainView::OnTableView2Delete(TableView2* table_view) {}

void MainView::ButtonPressed(Button* sender, const ui::Event& event) {
  if (sender == clear_table_) {
    status_info_data_.clear();
    table_order_->OnModelChanged();
  }
}



void MainView::Observe(int type,
  const content::NotificationSource& source,
  const content::NotificationDetails& details) {
    if (type == phone_module::NOTIFICATION_CTP_MQ_MQLEVEL_INFO) {

    }
}

bool MainView::OnMessageReceived(IPC::Message const & msg) {

  if (msg.routing_id() == MSG_ROUTING_CONTROL) {
    // Dispatch control messages.
    bool msg_is_ok = false;
    IPC_BEGIN_MESSAGE_MAP_EX(MainView, msg, msg_is_ok)

      IPC_MESSAGE_HANDLER(L2U_DeviceUpdate, OnDeviceUpdate)
      IPC_MESSAGE_HANDLER(L2U_StatusInfo, OnStatusInfo)
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
void MainView::EnsureVisible() {
  if (status_info_data_.size()) {
    //table_order_->Select(list_data_.size() - 1);
    table_order_->Select(status_info_data_.size() - 1);
  }
}



void MainView::OnStatusInfo(PointerWrapper<phone_module::StatusInfo> const & p) {
  status_info_data_.push_back(std::shared_ptr<phone_module::StatusInfo>(new phone_module::StatusInfo(*p.get())));
  table_order_->OnItemsAdded(status_info_data_.size() - 1, 1);
  EnsureVisible();
}

void MainView::OnInitParam(std::string const & s) {

}


bool MainView::OverrideThreadForMessage(IPC::Message const& message,
  CommonThread::ID* thread) {
/*
    if (message.type() == CTP_CONSOLE_Command::ID || 
      message.type() == CTP_CONSOLE_CommandProtoBuf::ID) {
        *thread = CommonThread::UI;
        return true;
    }*/

    return false;
}

void MainView::OnDeviceUpdate(PointerWrapper<phone_module::DeviceData> const & p) {
  std::wstring key = p.get()->serial_no;
  uint32 const old_size = device_data_.size();
  device_data_[key] = std::shared_ptr<phone_module::DeviceData>(new phone_module::DeviceData(*p.get()));
  uint32 const new_size = device_data_.size();
  if (device_keys_.end() == std::find(device_keys_.begin(), device_keys_.end(), key)) {
    device_keys_.push_back(key);
  }

  if (old_size == new_size) {
    table_position_->OnItemsChanged(0, device_data_.size());
  } else {
    table_position_->OnModelChanged();
  }

  //if (list_data_.size() > 0) {
  //table_->ScrollRectToVisible(table_->GetRowBounds(list_data_.size() - 1));
  //table_->Select(list_data_.size() - 1);
  //}
  EnsureVisible();

}


void MainView::Selected() {
  //manual_type_checkbox_->ModelChanged();
  //manual_direction_checkbox_->ModelChanged();
 //manual_oc_checkbox_->ModelChanged();
}

bool MainView::GetCellColors(
  CCTableView* who,
  int model_row,
  int column,
  CCTableView::ItemColor* foreground,
  CCTableView::ItemColor* background,
  LOGFONT* logfont) {
    //DCHECK(who == table_position_);
/*
    DataList::iterator it = order_result_data_.find(keys_[model_row]);
    
    if (it != order_result_data_.end()) {
      ColorMap::iterator it_color = position_color_map_.find(it->first);
      if (it_color != position_color_map_.end()) {
        foreground->color_is_set = true;
        foreground->color = it_color->second;
        return true;
      }
      
    }*/

    return false;
}

int MainView::size() {
  return device_keys_.size();
}
string16 MainView::text(int row, int column_id) {
  int type_select_index = 0;//type_combobox_->selected_index();
  int level_select_index = 0;//strategy_combobox_->selected_index();

  DCHECK(device_data_.size() == device_keys_.size());
  if (type_select_index == 0 && level_select_index == 0) {
    if ((uint32)row < device_keys_.size()) {
      std::wstring key = device_keys_[row];
      return GetColumnTextPosition(column_id, * device_data_[key].get());
    } else {
      DCHECK(false);
    }
  } 



  return string16();
}

}  // namespace examples
}  // namespace views
