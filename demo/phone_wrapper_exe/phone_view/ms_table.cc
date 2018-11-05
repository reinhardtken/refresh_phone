// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ms_table.h"

#include <vector>

#include "base/string_util.h"
#include "base/string_tokenizer.h"
#include "base/string_number_conversions.h"
#include "third_party/skia/include/core/SkBitmap.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "ui/gfx/image/image_skia.h"
#include "ui/views/controls/button/checkbox.h"
#include "ui/views/layout/grid_layout.h"

#include "../../phone_common/common/common_common.h"
#include "ui_common.h"

namespace {
  std::string Enum2String(phone_module::MQLevel const l) {
    switch(l) {
      case phone_module::MQ1S: {
        return "MQ1S";
      }
      case phone_module::MQ5S: {
        return "MQ5S";
      }
      case phone_module::MQ1M: {
        return "MQ1M";
      }
      case phone_module::MQ5M: {
        return "MQ5M";
      }
      case phone_module::MQ15M: {
        return "MQ15M";
      }
      case phone_module::MQ1H: {
        return "MQ1H";
      }
      case phone_module::MQ1D: {
        return "MQ1D";
      }
      default: {
        //DCHECK(false);
        return "MQALL";
      }
    }
  }




  string16 GetColumnTextPosition(int id, phone_module::PositionData const & info) {
  switch (id) {
      case 0: {
        return ASCIIToUTF16(info.ba);
        break;
     }
      case 1: {
        return ASCIIToUTF16(info.time_string);
        break;
      }
      case 2: {
        return ASCIIToUTF16(info.field.InstrumentID);
        break;
      }
      case 3: {
        return info.field.PosiDirection == '2' ? L"多" : L"空";
        break;
      }
      case 4: {
        return ASCIIToUTF16(base::DoubleToString(info.field.UseMargin));
        break;
      }
      case 5: {
        return ASCIIToUTF16(base::DoubleToString(info.price_index));
        break;
      }
      case 6: {
        return ASCIIToUTF16(base::DoubleToString(info.stop_price));
        break;
      }
      case 7: {
        return ASCIIToUTF16(base::DoubleToString(info.stop_win_price));
        break;
      }
      case 8: {
        return ASCIIToUTF16(base::DoubleToString(info.field.PositionCost));
        break;
      }
      case 9: {
        return ASCIIToUTF16(base::DoubleToString(info.field.Position));
        break;
      }
      case 10: {
        return ASCIIToUTF16(base::DoubleToString(info.field.TodayPosition));
        break;
      }
      case 11: {
        return ASCIIToUTF16(base::DoubleToString(info.field.Position - info.field.TodayPosition));
        break;
      }

      case 12: {
        return ASCIIToUTF16(base::DoubleToString(info.field.CloseProfit));
        break;
      }
      case 13: {
        return ASCIIToUTF16(base::DoubleToString(info.field.PositionProfit));
        break;
      }
      case 14: {
        return ASCIIToUTF16(base::DoubleToString(info.field.MarginRateByMoney));
        break;
      }
      case 15: {
        return ASCIIToUTF16(base::DoubleToString(info.field.Commission));
        break;
      }
      case 16: {
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
}


namespace views {
namespace examples {
  


  //================================================================

MSTable::MSTable() 
  :CTPViewBase("Table") ,
  table_(NULL),
  table_position_(NULL),
  ALLOW_THIS_IN_INITIALIZER_LIST(model_position_(this)),
  ThreadMessageFilter(true)/*,
这段代码高度危险，不推荐使用=
  strategy_combobox_(new Combobox(&strategy_combobox_model_)),
  type_combobox_(new Combobox(&type_combobox_model_))*/ {

  strategy_combobox_ = (new Combobox(&strategy_combobox_model_));
  type_combobox_ = (new Combobox(&type_combobox_model_));


  strategy_combobox_model_.Add("MS.ALL");//必须是第一个


  type_combobox_model_.Add("TYPE.ALL");//必须是第一个

  //===========================================
  strategy_combobox_->set_listener(this);
  type_combobox_->set_listener(this);

}

MSTable::~MSTable() {
  // Delete the view before the model.
  delete table_;
  table_ = NULL;
}

void MSTable::OnSelectedIndexChanged(Combobox* combobox) {
/*
  if (type_combobox_ == combobox) {
    table_->OnModelChanged();
    EnsureVisible();
  } else if (strategy_combobox_ == combobox) {
    table_->OnModelChanged();
    EnsureVisible();
  } else {
    DCHECK(false);
  }*/

}

void MSTable::CreateExampleView(View* container) {
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

  GridLayout* layout = new GridLayout(container);
  container->SetLayoutManager(layout);

  std::vector<ui::TableColumn> columns;
  columns.push_back(ui::TableColumn(0, L"时间",
                                    ui::TableColumn::LEFT, 150));

  columns.push_back(ui::TableColumn(1, L"MS名称",
                                    ui::TableColumn::LEFT, 100));
  columns.push_back(ui::TableColumn(2, L"OS名称",
    ui::TableColumn::LEFT, 100));
  columns.push_back(ui::TableColumn(3, L"品种",
                                    ui::TableColumn::LEFT, 100));
  columns.push_back(ui::TableColumn(4, L"级别",
                                    ui::TableColumn::LEFT, 100));
  columns.push_back(ui::TableColumn(5, L"开/平",
    ui::TableColumn::LEFT, 100));
  columns.push_back(ui::TableColumn(6, L"多/空",
    ui::TableColumn::LEFT, 100));
  columns.push_back(ui::TableColumn(7, L"价格",
    ui::TableColumn::LEFT, 100));
  columns.push_back(ui::TableColumn(8, L"定价/限价",
    ui::TableColumn::LEFT, 100));
  columns.push_back(ui::TableColumn(9, L"每手资金",
    ui::TableColumn::LEFT, 100));
/*
  columns.push_back(ui::TableColumn(9, L"亏限制",
    ui::TableColumn::LEFT, 100));
  columns.push_back(ui::TableColumn(10, L"每手资金",
    ui::TableColumn::LEFT, 100));
  columns.push_back(ui::TableColumn(11, L"自动平仓",
    ui::TableColumn::LEFT, 100));*/

  table_ = new TableView(this, columns, TEXT_ONLY, true, true, true);
  table_->SetObserver(this);
  //icon1_.setConfig(SkBitmap::kARGB_8888_Config, 16, 16);
  //icon1_.allocPixels();
  //SkCanvas canvas1(icon1_);
  //canvas1.drawColor(SK_ColorRED);

  //icon2_.setConfig(SkBitmap::kARGB_8888_Config, 16, 16);
  //icon2_.allocPixels();
  //SkCanvas canvas2(icon2_);
  //canvas2.drawColor(SK_ColorBLUE);



  //持仓
  std::vector<ui::TableColumn> columns_pos;
  MakeupPositionColunms(columns_pos, true);
  //==============================================
  table_position_ = new CCTableView(this, &model_position_, columns_pos, TEXT_ONLY, true, true, true);
  table_position_->SetObserver(&model_position_);
  //持仓=====


  ColumnSet* column_set = layout->AddColumnSet(0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL, 1,
                        GridLayout::USE_PREF, 0, 0);
  layout->StartRow(3 /* expand */, 0);
  layout->AddView(table_->CreateParentIfNecessary());

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


  layout->AddView(strategy_combobox_);
  layout->AddView(type_combobox_);
  layout->AddView(column3_visible_checkbox_);
  noused_button_ = new BigTextButton(NULL, L"No Used");
  layout->AddView(noused_button_);
  //layout->AddView(column4_visible_checkbox_);
  //=============================================
  column_set = layout->AddColumnSet(2);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1, GridLayout::USE_PREF, 0, 0);

  layout->StartRow(7 /* no expand */, 2);
  layout->AddView(table_position_);
  

}

int MSTable::RowCount() {
  int type_select_index = type_combobox_->selected_index();
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

  DCHECK(false);
  return 0;
}

string16 MSTable::GetText(int row, int column_id) {
  int type_select_index = type_combobox_->selected_index();
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
  }

  

  return string16();
}

string16 MSTable::GetColumnText(int id, phone_module::MarketStrategyOrder const & info) {
  switch (id) {
      case 0: {
        return ASCIIToUTF16(info.time_string);
        break;
      }
      case 1: {
        return ASCIIToUTF16(info.ms_name);
        break;
      }
      case 2: {
        return ASCIIToUTF16(info.os_name);
        break;
      }
      case 3: {
        return ASCIIToUTF16(info.type);
        break;
      }
      case 4: {
        return ASCIIToUTF16(::Enum2String(info.level));
        break;
      }
      case 5: {
        if (StartsWithASCII(info.ms_name, market_strategy_name::kSuccessive, true)) {
          if (info.buy_clean_sell != 0) {
            return L"开";
          } else {
            return L"平";
          }
        }
        break;
      }
      case 6: {
        if (StartsWithASCII(info.ms_name, market_strategy_name::kSuccessive, true)) {
          if (info.buy_clean_sell == 1) {
            return L"多";
          } else if (info.buy_clean_sell == -1) {
            return L"空";
          }
        }
        break;
      }
      case 7: {
        return ASCIIToUTF16(base::DoubleToString(info.price));
        break;
      }
      case 8: {
        if (info.os_name == "os.fixprice") {
          return L"定价";
        } if (info.os_name == "os.pursueprice") {
          return ASCIIToUTF16(base::DoubleToString(info.limit_percent));
        }
        break;
      }
      case 9: {
        return ASCIIToUTF16(base::DoubleToString(info.position_cash));
        break;
      }
/*
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

gfx::ImageSkia MSTable::GetIcon(int row) {
  DCHECK(false);
  return gfx::ImageSkia();
  //return row % 2 ? gfx::ImageSkia(icon1_) : gfx::ImageSkia(icon2_);
}

void MSTable::SetObserver(ui::TableModelObserver* observer) {}

void MSTable::OnSelectionChanged() {
  if (table_->FirstSelectedRow() >= 0) {
    PrintStatus("Selected: %s",
                UTF16ToASCII(GetText(table_->FirstSelectedRow(), 0)).c_str());
  }
}

void MSTable::OnDoubleClick() {
  if (table_->FirstSelectedRow() >= 0) {
    PrintStatus("Double Click: %s",
                UTF16ToASCII(GetText(table_->FirstSelectedRow(), 0)).c_str());
  }
}

void MSTable::OnMiddleClick() {}

void MSTable::OnKeyDown(ui::KeyboardCode virtual_keycode) {}

void MSTable::OnTableViewDelete(TableView* table_view) {}

void MSTable::OnTableView2Delete(TableView2* table_view) {}

void MSTable::ButtonPressed(Button* sender, const ui::Event& event) {
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



void MSTable::Observe(int type,
  const content::NotificationSource& source,
  const content::NotificationDetails& details) {
}

bool MSTable::OnMessageReceived(IPC::Message const & msg) {


  CommonThread::ID thread;
  
  if (!CommonThread::GetCurrentThreadIdentifier(&thread) &&
    OverrideThreadForMessage(msg, &thread)) {

      CommonThread::PostTask(
        thread, FROM_HERE,
        base::Bind(base::IgnoreResult(&MSTable::OnMessageReceived),
        base::Unretained(this), msg));

      return true;
  }


  if (msg.routing_id() == MSG_ROUTING_CONTROL) {
    // Dispatch control messages.
    bool msg_is_ok = false;
    IPC_BEGIN_MESSAGE_MAP_EX(MSTable, msg, msg_is_ok)
      IPC_MESSAGE_HANDLER(CTP_MS2UI, OnMessageBroadcast)
      //IPC_MESSAGE_HANDLER(CTP_MQLevelParam, OnInitParam)
      //IPC_MESSAGE_HANDLER(CTP_CONSOLE_CommandProtoBuf, OnConsoleCommandProtoBuf)
      //IPC_MESSAGE_HANDLER(CTP_TradeUnitCreated, OnTradeUnitCreated)
      //IPC_MESSAGE_HANDLER(CTP_ClearList, OnClearList)


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
void MSTable::EnsureVisible() {
  int type_select_index = type_combobox_->selected_index();
  int level_select_index = strategy_combobox_->selected_index();

  if (type_select_index ==0 && level_select_index == 0) {
    if (list_data_.size()) {
      table_->Select(list_data_.size() - 1);
    }
  } else if (type_select_index ==0) {
    std::string mql = strategy_combobox_model_.Get(level_select_index);
    if (strategy_map_[mql].size()) {
      table_->Select(strategy_map_[mql].size() - 1);
    }
  } else if (level_select_index ==0) {
    std::string type = type_combobox_model_.Get(type_select_index);
    if (type_map_[type].size()) {
      table_->Select(type_map_[type].size() - 1);
    }
  } else {
    std::string mql = strategy_combobox_model_.Get(level_select_index);
    std::string type = type_combobox_model_.Get(type_select_index);
    std::pair<std::string, std::string> key(mql, type);
    if (strategy_type_map_[key].size()) {
      table_->Select(strategy_type_map_[key].size() - 1);
    }
  }
}

void MSTable::OnMessageBroadcast(PointerWrapper<phone_module::MarketStrategyOrder> const & p) {
  list_data_.push_back(std::shared_ptr<phone_module::MarketStrategyOrder>(new phone_module::MarketStrategyOrder(*p.get())));
  strategy_map_[p.get()->ms_name].push_back(list_data_[list_data_.size() - 1].get());
  type_map_[p.get()->type].push_back(list_data_[list_data_.size() - 1].get());
  strategy_type_map_[std::make_pair(p.get()->ms_name, p.get()->type)].push_back(list_data_[list_data_.size() - 1].get());


  table_->OnItemsAdded(list_data_.size() - 1, 1);
  //if (list_data_.size() > 0) {
    //table_->ScrollRectToVisible(table_->GetRowBounds(list_data_.size() - 1));
    //table_->Select(list_data_.size() - 1);
  //}
  EnsureVisible();

}

void MSTable::OnInitParam(std::string const & s) {
  StringTokenizer type(s, ",");
  while(type.GetNext()) {
    type_combobox_model_.Add(type.token());
    type_combobox_->ModelChanged();
  }
}


bool MSTable::OverrideThreadForMessage(IPC::Message const& message,
  CommonThread::ID* thread) {
    if (message.type() == CTP_CONSOLE_Command::ID || 
      message.type() == CTP_CONSOLE_CommandProtoBuf::ID) {
        *thread = CommonThread::UI;
        return true;
    }

    return false;
}


void MSTable::Selected() {
  type_combobox_->ModelChanged();
  strategy_combobox_->ModelChanged();
}






int MSTable::size() {
  return position_keys_.size();
}
string16 MSTable::text(int row, int column_id) {
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


bool MSTable::GetCellColors(
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


void MSTable::OnPosition(std::string const & ba, PointerWrapper<phone_module::PositionData> const & p) {
  //  [7/18/2016 liuqingping]
  //持仓数量不为0或者平仓利润绝对值大于50
  if (p.get()->field.Position > 0 || std::fabs(p.get()->field.CloseProfit) > 50) {
    std::string key = ui::MakeupKey(p.get() ->field.InstrumentID, p.get() ->field.PosiDirection);
    key = ba + " " + key;

    uint32 const old_size = position_data_.size();
    position_data_[key] = std::shared_ptr<phone_module::PositionData>(new phone_module::PositionData(*p.get()));
    uint32 const new_size = position_data_.size();
    if (position_keys_.end() == std::find(position_keys_.begin(), position_keys_.end(), key)) {
      position_keys_.push_back(key);
    }

    if (old_size == new_size) {
      table_position_->OnItemsChanged(0, position_data_.size());
    } else {
      table_position_->OnModelChanged();
    }

    EnsureVisible();
  }
}

}  // namespace examples
}  // namespace views
