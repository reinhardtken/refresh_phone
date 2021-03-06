// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "auto_main_view.h"

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
#include "ui/views/widget/widget.h"
#include "ui/views/controls/button/checkbox.h"
#include "ui/views/layout/grid_layout.h"
#include "ui/views/controls/textfield/textfield.h"

#include "../../../phone_common/include/ctp_notification_types.h"
#include "../ui_common.h"


#include "../tabbed_pane_example.h"

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

  void DeviceModel::OnDoubleClick() {
    delegate_->OnDoubleClick();
  }

  //=====================================================
  void MakeupDeviceColunms(std::vector<ui::TableColumn> &columns) {

    int index = 0;
    columns.push_back(ui::TableColumn(index++, L"时间",
      ui::TableColumn::LEFT, 150));

    columns.push_back(ui::TableColumn(index++, L"设备ID",
      ui::TableColumn::LEFT, 150));
    columns.push_back(ui::TableColumn(index++, L"product",
      ui::TableColumn::LEFT, 150));
    columns.push_back(ui::TableColumn(index++, L"model",
      ui::TableColumn::LEFT, 150));
    columns.push_back(ui::TableColumn(index++, L"device",
      ui::TableColumn::LEFT, 150));
    
  }

  
  //-====================================
  int AutoStatusModel::RowCount() {
    return table_->status_info_data_.size();
    return 0;
  }

  string16 AutoStatusModel::GetText(int row, int column_id) {

    return table_->GetText(row, column_id);
    return string16();
  }


  gfx::ImageSkia AutoStatusModel::GetIcon(int row) {
    DCHECK(false);
    return gfx::ImageSkia();
  }

 
  //===========================================================
  
  //================================================================

AutoMainView::AutoMainView(CTPTabbedPane *p, std::string const &bc) 
  :CTPViewBase("Table"),
  pane_(p),
  bc_(bc),
  table_order_(NULL),
  table_device_(NULL),
  ALLOW_THIS_IN_INITIALIZER_LIST(model_device_(this)),
  ALLOW_THIS_IN_INITIALIZER_LIST(model_order_(this)),
  ThreadMessageFilter(true) {

 
    //========================================================
    registrar_.Add(this, phone_module::NOTIFICATION_CTP_MQ_MQLEVEL_INFO,
      content::NotificationService::AllSources());

}

AutoMainView::~AutoMainView() {
  // Delete the view before the model.
  delete table_order_;
  table_order_ = NULL;
  delete table_device_;
  table_device_ = NULL;
}

void AutoMainView::OnSelectedIndexChanged(Combobox* combobox) {

}

void AutoMainView::CreateExampleView(View* container) {


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

  MakeupDeviceColunms(columns);
  //==============================================
  table_order_ = new TableView(&model_order_, columns2, TEXT_ONLY, true, true, true);
  table_order_->SetObserver(&model_order_);

  table_device_ = new CCTableView(this, &model_device_, columns, TEXT_ONLY, true, true, true);
  table_device_->SetObserver(&model_device_);


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
  layout->AddView(table_device_->CreateParentIfNecessary());
  //=========================================


  ++index;


  //refresh_ = new TextButton(this, L"刷机-选中");
  //refresh_->set_alignment(TextButton::ALIGN_CENTER);
  //refresh_->SetEnabled(false);

  refresh_all_ = new TextButton(this, L"刷机-所有");
  refresh_all_->set_alignment(TextButton::ALIGN_CENTER);
  //refresh_all_->SetEnabled(false);

  clear_table_ = new TextButton(this, L"清除显示");
  clear_table_->set_alignment(TextButton::ALIGN_CENTER);
  

  column_set = layout->AddColumnSet(index);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  //column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
  //  1.0f, GridLayout::USE_PREF, 0, 0);

  layout->StartRow(0 /* expand */, index);
  //layout->AddView(refresh_);
  layout->AddView(refresh_all_);
  layout->AddView(clear_table_);



}

int AutoMainView::RowCount() {

  return status_info_data_.size();
  return 0;
}

string16 AutoMainView::GetText(int row, int column_id) {
  return GetColumnText(column_id, *status_info_data_[row].get());
}

string16 AutoMainView::GetColumnText(int id, phone_module::StatusInfo const & info) {
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


string16 AutoMainView::GetColumnTextPosition(int id, phone_module::AdbDevice const & info) {
  switch (id) {
      case 0: {
        base::Time::Exploded e;
        base::Time::Now().LocalExplode(&e);
        return base::StringPrintf(L"%02d:%02d:%02d", e.hour, e.minute, e.second);
        break;
      }
      case 1: {
        return info.serial_no;
        break;
      }
      case 2: {
        return info.product;
        break;
      }
      case 3: {
        return info.model;
        break;
      }
      case 4: {
        return info.device;
        break;
      }
      default: {
        DCHECK(false);
      }
  }
 

  return string16();
}

gfx::ImageSkia AutoMainView::GetIcon(int row) {
  DCHECK(false);
  return gfx::ImageSkia();
  //return row % 2 ? gfx::ImageSkia(icon1_) : gfx::ImageSkia(icon2_);
}

void AutoMainView::SetObserver(ui::TableModelObserver* observer) {}

void AutoMainView::OnSelectionChanged() {

}

void AutoMainView::OnDoubleClick() {
  uint32 index = table_device_->FirstSelectedRow();
  if (index < device_data_.size()) {
    ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::CTP, new U2L_Refresh(device_data_[index].serial_no));
  }
}

void AutoMainView::OnMiddleClick() {}

void AutoMainView::OnKeyDown(ui::KeyboardCode virtual_keycode) {}

void AutoMainView::OnTableViewDelete(TableView* table_view) {}

void AutoMainView::OnTableView2Delete(TableView2* table_view) {}

void AutoMainView::ButtonPressed(Button* sender, const ui::Event& event) {
  if (sender == clear_table_) {
    status_info_data_.clear();
    table_order_->OnModelChanged();
  }else if (sender == refresh_all_) {
    ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::CTP, new U2L_Refresh(L"all"));
  }
}



void AutoMainView::Observe(int type,
  const content::NotificationSource& source,
  const content::NotificationDetails& details) {
    if (type == phone_module::NOTIFICATION_CTP_MQ_MQLEVEL_INFO) {

    }
}

bool AutoMainView::OnMessageReceived(IPC::Message const & msg) {

  if (msg.routing_id() == MSG_ROUTING_CONTROL) {
    // Dispatch control messages.
    bool msg_is_ok = false;
    IPC_BEGIN_MESSAGE_MAP_EX(AutoMainView, msg, msg_is_ok)

      IPC_MESSAGE_HANDLER(L2U_DevicesList, OnDeviceUpdate)
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
void AutoMainView::EnsureVisible() {
  if (status_info_data_.size()) {
    //table_order_->Select(list_data_.size() - 1);
    table_order_->Select(status_info_data_.size() - 1);
  }
}



void AutoMainView::OnStatusInfo(PointerWrapper<phone_module::StatusInfo> const & p) {
  status_info_data_.push_back(std::shared_ptr<phone_module::StatusInfo>(new phone_module::StatusInfo(*p.get())));
  table_order_->OnItemsAdded(status_info_data_.size() - 1, 1);
  EnsureVisible();
}

void AutoMainView::OnInitParam(std::string const & s) {

}


bool AutoMainView::OverrideThreadForMessage(IPC::Message const& message,
  CommonThread::ID* thread) {
/*
    if (message.type() == CTP_CONSOLE_Command::ID || 
      message.type() == CTP_CONSOLE_CommandProtoBuf::ID) {
        *thread = CommonThread::UI;
        return true;
    }*/

    return false;
}



void AutoMainView::OnDeviceUpdate(PointerWrapper< phone_module::DevicesList> const & p) {
  phone_module::DevicesList & list = *p.get();
  pane_->OnUpdateDevicesList(p);

  std::size_t old_size = device_data_.size();
  device_data_.swap(list);
  
  if (old_size == device_data_.size() && old_size > 0) {
    table_device_->OnItemsChanged(0, device_data_.size());
  } else {
    table_device_->OnModelChanged();
  }

  
}


void AutoMainView::ShowContextMenuForView(views::View* source,
  const gfx::Point& point) {
  //UpdateStatsCounters();
  if (source == table_device_) {
    scoped_ptr<views::Menu> menu(views::Menu::Create(
      this, views::Menu::TOPLEFT, source->GetWidget()->GetNativeView()));

    if (device_data_.size()) {
      int index = 0;
      for (auto it = device_data_.begin(); it != device_data_.end(); ++it) {
        menu->AppendMenuItem(index, L"一键刷机：" + it->serial_no,
          views::Menu::NORMAL);
        index++;
      }
      menu->RunMenuAt(point.x(), point.y());
    }
  }
  
}

void AutoMainView::ExecuteCommand(int id) {
  //tab_table_->SetColumnVisibility(id, !tab_table_->IsColumnVisible(id));
  if (device_data_.size() > (uint32)id) {
  }
}


void AutoMainView::Selected() {
  //manual_type_checkbox_->ModelChanged();
  //manual_direction_checkbox_->ModelChanged();
 //manual_oc_checkbox_->ModelChanged();
}

bool AutoMainView::GetCellColors(
  CCTableView* who,
  int model_row,
  int column,
  CCTableView::ItemColor* foreground,
  CCTableView::ItemColor* background,
  LOGFONT* logfont) {
    //DCHECK(who == table_device_);
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

int AutoMainView::size() {
  return device_data_.size();
}
string16 AutoMainView::text(int row, int column_id) {
  return GetColumnTextPosition(column_id, device_data_[row]);
}

}  // namespace examples
}  // namespace views
