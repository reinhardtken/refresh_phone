// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "tu_table.h"

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
#include "../ui_common.h"

namespace {

  void OneRecord2OrderData(phone_module::OrderFootprints::OneRecord const *record, views::examples::OrderData & out) {
    out.operation = record->type;
    out.msos = record->msos;

    switch(record->type) {
      case phone_module::OrderFootprints::OneRecord::OrderRequest: {
          out.type = record->order_request.InstrumentID;
          out.price = record->order_request.LimitPrice;
          out.volumn = record->order_request.VolumeTotalOriginal;
          out.oc = record->order_request.CombOffsetFlag[0];
          out.bs = record->order_request.Direction;
          out.order_ref = record->order_request.OrderRef;
          out.utc = record->time;
          out.utc_string = record->time_string;
          break;
      }
      case phone_module::OrderFootprints::OneRecord::OrderResponse: {
        out.type = record->order_response.InstrumentID;
        out.price = record->order_response.LimitPrice;
        out.volumn = record->order_response.VolumeTotalOriginal;
        out.oc = record->order_response.CombOffsetFlag[0];
        out.bs = record->order_response.Direction;
        out.order_ref = record->order_response.OrderRef;
        out.utc = record->time;
        out.utc_string = record->time_string;
        out.status = record->order_response.OrderStatus;
        out.submit_status = record->order_response.OrderSubmitStatus;
        out.msg = record->order_response.StatusMsg;
        break;
      }
      case phone_module::OrderFootprints::OneRecord::OrderResponseError: {
        out.type = record->order_request.InstrumentID;
        out.price = record->order_request.LimitPrice;
        out.volumn = record->order_request.VolumeTotalOriginal;
        out.oc = record->order_request.CombOffsetFlag[0];
        out.bs = record->order_request.Direction;
        out.order_ref = record->order_request.OrderRef;
        out.utc = record->time;
        out.utc_string = record->time_string;

        out.status = record->common_filed.ErrorID;
        out.msg = record->common_filed.ErrorMsg;
        break;
      }
      case phone_module::OrderFootprints::OneRecord::ActionRequest: {
        out.type = record->action_request.InstrumentID;
        out.price = record->action_request.LimitPrice;
        out.order_ref = record->action_request.OrderRef;
        out.utc = record->time;
        out.utc_string = record->time_string;

        break;
      }
      case phone_module::OrderFootprints::OneRecord::ActionResponseError: {
        out.type = record->action_request.InstrumentID;
        out.price = record->action_request.LimitPrice;
        out.order_ref = record->action_request.OrderRef;
        out.utc = record->time;
        out.utc_string = record->time_string;

        out.status = record->common_filed.ErrorID;
        out.msg = record->common_filed.ErrorMsg;
        break;
      }
      case phone_module::OrderFootprints::OneRecord::TradeResponse: {
        out.type = record->trade_response.InstrumentID;
        out.price = record->trade_response.Price;
        out.volumn = record->trade_response.Volume;
        out.oc = record->trade_response.OffsetFlag;
        out.bs = record->trade_response.Direction;
        out.order_ref = record->trade_response.OrderRef;
        out.trade_time = record->trade_response.TradeTime;

        out.utc = record->time;
        out.utc_string = record->time_string;
        
        //status = real->record->trade_response.OrderStatus;
        //submit_status = real->record->trade_response.OrderSubmitStatus;
        //msg = real->record->trade_response.StatusMsg;
        break;
      }
      case phone_module::OrderFootprints::OneRecord::OrderSperator: {
        out.type = ui::kSperatorTable;
        base::Time::Exploded e;
        record->ms_arrive.LocalExplode(&e);
        out.ms_arrive = base::StringPrintf("%d:%d:%d", e.hour, e.minute, e.second);
        record->os_start.LocalExplode(&e);
        out.os_start = base::StringPrintf("%d:%d:%d", e.hour, e.minute, e.second);
        record->os_over.LocalExplode(&e);
        out.os_over = base::StringPrintf("%d:%d:%d", e.hour, e.minute, e.second);
        break;
      }
      default: {
        DCHECK(false);
        return;
      }
    }
  }
}


namespace views {
namespace examples {

  gfx::Size BigTextButton::GetPreferredSize() {
    gfx::Size size = TextButton::GetPreferredSize();
    size.set_height(size.height() * 2);
    return size;
  }
  //-====================================
  
  


  //==============================================================
  int OrderModel::RowCount() {
    return table_->order_data_.size();
    return 0;
  }

  string16 OrderModel::GetText(int row, int column_id) {
    int type_select_index = 0;//type_combobox_->selected_index();
    int level_select_index = 0;//strategy_combobox_->selected_index();


    if (type_select_index ==0 && level_select_index == 0) {
      if ((uint32)row < table_->order_data_.size()) {
        return table_->GetColumnTextOrder(column_id, *table_->order_data_[row].get());
      } else {
        DCHECK(false);
      }
    }
    return string16();
  }

  
  gfx::ImageSkia OrderModel::GetIcon(int row) {
    DCHECK(false);
    return gfx::ImageSkia();
  }

  //================================================================

TUTable::TUTable(CTPTabbedPane *p, std::string const &bc) 
  :CTPViewBase("Table"),
  pane_(p),
  bc_(bc),
  table_order_(NULL),
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


    
    
/*
    manual_type_checkbox_model_.Add("IF1310");
    manual_type_checkbox_model_.Add("ru1401");
    manual_type_checkbox_model_.Add("rb1401");
    manual_type_checkbox_model_.Add("FG401");
    manual_type_checkbox_model_.Add("m1401");*/


    manual_direction_checkbox_model_.Add("buy");
    manual_direction_checkbox_model_.Add("sell");
    manual_oc_checkbox_model_.Add("open");
    manual_oc_checkbox_model_.Add("close");
    manual_oc_checkbox_model_.Add("close_yesterday");


    if (pane_->HasID(bc_)) {
      ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::CTP,
        new CTP_TU_QryTypeList(pane_->GetID(bc_)));
    } else {
      DCHECK(false);
    }
    

    //manual_close_day_checkbox_model_.Add("today");
    //manual_close_day_checkbox_model_.Add("yesterday");
    //========================================================


}

TUTable::~TUTable() {
  // Delete the view before the model.
  delete table_order_;
  table_order_ = NULL;

}

void TUTable::OnSelectedIndexChanged(Combobox* combobox) {
  /*if (type_combobox_ == combobox) {
    table_->OnModelChanged();
    EnsureVisible();
  } else if (strategy_combobox_ == combobox) {
    table_->OnModelChanged();
    EnsureVisible();
  } else {
    DCHECK(false);
  }*/

}

void TUTable::CreateExampleView(View* container) {
#if defined(OS_WIN) && !defined(USE_AURA)
  column1_visible_checkbox_ = new Checkbox(
      ASCIIToUTF16("Fruit column visible"));
  column1_visible_checkbox_->SetChecked(true);
  column1_visible_checkbox_->set_listener(this);
  column2_visible_checkbox_ = new Checkbox(
      ASCIIToUTF16("Color column visible"));
  column2_visible_checkbox_->SetChecked(true);
  column2_visible_checkbox_->set_listener(this);
  column3_visible_checkbox_ = new Checkbox(
      ASCIIToUTF16("MS column visible"));
  column3_visible_checkbox_->SetChecked(true);
  column3_visible_checkbox_->set_listener(this);
  column4_visible_checkbox_ = new Checkbox(
      ASCIIToUTF16("MS column visible"));
  column4_visible_checkbox_->SetChecked(true);
  column4_visible_checkbox_->set_listener(this);
#endif


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
  columns2.push_back(ui::TableColumn(0, L"时间",
    ui::TableColumn::LEFT, 110));

  columns2.push_back(ui::TableColumn(1, L"品种",
    ui::TableColumn::LEFT, 70));

  columns2.push_back(ui::TableColumn(2, L"ms/os",
    ui::TableColumn::LEFT, 100));
  columns2.push_back(ui::TableColumn(3, L"oid",
    ui::TableColumn::LEFT, 10));

  columns2.push_back(ui::TableColumn(4, L"操作",
    ui::TableColumn::LEFT, 100));
  columns2.push_back(ui::TableColumn(5, L"开/平",
    ui::TableColumn::LEFT, 50));
  columns2.push_back(ui::TableColumn(6, L"多/空",
    ui::TableColumn::LEFT, 50));
  columns2.push_back(ui::TableColumn(7, L"价格",
    ui::TableColumn::LEFT, 100));
  columns2.push_back(ui::TableColumn(8, L"数量",
    ui::TableColumn::LEFT, 50));
  columns2.push_back(ui::TableColumn(9, L"status",
    ui::TableColumn::LEFT, 100));
  columns2.push_back(ui::TableColumn(10, L"submit_status",
    ui::TableColumn::LEFT, 100));
  columns2.push_back(ui::TableColumn(11, L"msg",
    ui::TableColumn::LEFT, 100));
  columns2.push_back(ui::TableColumn(12, L"成交时间",
    ui::TableColumn::LEFT, 100));

  columns2.push_back(ui::TableColumn(13, L"MS发生",
    ui::TableColumn::LEFT, 100));
  columns2.push_back(ui::TableColumn(14, L"交易开始",
    ui::TableColumn::LEFT, 100));
  columns2.push_back(ui::TableColumn(15, L"交易结束",
    ui::TableColumn::LEFT, 100));

  table_order_ = new TableView(&model_order_, columns2, TEXT_ONLY, true, true, true);
  table_order_->SetObserver(&model_order_);

  //============================================================
  
/*
  columns.push_back(ui::TableColumn(8, L"空限制",
    ui::TableColumn::LEFT, 100));
  columns.push_back(ui::TableColumn(9, L"亏限制",
    ui::TableColumn::LEFT, 100));
  columns.push_back(ui::TableColumn(10, L"每手资金",
    ui::TableColumn::LEFT, 100));
  columns.push_back(ui::TableColumn(11, L"自动平仓",
    ui::TableColumn::LEFT, 100));*/


  ColumnSet* column_set = layout->AddColumnSet(0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL, 1,
                        GridLayout::USE_PREF, 0, 0);
  layout->StartRow(0.7f /* expand */, 0);
  layout->AddView(table_order_->CreateParentIfNecessary());

  //=========================================
/*
  column_set = layout->AddColumnSet(1);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL, 1,
    GridLayout::USE_PREF, 0, 0);
  layout->StartRow(0.3f, 1);
  layout->AddView(table_position_->CreateParentIfNecessary());*/
  //=========================================


  column_set = layout->AddColumnSet(1);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
                        0.5f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
                        0.5f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
                        0.5f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
                        0.5f, GridLayout::USE_PREF, 0, 0);

  layout->StartRow(0 /* no expand */, 1);


  layout->AddView(cash_label_);
  layout->AddView(balance_label_);
  layout->AddView(position_profit_label_);
  layout->AddView(clear_order_button_);

  //=============================================
  manual_type_checkbox_ = new Combobox(&manual_type_checkbox_model_);
  manual_direction_checkbox_ = new Combobox(&manual_direction_checkbox_model_);
  manual_oc_checkbox_ = new Combobox(&manual_oc_checkbox_model_);
  //manual_close_day_checkbox_ = new Combobox(&manual_close_day_checkbox_model_);
  manual_type_checkbox_->set_listener(this);
  manual_direction_checkbox_->set_listener(this);
  //manual_close_day_checkbox_->set_listener(this);

  //manual_direction_text_ = new Textfield();
  //manual_oc_text_ = new Textfield();

  manual_number_ = new Textfield();
  manual_button_ = new BigTextButton(this, L"Manual Order");


  column_set = layout->AddColumnSet(2);
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

  layout->StartRow(0 /* no expand */, 2);


  layout->AddView(manual_type_checkbox_);
  layout->AddView(manual_oc_checkbox_);
  layout->AddView(manual_direction_checkbox_);
  //layout->AddView(manual_close_day_checkbox_);
  //layout->AddView(manual_oc_text_);
  //layout->AddView(manual_direction_text_);
  layout->AddView(manual_number_);
  layout->AddView(manual_button_);


  //=====================================
  
}

int TUTable::RowCount() {
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
  return 0;
}

string16 TUTable::GetText(int row, int column_id) {
  /*int type_select_index = type_combobox_->selected_index();
  int level_select_index = strategy_combobox_->selected_index();

  if (type_select_index ==0 && level_select_index == 0) {
    if ((uint32)row < list_data_.size()) {
      return GetColumnText(column_id, *list_data_[row].get());
    } else {
      DCHECK(false);
    }
  } else if (type_select_index ==0) {
    std::string mql = strategy_combobox_model_.Get(level_select_index);
    if ((uint32)row < strategy_map_[mql].size()) {
      return GetColumnText(column_id, *strategy_map_[mql][row]);
    } else {
      DCHECK(false);
    }
  } else if (level_select_index ==0) {
    std::string type = type_combobox_model_.Get(type_select_index);
    if ((uint32)row < type_map_[type].size()) {
      return GetColumnText(column_id, *type_map_[type][row]);
    } else {
      DCHECK(false);
    }
  } else {
    std::string mql = strategy_combobox_model_.Get(level_select_index);
    std::string type = type_combobox_model_.Get(type_select_index);
    std::pair<std::string, std::string> key(mql, type);

    if ((uint32)row < strategy_type_map_[key].size()) {
      return GetColumnText(column_id, *strategy_type_map_[key][row]);
    } else {
      DCHECK(false);
    }
  }*/

  DCHECK(false);

  return string16();
}

string16 TUTable::GetColumnText(int id, phone_module::PositionData const & info) {
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
        return ASCIIToUTF16(base::DoubleToString(info.field.PositionCost));
        break;
      }
      case 7: {
        return ASCIIToUTF16(base::DoubleToString(info.field.Position));
        break;
      }
      case 8: {
        return ASCIIToUTF16(base::DoubleToString(info.field.CloseProfit));
        break;
      }
      case 9: {
        return ASCIIToUTF16(base::DoubleToString(info.field.PositionProfit));
        break;
      }
      case 10: {
        return ASCIIToUTF16(base::DoubleToString(info.field.MarginRateByMoney));
        break;
      }
      /*case 8: {
        return ASCIIToUTF16(base::DoubleToString(info.sell_limit_price));
        break;
      }
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


string16 TUTable::GetColumnTextOrder(int id, OrderData const & info) {
  if (id == 2) {
    return ASCIIToUTF16(info.msos);
  }

  if (info.operation == phone_module::OrderFootprints::OneRecord::OrderSperator) {
    switch(id) {
      case 13: {
        return base::SysNativeMBToWide(info.ms_arrive);
        break;
      }
      case 14: {
        return base::SysNativeMBToWide(info.os_start);
        break;
      }
      case 15: {
        return base::SysNativeMBToWide(info.os_over);
        break;
      }
    }
    return string16();
  }
  
  switch (id) {
      case 0: {
        return ASCIIToUTF16(info.utc_string);
        break;
      }
      case 1: {
        return ASCIIToUTF16(info.type);
        break;
      }
      case 3: {
        return string16();
        break;
      }
      case 4: {
        return phone_module::Value2Text::Operation2Text(info.operation);
        break;
      }
      case 5: {
        return info.oc == THOST_FTDC_OF_Open ? L"开" : L"平";
        break;
      }
      case 6: {
        return info.bs == THOST_FTDC_D_Buy ? L"多" : L"空";
        break;
      }
      case 7: {
        return ASCIIToUTF16(base::DoubleToString(info.price));
        break;
      }
      case 8: {
        return ASCIIToUTF16(base::DoubleToString(info.volumn));
        break;
      }
      case 9: {
        return base::SysNativeMBToWide(phone_module::Value2Text::TThostFtdcOrderStatusType2Text(info.status));
        break;
      }
      case 10: {
        return base::SysNativeMBToWide(phone_module::Value2Text::TThostFtdcOrderSubmitStatusType2Text(info.submit_status));
        break;
      }
      case 11: {
        return base::SysNativeMBToWide(info.msg);
        break;
      }
      case 12: {
        return base::SysNativeMBToWide(info.trade_time);
        break;
      }
      case 13: {
        return string16();
        break;
      }
      case 14: {
        return string16();
        break;
      }
      case 15: {
        return string16();
        break;
      }
      /*case 10: {
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

gfx::ImageSkia TUTable::GetIcon(int row) {
  DCHECK(false);
  return gfx::ImageSkia();
  //return row % 2 ? gfx::ImageSkia(icon1_) : gfx::ImageSkia(icon2_);
}

void TUTable::SetObserver(ui::TableModelObserver* observer) {}

void TUTable::OnSelectionChanged() {
  /*if (table_->FirstSelectedRow() >= 0) {
    PrintStatus("Selected: %s",
                UTF16ToASCII(GetText(table_->FirstSelectedRow(), 0)).c_str());
  }*/
}

void TUTable::OnDoubleClick() {
  /*if (table_->FirstSelectedRow() >= 0) {
    PrintStatus("Double Click: %s",
                UTF16ToASCII(GetText(table_->FirstSelectedRow(), 0)).c_str());
  }*/
}

void TUTable::OnMiddleClick() {}

void TUTable::OnKeyDown(ui::KeyboardCode virtual_keycode) {}

void TUTable::OnTableViewDelete(TableView* table_view) {}

void TUTable::OnTableView2Delete(TableView2* table_view) {}

void TUTable::ButtonPressed(Button* sender, const ui::Event& event) {
  if (sender == clear_order_button_) {
    order_data_.clear();
    table_order_->OnModelChanged();
  } else if (sender == manual_button_) {
    std::string type = manual_type_checkbox_model_.Get(manual_type_checkbox_->selected_index());
    std::string oc_string = manual_oc_checkbox_model_.Get(manual_oc_checkbox_->selected_index());
    std::string direction = manual_direction_checkbox_model_.Get(manual_direction_checkbox_->selected_index());
    //std::string day = manual_close_day_checkbox_model_.Get(manual_close_day_checkbox_->selected_index());
    //std::wstring oc_string = manual_oc_text_->text();
    //std::wstring direction = manual_direction_text_->text();
    std::wstring number_string = manual_number_->text();
    manual_number_->SetText(L"");
    uint32 number;
    if (base::StringToInt(number_string, (int*)&number)) {
      char oc;
      if (oc_string == "open") {
        oc = phone_module::OpenOrder;
      } else if (oc_string == "close") {
        oc = phone_module::ManualTodayClose;
      } else if (oc_string == "close_yesterday") {
        oc = phone_module::ManualYestodayClose;
      } else {
          CHECK(false);
      }

      phone_module::BuySellEnum d;
      if (direction == "buy") {
        d = phone_module::BuyOrder;
      } else if (direction == "sell") {
        d = phone_module::SellOrder;
      } else {
        DCHECK(false);
      }

      if (pane_->HasID(bc_)) {
        ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::CTP,
          new CTP_TU_OpenCloseNew(pane_->GetID(bc_), type, oc, d, number));
      } else {
        DCHECK(false);
      }
      
    }
  }

  return;

#if defined(OS_WIN) && !defined(USE_AURA)
  int index = 0;
  bool show = true;
  if (sender == column1_visible_checkbox_) {
    index = 0;
    show = column1_visible_checkbox_->checked();
  } else if (sender == column2_visible_checkbox_) {
    index = 1;
    show = column2_visible_checkbox_->checked();
  } else if (sender == column3_visible_checkbox_) {
    index = 2;
    show = column3_visible_checkbox_->checked();
  } else if (sender == column4_visible_checkbox_) {
    index = 3;
    show = column4_visible_checkbox_->checked();
  }
  //table_->SetColumnVisibility(index, show);
#endif
}



void TUTable::Observe(int type,
  const content::NotificationSource& source,
  const content::NotificationDetails& details) {

}

bool TUTable::OnMessageReceived(IPC::Message const & msg) {


  CommonThread::ID thread;
  
  if (!CommonThread::GetCurrentThreadIdentifier(&thread) &&
    OverrideThreadForMessage(msg, &thread)) {

      CommonThread::PostTask(
        thread, FROM_HERE,
        base::Bind(base::IgnoreResult(&TUTable::OnMessageReceived),
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
void TUTable::EnsureVisible() {
  int type_select_index = 0;//type_combobox_->selected_index();
  int level_select_index = 0;//strategy_combobox_->selected_index();

  if (order_data_.size()) {
    //table_order_->Select(list_data_.size() - 1);
    table_order_->Select(order_data_.size() - 1);
  }

}

void TUTable::OnOrder(PointerWrapper<phone_module::OrderFootprints::OneRecord> const & p) {
  OrderData info;
  OneRecord2OrderData(p.get(), info);
  order_data_.push_back(std::shared_ptr<OrderData>(new OrderData(info)));

  table_order_->OnItemsAdded(order_data_.size() - 1, 1);
  EnsureVisible();
}

void TUTable::OnCash(PointerWrapper<CThostFtdcTradingAccountField> const & p) {
  std::wstring s = base::StringPrintf(L"Available: %lf", p.get()->Available);
  cash_label_->SetText(s);
  s = base::StringPrintf(L"Balance: %lf", p.get()->Balance);
  balance_label_->SetText(s);
  s = base::StringPrintf(L"PositionProfit: %lf", p.get()->PositionProfit);
  position_profit_label_->SetText(s);
}

void TUTable::OnTypeList(std::vector<std::string> const &vec) {
  for (auto it = vec.begin(); it != vec.end(); ++it) {
    manual_type_checkbox_model_.Add(*it);
  }
  manual_type_checkbox_->ModelChanged();
}


void TUTable::OnInitParam(std::string const & s) {
/*
  StringTokenizer type(s, ",");
  while(type.GetNext()) {
    type_combobox_model_.Add(type.token());
    type_combobox_->ModelChanged();
  }*/
}


bool TUTable::OverrideThreadForMessage(IPC::Message const& message,
  CommonThread::ID* thread) {
/*
    if (message.type() == CTP_CONSOLE_Command::ID || 
      message.type() == CTP_CONSOLE_CommandProtoBuf::ID) {
        *thread = CommonThread::UI;
        return true;
    }*/

    return false;
}


void TUTable::Selected() {
  manual_type_checkbox_->ModelChanged();
  manual_direction_checkbox_->ModelChanged();
  manual_oc_checkbox_->ModelChanged();
}

bool TUTable::GetCellColors(
  CCTableView* who,
  int model_row,
  int column,
  CCTableView::ItemColor* foreground,
  CCTableView::ItemColor* background,
  LOGFONT* logfont) {
    return false;
}

}  // namespace examples
}  // namespace views
