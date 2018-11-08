// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "mq_table.h"

#include <vector>

#include "base/string_util.h"
#include "base/string_tokenizer.h"
#include "base/string_number_conversions.h"
#include "third_party/skia/include/core/SkBitmap.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "ui/gfx/image/image_skia.h"
#include "ui/views/controls/button/checkbox.h"
#include "ui/views/layout/grid_layout.h"
#include "content/browser/notification_service_impl.h"


#include "../../phone_common/include/ctp_notification_types.h"


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
}


namespace views {
namespace examples {
  string16 LevelComboboxModel::GetItemAt(int index) {
    return ASCIIToUTF16(::Enum2String(data_[index]));
  }


  //================================================================

MQTable::MQTable() 
  :CTPViewBase("Table") ,
  table_(NULL),
  ThreadMessageFilter(true)/*,
这段代码高度危险，不推荐使用=
  level_combobox_(new Combobox(&level_combobox_model_)),
  type_combobox_(new Combobox(&type_combobox_model_))*/ {

  level_combobox_ = (new Combobox(&level_combobox_model_));
  type_combobox_ = (new Combobox(&type_combobox_model_));


  level_combobox_model_.Add(phone_module::MQ_ALL);//必须是第一个
  level_combobox_model_.Add(phone_module::MQ1S);
  level_combobox_model_.Add(phone_module::MQ5S);
  level_combobox_model_.Add(phone_module::MQ1M);
  level_combobox_model_.Add(phone_module::MQ5M);
  level_combobox_model_.Add(phone_module::MQ15M);
  level_combobox_model_.Add(phone_module::MQ1H);
  level_combobox_model_.Add(phone_module::MQ1D);

  type_combobox_model_.Add("TYPE_ALL");//必须是第一个

  //===========================================
  level_combobox_->set_listener(this);
  type_combobox_->set_listener(this);

}

MQTable::~MQTable() {
  // Delete the view before the model.
  delete table_;
  table_ = NULL;
}

void MQTable::OnSelectedIndexChanged(Combobox* combobox) {
/*
  if (type_combobox_ == combobox) {
    table_->OnModelChanged();
    EnsureVisible();
  } else if (level_combobox_ == combobox) {
    table_->OnModelChanged();
    EnsureVisible();
  } else {
    DCHECK(false);
  }*/

}

void MQTable::CreateExampleView(View* container) {
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
      ASCIIToUTF16("MQ olumn visible"));
  column3_visible_checkbox_->SetChecked(true);
  column3_visible_checkbox_->set_listener(this);
  column4_visible_checkbox_ = new Checkbox(
      ASCIIToUTF16("MQ column visible"));
  column4_visible_checkbox_->SetChecked(true);
  column4_visible_checkbox_->set_listener(this);
#endif

  GridLayout* layout = new GridLayout(container);
  container->SetLayoutManager(layout);

  std::vector<ui::TableColumn> columns;
  columns.push_back(ui::TableColumn(0, L"时间",
                                    ui::TableColumn::LEFT, 150));

  columns.push_back(ui::TableColumn(1, L"品种",
                                    ui::TableColumn::LEFT, 100));
  columns.push_back(ui::TableColumn(2, L"周期",
                                    ui::TableColumn::LEFT, 100));
  columns.push_back(ui::TableColumn(3, L"开",
                                    ui::TableColumn::LEFT, 100));
  columns.push_back(ui::TableColumn(4, L"高",
    ui::TableColumn::LEFT, 100));
  columns.push_back(ui::TableColumn(5, L"低",
    ui::TableColumn::LEFT, 100));
  columns.push_back(ui::TableColumn(6, L"收",
    ui::TableColumn::LEFT, 100));
  columns.push_back(ui::TableColumn(7, L"量",
    ui::TableColumn::LEFT, 100));
  columns.push_back(ui::TableColumn(8, L"样本数",
    ui::TableColumn::LEFT, 100));

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
  uint32 index = 0;
  ColumnSet* column_set = layout->AddColumnSet(index);

  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    0.5f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    0.5f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    0.5f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    0.5f, GridLayout::USE_PREF, 0, 0);

  layout->StartRow(0 /* expand */, index);

  price_open_time_ = new Label(L"开:");
  price_open_time_->SetHorizontalAlignment(Label::ALIGN_LEFT);
  price_high_time_ = new Label(L"高:");
  price_high_time_->SetHorizontalAlignment(Label::ALIGN_LEFT);
  price_low_time_ = new Label(L"低:");
  price_low_time_->SetHorizontalAlignment(Label::ALIGN_LEFT);
  price_close_time_ = new Label(L"收:");
  price_close_time_->SetHorizontalAlignment(Label::ALIGN_LEFT);
  
  layout->AddView(price_open_time_);
  layout->AddView(price_high_time_);
  layout->AddView(price_low_time_);
  layout->AddView(price_close_time_);

  //===========================================
  index++;
  column_set = layout->AddColumnSet(index);

  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    0.5f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    0.5f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    0.5f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    0.5f, GridLayout::USE_PREF, 0, 0);

  layout->StartRow(0 /* expand */, index);

  price_open_ = new Label(L"");
  price_open_->SetHorizontalAlignment(Label::ALIGN_LEFT);
  price_high_ = new Label(L"");
  price_high_->SetHorizontalAlignment(Label::ALIGN_LEFT);
  price_low_ = new Label(L"");
  price_low_->SetHorizontalAlignment(Label::ALIGN_LEFT);
  price_close_ = new Label(L"");
  price_close_->SetHorizontalAlignment(Label::ALIGN_LEFT);

  layout->AddView(price_open_);
  layout->AddView(price_high_);
  layout->AddView(price_low_);
  layout->AddView(price_close_);
  //==============================================
  index++;
  column_set = layout->AddColumnSet(index);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL, 1,
    GridLayout::USE_PREF, 0, 0);
  layout->StartRow(1 /* expand */, index);
  layout->AddView(table_->CreateParentIfNecessary());

  //===========================================
  index++;
  column_set = layout->AddColumnSet(index);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
                        0.5f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
                        0.5f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
                        0.5f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
                        0.5f, GridLayout::USE_PREF, 0, 0);

  layout->StartRow(0 /* no expand */, index);


  layout->AddView(level_combobox_);
  layout->AddView(type_combobox_);
  layout->AddView(column3_visible_checkbox_);
  noused_button_ = new BigTextButton(NULL, L"No Used");
  layout->AddView(noused_button_);
/*
  //=============================================
  connect_bar_ = new ProgressBar();
  //connect_bar_->SetValue(0.5);

  column_set = layout->AddColumnSet(2);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  layout->StartRow(0 / * no expand * /, 2);
  layout->AddView(connect_bar_);

  //==================================================
  connect_button_ = new TextButton(this, L"登录行情服务器");
  connect_button_->set_alignment(TextButton::ALIGN_CENTER);
  column_set = layout->AddColumnSet(3);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  layout->StartRow(0 / * no expand * /, 3);
  layout->AddView(connect_button_);*/

}

int MQTable::RowCount() {
  int type_select_index = type_combobox_->selected_index();
  int level_select_index = level_combobox_->selected_index();

  if (type_select_index ==0 && level_select_index == 0) {
    return list_data_.size();
  } else if (type_select_index ==0) {
    phone_module::MQLevel mql = level_combobox_model_.Get(level_select_index);
    return level_map_[mql].size();
  } else if (level_select_index ==0) {
    std::string type = type_combobox_model_.Get(type_select_index);
    return type_map_[type].size();
  } else {
    phone_module::MQLevel mql = level_combobox_model_.Get(level_select_index);
    std::string type = type_combobox_model_.Get(type_select_index);
    return level_type_map_[std::pair<phone_module::MQLevel, std::string>(mql, type)].size();
  }

  DCHECK(false);
  return 0;
}

string16 MQTable::GetText(int row, int column_id) {
  int type_select_index = type_combobox_->selected_index();
  int level_select_index = level_combobox_->selected_index();

  if (type_select_index ==0 && level_select_index == 0) {
    if ((uint32)row < list_data_.size()) {
      return GetColumnText(column_id, *list_data_[row].get());
    } else {
      DCHECK(false);
    }
  } else if (type_select_index ==0) {
    phone_module::MQLevel mql = level_combobox_model_.Get(level_select_index);
    if ((uint32)row < level_map_[mql].size()) {
      return GetColumnText(column_id, *level_map_[mql][row]);
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
    phone_module::MQLevel mql = level_combobox_model_.Get(level_select_index);
    std::string type = type_combobox_model_.Get(type_select_index);
    std::pair<phone_module::MQLevel, std::string> key(mql, type);

    if ((uint32)row < level_type_map_[key].size()) {
      return GetColumnText(column_id, *level_type_map_[key][row]);
    } else {
      DCHECK(false);
    }
  }

  

  return string16();
}

string16 MQTable::GetColumnText(int id, phone_module::LevelQuotationInfoBig const & info) {
  switch (id) {
      case 0: {
        return ASCIIToUTF16(info.time_string);
        break;
      }
      case 1: {
        return ASCIIToUTF16(info.type);
        break;
      }
      case 2: {
        return ASCIIToUTF16(::Enum2String(info.level));
        break;
      }
      case 3: {
        return ASCIIToUTF16(base::DoubleToString(info.open));
        break;
      }
      case 4: {
        return ASCIIToUTF16(base::DoubleToString(info.high));
        break;
      }
      case 5: {
        return ASCIIToUTF16(base::DoubleToString(info.low));
        break;
      }
      case 6: {
        return ASCIIToUTF16(base::DoubleToString(info.close));
        break;
      }
      case 7: {
        return ASCIIToUTF16(base::UintToString(info.volumn));
        break;
      }
      case 8: {
        return ASCIIToUTF16(base::UintToString(info.number));
        break;
      }
      default: {
        DCHECK(false);
      }
  }
 

  return string16();
}

gfx::ImageSkia MQTable::GetIcon(int row) {
  DCHECK(false);
  return gfx::ImageSkia();
  //return row % 2 ? gfx::ImageSkia(icon1_) : gfx::ImageSkia(icon2_);
}

void MQTable::SetObserver(ui::TableModelObserver* observer) {}

void MQTable::OnSelectionChanged() {
  //if (table_->FirstSelectedRow() >= 0) {
  //  PrintStatus("Selected: %s",
  //              UTF16ToASCII(GetText(table_->FirstSelectedRow(), 0)).c_str());
  //}
}

void MQTable::OnDoubleClick() {

}

void MQTable::OnMiddleClick() {}

void MQTable::OnKeyDown(ui::KeyboardCode virtual_keycode) {}

void MQTable::OnTableViewDelete(TableView* table_view) {}

void MQTable::OnTableView2Delete(TableView2* table_view) {}

void MQTable::ButtonPressed(Button* sender, const ui::Event& event) {
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
  } /*else if (sender == connect_button_) {
    ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::CTP, new CTP_MQLogin(0));
  }*/
  //table_->SetColumnVisibility(index, show);
#endif
}



void MQTable::Observe(int type,
  const content::NotificationSource& source,
  const content::NotificationDetails& details) {
}

bool MQTable::OnMessageReceived(IPC::Message const & msg) {


  CommonThread::ID thread;
  
  if (!CommonThread::GetCurrentThreadIdentifier(&thread) &&
    OverrideThreadForMessage(msg, &thread)) {

      CommonThread::PostTask(
        thread, FROM_HERE,
        base::Bind(base::IgnoreResult(&MQTable::OnMessageReceived),
        base::Unretained(this), msg));

      return true;
  }


  if (msg.routing_id() == MSG_ROUTING_CONTROL) {
    // Dispatch control messages.
    bool msg_is_ok = false;
    //IPC_BEGIN_MESSAGE_MAP_EX(MQTable, msg, msg_is_ok)
      //IPC_MESSAGE_HANDLER(CTP_MQLevel2UI, OnMQ)
      //IPC_MESSAGE_HANDLER(CTP_MQLevelParam, OnInitParam)
      //IPC_MESSAGE_HANDLER(CTP_MQ_Login_Success_2UI, OnLoginSuccess)
      //IPC_MESSAGE_HANDLER(CTP_MQ_Logout_2UI, OnDisConnect)
      
      //IPC_MESSAGE_HANDLER(CTP_CONSOLE_CommandProtoBuf, OnConsoleCommandProtoBuf)
      //IPC_MESSAGE_HANDLER(CTP_TradeUnitCreated, OnTradeUnitCreated)
      //IPC_MESSAGE_HANDLER(CTP_ClearList, OnClearList)


      //IPC_MESSAGE_UNHANDLED_ERROR()
      //IPC_END_MESSAGE_MAP_EX()

      if (msg_is_ok) {
        return true;
      }
  }

  // Dispatch incoming messages to the appropriate RenderView/WidgetHost.
  //RenderWidgetHost* rwh = render_widget_hosts_.Lookup(msg.routing_id());

  return false; 

}


void MQTable::OnLoginSuccess() {
  //connect_bar_->SetValue(1);
}


void MQTable::OnDisConnect() {
  //connect_bar_->SetValue(0);
}


//TaskManagerTableModel
void MQTable::EnsureVisible() {
  int type_select_index = type_combobox_->selected_index();
  int level_select_index = level_combobox_->selected_index();

  if (type_select_index ==0 && level_select_index == 0) {
    if (list_data_.size()) {
      table_->Select(list_data_.size() - 1);
    }
  } else if (type_select_index ==0) {
    phone_module::MQLevel mql = level_combobox_model_.Get(level_select_index);
    if (level_map_[mql].size()) {
      table_->Select(level_map_[mql].size() - 1);
    }
  } else if (level_select_index ==0) {
    std::string type = type_combobox_model_.Get(type_select_index);
    if (type_map_[type].size()) {
      table_->Select(type_map_[type].size() - 1);
    }
  } else {
    phone_module::MQLevel mql = level_combobox_model_.Get(level_select_index);
    std::string type = type_combobox_model_.Get(type_select_index);
    std::pair<phone_module::MQLevel, std::string> key(mql, type);
    if (level_type_map_[key].size()) {
      table_->Select(level_type_map_[key].size() - 1);
    }
  }
}

// [10/30/2013 ken] 
/*
view的实现是在不可见的时候销毁native window，这导致了show的时候创建，如果行情到达的时候，正好处于销毁没创建的阶段，就出现了今天遇到的问题==
对我的需求来说，这种不可见销毁并不符合我程序对性能的需求，我的程序对内存目前来说是不做任何限制的==
但是反过来说，如果行情table不可见，但是每隔5s就若干品种数据add进去，也确实十分浪费==
void NativeControlWin::VisibilityChanged(View* starting_from, bool is_visible) {
// We might get called due to visibility changes at any point in the
// hierarchy, lets check whether we are really visible or not.
bool is_drawn = IsDrawn();
if (!is_drawn && native_view()) {
// We destroy the child control HWND when we become invisible because of the
// performance cost of maintaining many HWNDs.
HWND hwnd = native_view();
Detach();
DestroyWindow(hwnd);
} else if (is_drawn && !native_view()) {
if (GetWidget())
CreateNativeControl();
}
if (is_drawn) {
// The view becomes visible after native control is created.
// Layout now.
Layout();
}
}
*/

void MQTable::OnOHLC_MQ(std::string const & type, PointerWrapper<phone_module::OHLCT> const & p) {
  price_open_time_->SetText(L"开: " + common_util::Time2StringW(p.get()->ot, 2));
  price_high_time_->SetText(L"高: " + common_util::Time2StringW(p.get()->ht, 2));
  price_low_time_->SetText(L"低: " + common_util::Time2StringW(p.get()->lt, 2));
  price_close_time_->SetText(L"收: " + common_util::Time2StringW(p.get()->ct, 2));

  price_open_->SetText(ASCIIToUTF16(base::DoubleToString(p.get()->op)));
  price_high_->SetText(ASCIIToUTF16(base::DoubleToString(p.get()->hp)));
  price_low_->SetText(ASCIIToUTF16(base::DoubleToString(p.get()->lp)));
  price_close_->SetText(ASCIIToUTF16(base::DoubleToString(p.get()->cp)));
}

void MQTable::OnMQ(int32 const type, int32 const id, base::Time const & time, PointerWrapper<phone_module::LevelQuotationInfoBig> const & p) {
#if 0
  temp_data_.push_back(std::shared_ptr<phone_module::LevelQuotationInfoBig>(new phone_module::LevelQuotationInfoBig(*p.get())));
  if (temp_data_.size() > 60) {
    for (uint32 i = 0; i < temp_data_.size(); i++) {
      list_data_.push_back(temp_data_[i]);
      level_map_[p.get()->level].push_back(list_data_[list_data_.size() - 1].get());
      type_map_[p.get()->type].push_back(list_data_[list_data_.size() - 1].get());
      level_type_map_[std::make_pair(p.get()->level, p.get()->type)].push_back(list_data_[list_data_.size() - 1].get());
    }
    temp_data_.clear();
#endif

    uint32 const old_counter = RowCount();
    DCHECK_RLOG(old_counter == list_data_.size());
    list_data_.push_back(std::shared_ptr<phone_module::LevelQuotationInfoBig>(new phone_module::LevelQuotationInfoBig(*p.get())));
    //list_data_.push_back(std::shared_ptr<phone_module::LevelQuotationInfoBig>(new phone_module::LevelQuotationInfoBig(*p.get())));
    //list_data_.push_back(std::shared_ptr<phone_module::LevelQuotationInfoBig>(new phone_module::LevelQuotationInfoBig(*p.get())));
    //list_data_.push_back(std::shared_ptr<phone_module::LevelQuotationInfoBig>(new phone_module::LevelQuotationInfoBig(*p.get())));
    //list_data_.push_back(std::shared_ptr<phone_module::LevelQuotationInfoBig>(new phone_module::LevelQuotationInfoBig(*p.get())));

    level_map_[p.get()->level].push_back(list_data_[list_data_.size() - 1].get());
    type_map_[p.get()->type].push_back(list_data_[list_data_.size() - 1].get());
    level_type_map_[std::make_pair(p.get()->level, p.get()->type)].push_back(list_data_[list_data_.size() - 1].get());


    uint32 const old_tc = table_->RowCount();
    table_->OnItemsAdded(old_counter, list_data_.size() - old_counter);
    EnsureVisible();
  //}
  

  //通知其他ui线程对象价格改变，比如持仓显示=
  //  [8/8/2016 liuqingping]这段代码干啥用？？？？
  content::NotificationService::current()->Notify(
    phone_module::NOTIFICATION_CTP_MQ_MQLEVEL_INFO,
    content::NotificationService::AllSources(),
    content::Details<phone_module::LevelQuotationInfoBig const>(p.get()));

}

void MQTable::OnInitParam(std::string const & s) {
  StringTokenizer type(s, ",");
  while(type.GetNext()) {
    type_combobox_model_.Add(type.token());
    type_combobox_->ModelChanged();
  }
}


bool MQTable::OverrideThreadForMessage(IPC::Message const& message,
  CommonThread::ID* thread) {
    if (message.type() == CTP_CONSOLE_Command::ID || 
      message.type() == CTP_CONSOLE_CommandProtoBuf::ID) {
        *thread = CommonThread::UI;
        return true;
    }

    return false;
}

void MQTable::Selected() {
  type_combobox_->ModelChanged();
  level_combobox_->ModelChanged();
}

}  // namespace examples
}  // namespace views
