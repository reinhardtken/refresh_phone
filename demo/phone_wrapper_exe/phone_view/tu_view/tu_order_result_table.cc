// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "tu_order_result_table.h"

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

  

  void OneRecord2OrderData(phone_module::OrderFootprints::OneRecord const *record, views::examples::OrderResultData & out) {

  }
}


namespace views {
namespace examples {


  //-====================================
  int OrderResultModel::RowCount() {
    //return table_->order_result_data_.size();
    return 0;
  }

  string16 OrderResultModel::GetText(int row, int column_id) {

    return table_->GetText(row, column_id);
    return string16();
  }


  gfx::ImageSkia OrderResultModel::GetIcon(int row) {
    DCHECK(false);
    return gfx::ImageSkia();
  }
 
  //===========================================================
  
  //================================================================

OrderResultTable::OrderResultTable(CTPTabbedPane *p, std::string const &bc) 
  :CTPViewBase("Table"),
  pane_(p),
  bc_(bc),
  table_order_(NULL),
  table_position_(NULL),
  ALLOW_THIS_IN_INITIALIZER_LIST(model_position_(this)),
  ALLOW_THIS_IN_INITIALIZER_LIST(model_order_(this)),

  ThreadMessageFilter(true)/*,
这段代码高度危险，不推荐使用=
  strategy_combobox_(new Combobox(&strategy_combobox_model_)),
  type_combobox_(new Combobox(&type_combobox_model_))*/ {

  //strategy_combobox_ = (new Combobox(&strategy_combobox_model_));
  //type_combobox_ = (new Combobox(&type_combobox_model_));


  //strategy_combobox_model_.Add("MS.ALL");//必须是第一个


  //type_combobox_model_.Add("TYPE.ALL");//必须是第一个

  //===========================================
  //strategy_combobox_->set_listener(this);
  //type_combobox_->set_listener(this);


    
    
    /*manual_type_checkbox_model_.Add("IF1310");
    manual_type_checkbox_model_.Add("ru1401");
    manual_type_checkbox_model_.Add("rb1401");
    manual_type_checkbox_model_.Add("FG401");
    manual_type_checkbox_model_.Add("m1401");


    manual_direction_checkbox_model_.Add("buy");
    manual_direction_checkbox_model_.Add("sell");
    manual_oc_checkbox_model_.Add("open");
    manual_oc_checkbox_model_.Add("close");
    manual_oc_checkbox_model_.Add("close_yesterday");*/

    //manual_close_day_checkbox_model_.Add("today");
    //manual_close_day_checkbox_model_.Add("yesterday");
    //========================================================
    registrar_.Add(this, phone_module::NOTIFICATION_CTP_MQ_MQLEVEL_INFO,
      content::NotificationService::AllSources());

}

OrderResultTable::~OrderResultTable() {
  // Delete the view before the model.
  delete table_order_;
  table_order_ = NULL;
  delete table_position_;
  table_position_ = NULL;
}

void OrderResultTable::OnSelectedIndexChanged(Combobox* combobox) {

}

void OrderResultTable::CreateExampleView(View* container) {



  clear_order_button_ = new TextButton(this, L"Clear Order");

  cash_label_ = new Label(L"Available:");
  cash_label_->SetHorizontalAlignment(Label::ALIGN_LEFT);
  balance_label_ = new Label(L"Balance:");
  balance_label_->SetHorizontalAlignment(Label::ALIGN_LEFT);
  position_profit_label_ = new Label(L"PositionProfit:");
  position_profit_label_->SetHorizontalAlignment(Label::ALIGN_LEFT);


  GridLayout* layout = new GridLayout(container);
  container->SetLayoutManager(layout);

  //====================================
  std::vector<ui::TableColumn> columns2;
  columns2.push_back(ui::TableColumn(0, L"msid",
    ui::TableColumn::LEFT, 120));
  columns2.push_back(ui::TableColumn(1, L"osid",
    ui::TableColumn::LEFT, 120));
  columns2.push_back(ui::TableColumn(2, L"msname",
    ui::TableColumn::LEFT, 50));
  columns2.push_back(ui::TableColumn(3, L"osname",
    ui::TableColumn::LEFT, 110));
  columns2.push_back(ui::TableColumn(4, L"ms发生时间",
    ui::TableColumn::LEFT, 100));
  columns2.push_back(ui::TableColumn(5, L"os开始时间",
    ui::TableColumn::LEFT, 100));
  columns2.push_back(ui::TableColumn(6, L"os结束时间",
    ui::TableColumn::LEFT, 100));
  columns2.push_back(ui::TableColumn(7, L"执行交易耗时",
    ui::TableColumn::LEFT, 80));
  columns2.push_back(ui::TableColumn(8, L"品种",
    ui::TableColumn::LEFT, 100));
  columns2.push_back(ui::TableColumn(9, L"开/平",
    ui::TableColumn::LEFT, 50));
  columns2.push_back(ui::TableColumn(10, L"多/空",
    ui::TableColumn::LEFT, 50));
  columns2.push_back(ui::TableColumn(11, L"计划数量",
    ui::TableColumn::LEFT, 60));
  columns2.push_back(ui::TableColumn(12, L"实际成交数量",
    ui::TableColumn::LEFT, 90));
  columns2.push_back(ui::TableColumn(13, L"ms价格",
    ui::TableColumn::LEFT, 100));
  columns2.push_back(ui::TableColumn(14, L"成交均价",
    ui::TableColumn::LEFT, 100));
  columns2.push_back(ui::TableColumn(15, L"总金额",
    ui::TableColumn::LEFT, 100));
  columns2.push_back(ui::TableColumn(16, L"执行状态",
    ui::TableColumn::LEFT, 200));
  columns2.push_back(ui::TableColumn(17, L"总撤单次数",
    ui::TableColumn::LEFT, 100));
  //----------------------------------------------------------------------
  std::vector<ui::TableColumn> columns;
  /*columns.push_back(ui::TableColumn(0, L"时间",
    ui::TableColumn::LEFT, 150));

  columns.push_back(ui::TableColumn(1, L"品种",
    ui::TableColumn::LEFT, 100));
  columns.push_back(ui::TableColumn(2, L"多/空",
    ui::TableColumn::LEFT, 100));
  columns.push_back(ui::TableColumn(3, L"占用的保证金",
    ui::TableColumn::LEFT, 100));
  columns.push_back(ui::TableColumn(4, L"对应的点数",
    ui::TableColumn::LEFT, 100));
  columns.push_back(ui::TableColumn(5, L"止损的点数",
    ui::TableColumn::LEFT, 100));
  columns.push_back(ui::TableColumn(6, L"止盈的点数",
    ui::TableColumn::LEFT, 100));
  columns.push_back(ui::TableColumn(7, L"成本",
    ui::TableColumn::LEFT, 100));
  columns.push_back(ui::TableColumn(8, L"数量",
    ui::TableColumn::LEFT, 100));
  columns.push_back(ui::TableColumn(9, L"CloseProfit",
    ui::TableColumn::LEFT, 100));
  columns.push_back(ui::TableColumn(10, L"PositionProfit",
    ui::TableColumn::LEFT, 100));
  columns.push_back(ui::TableColumn(11, L"MarginRateByMoney",
    ui::TableColumn::LEFT, 100));
  columns.push_back(ui::TableColumn(12, L"Commission",
    ui::TableColumn::LEFT, 100));
  columns.push_back(ui::TableColumn(13, L"自算手续费",
    ui::TableColumn::LEFT, 100));
    */
  MakeupPositionColunms(columns, false);
  //==============================================
  table_order_ = new TableView(&model_order_, columns2, TEXT_ONLY, true, true, true);
  table_order_->SetObserver(&model_order_);

  table_position_ = new CCTableView(this, &model_position_, columns, TEXT_ONLY, true, true, true);
  table_position_->SetObserver(&model_position_);


  //=========================================
 

  ColumnSet* column_set = layout->AddColumnSet(0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL, 1,
                        GridLayout::USE_PREF, 0, 0);
  layout->StartRow(0.7f /* expand */, 0);
  layout->AddView(table_order_->CreateParentIfNecessary());


  column_set = layout->AddColumnSet(1);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL, 1,
    GridLayout::USE_PREF, 0, 0);
  layout->StartRow(0.3f, 1);
  layout->AddView(table_position_->CreateParentIfNecessary());
  //=========================================
  /*column_set = layout->AddColumnSet(1);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL, 1,
    GridLayout::USE_PREF, 0, 0);
  layout->StartRow(0.3f, 1);
  layout->AddView(table_position_->CreateParentIfNecessary());
  //=========================================


  column_set = layout->AddColumnSet(2);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
                        0.5f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
                        0.5f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
                        0.5f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
                        0.5f, GridLayout::USE_PREF, 0, 0);

  layout->StartRow(0 / * no expand * /, 2);


  layout->AddView(cash_label_);
  layout->AddView(balance_label_);
  layout->AddView(position_profit_label_);
  layout->AddView(clear_order_button_);*/

  //=============================================
  //manual_type_checkbox_ = new Combobox(&manual_type_checkbox_model_);
  //manual_direction_checkbox_ = new Combobox(&manual_direction_checkbox_model_);
  //manual_oc_checkbox_ = new Combobox(&manual_oc_checkbox_model_);
  //manual_close_day_checkbox_ = new Combobox(&manual_close_day_checkbox_model_);
  //manual_type_checkbox_->set_listener(this);
  //manual_direction_checkbox_->set_listener(this);
  //manual_close_day_checkbox_->set_listener(this);

  //manual_direction_text_ = new Textfield();
  //manual_oc_text_ = new Textfield();

  //manual_number_ = new Textfield();
  //manual_button_ = new BigTextButton(this, L"Manual Order");


  column_set = layout->AddColumnSet(3);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    0.20f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    0.20f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    0.20f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    0.20f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    0.20f, GridLayout::USE_PREF, 0, 0);
/*
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    0.20f, GridLayout::USE_PREF, 0, 0);*/

  layout->StartRow(0 /* no expand */, 3);


  //layout->AddView(manual_type_checkbox_);
  //layout->AddView(manual_oc_checkbox_);
  //layout->AddView(manual_direction_checkbox_);
  //layout->AddView(manual_close_day_checkbox_);
  //layout->AddView(manual_oc_text_);
  //layout->AddView(manual_direction_text_);
  //layout->AddView(manual_number_);
  //layout->AddView(manual_button_);


  //=====================================
  //layout->AddPaddingRow(0, 50);
/*
  column_set = layout->AddColumnSet(4);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);


  layout->StartRow(0 / * no expand * /, 4);


  layout->AddView(new Label(L""));*/
}

int OrderResultTable::RowCount() {
  /*int type_select_index = type_combobox_->selected_index();
  int level_select_index = strategy_combobox_->selected_index();

  if (type_select_index ==0 && level_select_index == 0) {
    return list_data_.size();
  } else if (type_select_index ==0) {
    std::string mql = strategy_combobox_model_.Get(level_select_index);
    return strategy_map_[mql].size();
  } else if (level_select_index ==0) {
    std::string type = type_combobox_model_.Get(type_select_index);
    return type_map_[type].size();
  } else {
    std::string mql = strategy_combobox_model_.Get(level_select_index);
    std::string type = type_combobox_model_.Get(type_select_index);
    return strategy_type_map_[std::pair<std::string, std::string>(mql, type)].size();
  }

  DCHECK(false);*/
  //return order_result_data_.size();
  return 0;
}

string16 OrderResultTable::GetText(int row, int column_id) {
  return string16();
  //return GetColumnText(column_id, *order_result_data_[order_result_keys_[row]].get());
}

//string16 OrderResultTable::GetColumnText(int id, order_execute::OrderUnit2UI const & info) {
//  switch (id) {
//      case 0: {
//        return ASCIIToUTF16(info.ms_id);
//        break;
//      }
//      case 1: {
//        return ASCIIToUTF16(info.os_id);
//        break;
//      }
//      case 2: {
//        return ASCIIToUTF16(info.ms_name);
//        break;
//      }
//      case 3: {
//        return ASCIIToUTF16(info.os_name);
//        break;
//      }
//      case 4: {
//        base::Time::Exploded e;
//        info.ms_time.LocalExplode(&e);
//        return base::StringPrintf(L"%d:%d:%d", e.hour, e.minute, e.second);
//        break;
//      }
//      case 5: {
//        if (info.order_status == order_execute::OverStatus) {
//          base::Time::Exploded e;
//          info.rp.start.LocalExplode(&e);
//          return base::StringPrintf(L"%d:%d:%d", e.hour, e.minute, e.second);
//        } else {
//          return L"N/A";
//        }
//        break;
//      }
//      case 6: {
//        if (info.order_status == order_execute::OverStatus) {
//          base::Time::Exploded e;
//          info.rp.over.LocalExplode(&e);
//          return base::StringPrintf(L"%d:%d:%d", e.hour, e.minute, e.second);
//        } else {
//          return L"N/A";
//        }
//        break;
//      }
//      case 7: {
//        if (info.order_status == order_execute::OverStatus) {
//          base::TimeDelta diff = info.rp.over - info.rp.start;
//          return base::StringPrintf(L"%d", (uint32)(diff.ToInternalValue()/base::Time::kMicrosecondsPerSecond));
//        } else {
//          return L"N/A";
//        }
//        break;
//      }
//      case 8: {
//        return ASCIIToUTF16(info.type);
//        break;
//      }
//      case 9: {
//        return info.op.IsOpen() ? L"开" : L"平";
//        break;
//      }
//      case 10: {
//        return info.op.IsBuy() ? L"多" : L"空";
//        break;
//      }
//      case 11: {
//        //if (info.fill_stage == order_execute::OverStatus) {
//        return ASCIIToUTF16(base::UintToString(info.vp.total()));
//        break;
//      }
//      case 12: {
//        return ASCIIToUTF16(base::UintToString(info.vp.total() - info.vp.rest()));
//        break;
//      }
//      case 13: {
//        //return ASCIIToUTF16(base::DoubleToString(info->ms->pri));
//        return ASCIIToUTF16(info.ms_price);
//        break;
//      }
//      case 14: {
//        return ASCIIToUTF16(info.average_price);
//        break;
//      }
//      case 15: {
//        return ASCIIToUTF16(info.total_cash);
//        break;
//      }
//      case 16: {
//        return base::SysNativeMBToWide(info.rp.last_error);
//        break;
//      }
//      case 17: {
//        return ASCIIToUTF16(base::UintToString(info.rp.withdraw_times));
//        break;
//      }
//      default: {
//        DCHECK(false);
//      }
//  }
// 
//
//  return string16();
//}


string16 OrderResultTable::GetColumnTextPosition(int id, phone_module::PositionData const & info) {
  switch (id) {
      case 0: {
        return ASCIIToUTF16(info.time_string);
        break;
      }
      case 1: {
        return ASCIIToUTF16(info.field.InstrumentID);
        break;
      }
      case 2: {
        return info.field.PosiDirection == '2' ? L"多" : L"空";
        break;
      }
      case 3: {
        return ASCIIToUTF16(base::DoubleToString(info.field.UseMargin));
        break;
      }
      case 4: {
        return ASCIIToUTF16(base::DoubleToString(info.price_index));
        break;
      }
      case 5: {
        return ASCIIToUTF16(base::DoubleToString(info.stop_price));
        break;
      }
      case 6: {
        return ASCIIToUTF16(base::DoubleToString(info.stop_win_price));
        break;
      }
      case 7: {
        return ASCIIToUTF16(base::DoubleToString(info.field.PositionCost));
        break;
      }
      case 8: {
        return ASCIIToUTF16(base::DoubleToString(info.field.Position));
        break;
      }
      case 9: {
        return ASCIIToUTF16(base::DoubleToString(info.field.TodayPosition));
        break;
      }
      case 10: {
        return ASCIIToUTF16(base::DoubleToString(info.field.Position - info.field.TodayPosition));
        break;
      }

      case 11: {
        return ASCIIToUTF16(base::DoubleToString(info.field.CloseProfit));
        break;
      }
      case 12: {
        return ASCIIToUTF16(base::DoubleToString(info.field.PositionProfit));
        break;
      }
      case 13: {
        return ASCIIToUTF16(base::DoubleToString(info.field.MarginRateByMoney));
        break;
      }
      case 14: {
        return ASCIIToUTF16(base::DoubleToString(info.field.Commission));
        break;
      }
      case 15: {
        return ASCIIToUTF16(base::DoubleToString(info.fee));
        break;
      }
      /*
      case 9: {
        return ASCIIToUTF16(base::DoubleToString(info.force_close_percent));
        break;
      }
      case 10: {
        return ASCIIToUTF16(base::DoubleToString(info.position_cash));
        break;
      }
      case 11: {
        return info.auto_close == true ? L"是" : L"否";
        break;
      }*/
      default: {
        DCHECK(false);
      }
  }
 

  return string16();
}

gfx::ImageSkia OrderResultTable::GetIcon(int row) {
  DCHECK(false);
  return gfx::ImageSkia();
  //return row % 2 ? gfx::ImageSkia(icon1_) : gfx::ImageSkia(icon2_);
}

void OrderResultTable::SetObserver(ui::TableModelObserver* observer) {}

void OrderResultTable::OnSelectionChanged() {
  /*if (table_->FirstSelectedRow() >= 0) {
    PrintStatus("Selected: %s",
                UTF16ToASCII(GetText(table_->FirstSelectedRow(), 0)).c_str());
  }*/
}

void OrderResultTable::OnDoubleClick() {
  /*if (table_->FirstSelectedRow() >= 0) {
    PrintStatus("Double Click: %s",
                UTF16ToASCII(GetText(table_->FirstSelectedRow(), 0)).c_str());
  }*/
}

void OrderResultTable::OnMiddleClick() {}

void OrderResultTable::OnKeyDown(ui::KeyboardCode virtual_keycode) {}

void OrderResultTable::OnTableViewDelete(TableView* table_view) {}

void OrderResultTable::OnTableView2Delete(TableView2* table_view) {}

void OrderResultTable::ButtonPressed(Button* sender, const ui::Event& event) {

}



void OrderResultTable::Observe(int type,
  const content::NotificationSource& source,
  const content::NotificationDetails& details) {
    if (type == phone_module::NOTIFICATION_CTP_MQ_MQLEVEL_INFO) {
      phone_module::LevelQuotationInfoBig const* info = content::Details<phone_module::LevelQuotationInfoBig const>(details).ptr();
      bool modified = false;
      std::string key = ui::MakeupKey(info->type, THOST_FTDC_PD_Long);
      if (position_data_.find(key) != position_data_.end()) {
        if (info->close > position_data_[key]->field.PositionCost / position_data_[key]->field.Position) {
          if (position_color_map_.find(key) == position_color_map_.end() ||
            position_color_map_[key] != SK_ColorRED) {
              position_color_map_[key] = SK_ColorRED;
              modified = true;
          }
        } else {
          if (position_color_map_.find(key) == position_color_map_.end() ||
            position_color_map_[key] != SK_ColorGREEN) {
              position_color_map_[key] = SK_ColorGREEN;
              modified = true;
          }
        }
      }

      key = ui::MakeupKey(info->type, THOST_FTDC_PD_Short);
      if (position_data_.find(key) != position_data_.end()) {
        if (info->close < position_data_[key]->field.PositionCost / position_data_[key]->field.Position) {
          if (position_color_map_.find(key) == position_color_map_.end() ||
            position_color_map_[key] != SK_ColorRED) {
              position_color_map_[key] = SK_ColorRED;
              modified = true;
          }
        } else {
          if (position_color_map_.find(key) == position_color_map_.end() ||
            position_color_map_[key] != SK_ColorGREEN) {
              position_color_map_[key] = SK_ColorGREEN;
              modified = true;
          }
        }
      }


      if (modified) {
        table_position_->OnModelChanged();
      }
    }
}

bool OrderResultTable::OnMessageReceived(IPC::Message const & msg) {


  CommonThread::ID thread;
  
  if (!CommonThread::GetCurrentThreadIdentifier(&thread) &&
    OverrideThreadForMessage(msg, &thread)) {

      CommonThread::PostTask(
        thread, FROM_HERE,
        base::Bind(base::IgnoreResult(&OrderResultTable::OnMessageReceived),
        base::Unretained(this), msg));

      return true;
  }


  if (msg.routing_id() == MSG_ROUTING_CONTROL) {
    // Dispatch control messages.
    bool msg_is_ok = false;
    

      if (msg_is_ok) {
        return true;
      }
  }

  // Dispatch incoming messages to the appropriate RenderView/WidgetHost.
  //RenderWidgetHost* rwh = render_widget_hosts_.Lookup(msg.routing_id());

  return false; 

}


//TaskManagerTableModel
void OrderResultTable::EnsureVisible() {
  //int type_select_index = 0;//type_combobox_->selected_index();
  //int level_select_index = 0;//strategy_combobox_->selected_index();

  //if (type_select_index ==0 && level_select_index == 0) {
  //  if (order_result_data_.size()) {
  //    //table_order_->Select(list_data_.size() - 1);
  //    //table_position_->Select(order_result_data_.size() - 1);
  //  }

  //  if (order_result_data_.size()) {
  //    //table_order_->Select(list_data_.size() - 1);
  //    table_order_->Select(order_result_data_.size() - 1);
  //  }
  //} 
}



//void OrderResultTable::OnOrderResult(PointerWrapper<order_execute::OrderUnit2UI> const & p) {
//  std::string key = p.get()->ms_id + "+" + p.get()->os_id;
//  order_result_data_[key] = std::shared_ptr<order_execute::OrderUnit2UI>(new order_execute::OrderUnit2UI(*p.get()));
//  auto it = order_result_keys_map_.find(key);
//  if (it == order_result_keys_map_.end()) {
//    order_result_keys_map_[key] = order_result_keys_.size();
//    order_result_keys_.push_back(key);
//    table_order_->OnItemsAdded(order_result_keys_.size() - 1, 1);
//  } else {
//    table_order_->OnItemsChanged(it->second, 1);
//  }
//
//  EnsureVisible();
//
//}

void OrderResultTable::OnInitParam(std::string const & s) {
/*
  StringTokenizer type(s, ",");
  while(type.GetNext()) {
    type_combobox_model_.Add(type.token());
    type_combobox_->ModelChanged();
  }*/
}


bool OrderResultTable::OverrideThreadForMessage(IPC::Message const& message,
  CommonThread::ID* thread) {
/*
    if (message.type() == CTP_CONSOLE_Command::ID || 
      message.type() == CTP_CONSOLE_CommandProtoBuf::ID) {
        *thread = CommonThread::UI;
        return true;
    }*/

    return false;
}

void OrderResultTable::OnPosition(PointerWrapper<phone_module::PositionData> const & p) {
  std::string key = ui::MakeupKey(p.get() ->field.InstrumentID, p.get() ->field.PosiDirection);
  uint32 const old_size = position_data_.size();
  position_data_[key] = std::shared_ptr<phone_module::PositionData>(new phone_module::PositionData(*p.get()));
  uint32 const new_size = position_data_.size();
  if (position_keys_.end() == std::find(position_keys_.begin(), position_keys_.end(), key)) {
    position_keys_.push_back(key);
  }
  //strategy_map_[p.get()->ms_name].push_back(list_data_[list_data_.size() - 1].get());
  //type_map_[p.get()->type].push_back(list_data_[list_data_.size() - 1].get());
  //strategy_type_map_[std::make_pair(p.get()->ms_name, p.get()->type)].push_back(list_data_[list_data_.size() - 1].get());

  if (old_size == new_size) {
    table_position_->OnItemsChanged(0, position_data_.size());
  } else {
    table_position_->OnModelChanged();
  }

  //if (list_data_.size() > 0) {
  //table_->ScrollRectToVisible(table_->GetRowBounds(list_data_.size() - 1));
  //table_->Select(list_data_.size() - 1);
  //}
  EnsureVisible();

}


void OrderResultTable::Selected() {
  //manual_type_checkbox_->ModelChanged();
  //manual_direction_checkbox_->ModelChanged();
 //manual_oc_checkbox_->ModelChanged();
}

bool OrderResultTable::GetCellColors(
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

int OrderResultTable::size() {
  return position_keys_.size();
}
string16 OrderResultTable::text(int row, int column_id) {
  int type_select_index = 0;//type_combobox_->selected_index();
  int level_select_index = 0;//strategy_combobox_->selected_index();

  DCHECK(position_data_.size() == position_keys_.size());
  if (type_select_index == 0 && level_select_index == 0) {
    if ((uint32)row < position_keys_.size()) {
      std::string key = position_keys_[row];
      return GetColumnTextPosition(column_id, * position_data_[key].get());
    } else {
      DCHECK(false);
    }
  } 



  return string16();
}

}  // namespace examples
}  // namespace views
