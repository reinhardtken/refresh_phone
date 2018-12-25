// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "backtesting_table.h"

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
#include "ui/views/widget/widget.h"


#include "../../../phone_common/include/ctp_notification_types.h"
#include "../tabbed_pane_example.h"

namespace {

}


namespace views {
namespace examples {

  
  //================================================================

BacktestingView::BacktestingView(CTPTabbedPane *p, std::string const &bc, int32 const id) 
  :CTPViewBase(L"Table"),
  p_(p),
  bc_(bc),
  id_(id),
  ThreadMessageFilter(true) {

}

BacktestingView::~BacktestingView() {
}

void BacktestingView::OnSelectedIndexChanged(Combobox* combobox) {
  
}

void BacktestingView::CreateExampleView(View* container) {
  GridLayout* layout = new GridLayout(container);
  container->SetLayoutManager(layout);

  uint32 index = 0;
  ColumnSet*column_set = NULL;
  //connect_bar_ = new ProgressBar();
  //connect_bar_->SetValue(0.5);

  //ColumnSet* column_set = layout->AddColumnSet(0);
  //column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
 //   1.0f, GridLayout::USE_PREF, 0, 0);
  //layout->StartRow(0 /* no expand */, 0);
  //layout->AddView(connect_bar_);

  //===========================
  type_ = new Label(L"品种:");
  type_->SetHorizontalAlignment(Label::ALIGN_LEFT);
  start_ = new Label(L"开始时间:");
  start_->SetHorizontalAlignment(Label::ALIGN_LEFT);
  end_ = new Label(L"结束时间:");
  end_->SetHorizontalAlignment(Label::ALIGN_LEFT);
  level_ = new Label(L"级别:");
  level_->SetHorizontalAlignment(Label::ALIGN_LEFT);

  column_set = layout->AddColumnSet(index);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  layout->StartRow(0 /* no expand */, index);
  layout->AddView(type_);
  layout->AddView(start_);
  layout->AddView(end_);
  layout->AddView(level_);
  //==================================================
  index++;
  init_button_ = new TextButton(this, ASCIIToWide(bc_) + L" 准备数据");
  init_button_->set_alignment(TextButton::ALIGN_CENTER);
  column_set = layout->AddColumnSet(index);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  layout->StartRow(0 /* no expand */, index);
  layout->AddView(init_button_);
  //=============================
  index++;
  total_ = new Label(L"回测样本数目:");
  total_->SetHorizontalAlignment(Label::ALIGN_LEFT);
  high_level_total_ = new Label(L"高等级行情加总数目:");
  high_level_total_->SetHorizontalAlignment(Label::ALIGN_LEFT);
  cost_time_ = new Label(L"加载数据耗时:");
  cost_time_->SetHorizontalAlignment(Label::ALIGN_LEFT);

  column_set = layout->AddColumnSet(index);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  
  layout->StartRow(0 /* no expand */, index);
  layout->AddView(total_);
  layout->AddView(high_level_total_);
  layout->AddView(cost_time_);
  //============================
  index++;
  init_bar_ = new ProgressBar();
  init_bar_->SetValue(0);
  column_set = layout->AddColumnSet(index);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  layout->StartRow(0 /* no expand */, index);
  layout->AddView(init_bar_);
  //==============================================
  
  //==================================================
  index++;
  expect_button_ = new TextButton(this, ASCIIToWide(bc_) + L" 加载默认预期结果");
  expect_button_->set_alignment(TextButton::ALIGN_CENTER);
  prop_expect_button_ = new TextButton(this, ASCIIToWide(bc_) + L" 加载配置文件设定的预期结果");
  prop_expect_button_->set_alignment(TextButton::ALIGN_CENTER);
  
  column_set = layout->AddColumnSet(index);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  layout->StartRow(0 /* no expand */, index);
  layout->AddView(expect_button_);
  layout->AddView(prop_expect_button_);
  //=============================
  index++;
  expect_total_ = new Label(L"预期结果数目:");
  expect_total_->SetHorizontalAlignment(Label::ALIGN_LEFT);
  expect_cost_time_ = new Label(L"加载数据耗时:");
  expect_cost_time_->SetHorizontalAlignment(Label::ALIGN_LEFT);

  column_set = layout->AddColumnSet(index);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);

  layout->StartRow(0 /* no expand */, index);
  layout->AddView(expect_total_);
  layout->AddView(expect_cost_time_);
  //============================
  index++;
  expect_bar_ = new ProgressBar();
  expect_bar_->SetValue(0);
  column_set = layout->AddColumnSet(index);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  layout->StartRow(0 /* no expand */, index);
  layout->AddView(expect_bar_);
  //==============================================


  index++;
  max_retracement_ = new Label(L"最大回撤:");
  max_retracement_->SetHorizontalAlignment(Label::ALIGN_LEFT);
  progress_ = new Label(L"已处理样本数目:");
  progress_->SetHorizontalAlignment(Label::ALIGN_LEFT);
  run_cost_time_ = new Label(L"耗时:");
  run_cost_time_->SetHorizontalAlignment(Label::ALIGN_LEFT);

  run_c_cost_time_ = new Label(L"C模块耗时:");
  run_c_cost_time_->SetHorizontalAlignment(Label::ALIGN_LEFT);

  column_set = layout->AddColumnSet(index);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  layout->StartRow(0 /* no expand */, index);
  layout->AddView(max_retracement_);
  layout->AddView(progress_);
  layout->AddView(run_cost_time_);
  layout->AddView(run_c_cost_time_);
  //==================================================
  index++;
  backtesting_start_button_ = new TextButton(this, ASCIIToWide(bc_) + L" 开始回测");
  backtesting_start_button_->set_alignment(TextButton::ALIGN_CENTER);
  backtesting_pause_button_ = new TextButton(this, ASCIIToWide(bc_) + L" 暂停回测");
  backtesting_pause_button_->set_alignment(TextButton::ALIGN_CENTER);
  backtesting_giveup_button_ = new TextButton(this, ASCIIToWide(bc_) + L" 停止回测");
  backtesting_giveup_button_->set_alignment(TextButton::ALIGN_CENTER);
  column_set = layout->AddColumnSet(index);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
/*
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);*/
  layout->StartRow(0 /* no expand */, index);
  layout->AddView(backtesting_start_button_);
  //layout->AddView(backtesting_pause_button_);
  layout->AddView(backtesting_giveup_button_);
  //=============================
  index++;
  backtesting_bar_ = new ProgressBar();
  backtesting_bar_->SetValue(0);
  column_set = layout->AddColumnSet(index);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
     1.0f, GridLayout::USE_PREF, 0, 0);
  layout->StartRow(0 /* no expand */, index);
  layout->AddView(backtesting_bar_);
  //==================================
  index++;
  principal_ = new Label(L"本金:");
  principal_->SetHorizontalAlignment(Label::ALIGN_LEFT);
  max_profit_ = new Label(L"最大值:");
  max_profit_->SetHorizontalAlignment(Label::ALIGN_LEFT);
  min_profit_ = new Label(L"最小值:");
  min_profit_->SetHorizontalAlignment(Label::ALIGN_LEFT);
  profit_ = new Label(L"终值:");
  profit_->SetHorizontalAlignment(Label::ALIGN_LEFT);

  column_set = layout->AddColumnSet(index);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  layout->StartRow(0 /* no expand */, index);
  layout->AddView(principal_);
  layout->AddView(max_profit_);
  layout->AddView(min_profit_);
  layout->AddView(profit_);
  //==============================================
  index++;
  buy_times_ = new Label(L"开多次数:");
  buy_times_->SetHorizontalAlignment(Label::ALIGN_LEFT);
  sell_times_ = new Label(L"开空次数:");
  sell_times_->SetHorizontalAlignment(Label::ALIGN_LEFT);
  stop_times_ = new Label(L"止损次数:");
  stop_times_->SetHorizontalAlignment(Label::ALIGN_LEFT);
  stop_win_times_ = new Label(L"止盈次数:");
  stop_win_times_->SetHorizontalAlignment(Label::ALIGN_LEFT);
  sxf_ = new Label(L"手续费:");
  sxf_->SetHorizontalAlignment(Label::ALIGN_LEFT);

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
  layout->StartRow(0 /* no expand */, index);
  layout->AddView(buy_times_);
  layout->AddView(sell_times_);
  layout->AddView(stop_times_);
  layout->AddView(stop_win_times_);
  layout->AddView(sxf_);

  //==============================================
  index++;
  pound_total_ = new Label(L"冲击成本:");
  pound_total_->SetHorizontalAlignment(Label::ALIGN_LEFT);

  
  max_successive_deficit_period_ = new Label(L"最大连续亏损周期:");
  max_successive_deficit_period_->SetHorizontalAlignment(Label::ALIGN_LEFT);

  max_no_newhigh_period_ = new Label(L"最大不创新高周期:");
  max_no_newhigh_period_->SetHorizontalAlignment(Label::ALIGN_LEFT);


  column_set = layout->AddColumnSet(index);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);

  layout->StartRow(0 /* no expand */, index);
  layout->AddView(pound_total_);
  layout->AddView(max_successive_deficit_period_);
  layout->AddView(max_no_newhigh_period_);
  
  //==============================================
  index++;

  net_profit_ = new Label(L"净赚累加:");
  net_profit_->SetHorizontalAlignment(Label::ALIGN_LEFT);

  net_deficit_ = new Label(L"净亏累加:");
  net_deficit_->SetHorizontalAlignment(Label::ALIGN_LEFT);

  max_deviation_ = new Label(L"累计最大误差:");
  max_deviation_->SetHorizontalAlignment(Label::ALIGN_LEFT);
  


  column_set = layout->AddColumnSet(index);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);

  layout->StartRow(0 /* no expand */, index);
  layout->AddView(net_profit_);
  layout->AddView(net_deficit_);
  layout->AddView(max_deviation_);

  //==============================================
  index++;

  mean_value_ = new Label(L"平均值:");
  mean_value_->SetHorizontalAlignment(Label::ALIGN_LEFT);

  standard_deviation_ = new Label(L"标准差:");
  standard_deviation_->SetHorizontalAlignment(Label::ALIGN_LEFT);

  sharp_radio_ = new Label(L"夏普率:");
  sharp_radio_->SetHorizontalAlignment(Label::ALIGN_LEFT);



  column_set = layout->AddColumnSet(index);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);

  layout->StartRow(0 /* no expand */, index);
  layout->AddView(mean_value_);
  layout->AddView(standard_deviation_);
  layout->AddView(sharp_radio_);

  //==============================================
  index++;

  result_2_db_label_ = new Label(L"回测结果存至db的表名：");
  result_2_db_label_->SetHorizontalAlignment(Label::ALIGN_LEFT);

  result_2_db_tablename_ = new Textfield();

  column_set = layout->AddColumnSet(index);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);

  layout->StartRow(0 /* no expand */, index);
  layout->AddView(result_2_db_label_);
  layout->AddView(result_2_db_tablename_);

  //==============================================
  index++;

  result_2_db_button_ = new TextButton(this, ASCIIToWide(bc_) + L" 保存回测结果到db");
  result_2_db_button_->set_alignment(TextButton::ALIGN_CENTER);

  result_2_pic_ = new Checkbox(L"生成走势图");
  
  

  column_set = layout->AddColumnSet(index);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);

  layout->StartRow(0 /* no expand */, index);
  layout->AddView(result_2_db_button_);
  layout->AddView(result_2_pic_);
  //=============================
  index++;
  result_2_db_bar_ = new ProgressBar();
  result_2_db_bar_->SetValue(0);
  column_set = layout->AddColumnSet(index);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  layout->StartRow(0 /* no expand */, index);
  layout->AddView(result_2_db_bar_);
  
}


void BacktestingView::ButtonPressed(Button* sender, const ui::Event& event) {
  if (sender == init_button_) {
    start_time_ = base::Time::Now();
    ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::CTP, new CTP_BacktestingInit(id_));
  } else if (sender == backtesting_start_button_) {
    //start_time2_ = base::Time::Now();
    ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::CTP, new CTP_BacktestingStart());
  } else if (sender == backtesting_pause_button_) {
    ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::CTP, new CTP_BacktestingPause());
  } else if (sender == backtesting_giveup_button_) {
    ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::CTP, new CTP_BacktestingGiveup());
  } else if (sender == expect_button_) {
    ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::CTP, new CTP_BacktestingDefaultExpectInit());
  } else if (sender == result_2_db_button_) {
    std::wstring table_name = result_2_db_tablename_->text();
    if (!table_name.size()) {
      MessageBox(example_view()->GetWidget()->GetNativeWindow(), 
        L"保存回测结果的表名为空，无法保存", L"表名为空", MB_OK);
        return;
    }
    std::string name = ::WideToASCII(table_name);
    bool chart = result_2_pic_->checked();
    LOG(INFO)<<"save to db "<<name;
    ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::CTP, new CTP_BacktestingResult2DB(name, chart));
  } else if (sender == prop_expect_button_) {
    ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::CTP, new CTP_BacktestingPropExpectInit());
  } else if (sender == result_2_pic_) {
    /*std::wstring table_name = result_2_db_tablename_->text();
    if (!table_name.size()) {
      MessageBox(example_view()->GetWidget()->GetNativeWindow(), 
        L"图片名称为空，需要名称", L"图片名称", MB_OK);
      return;
    }
    std::string name = ::WideToASCII(table_name);
    ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::CTP, new CTP_BacktestingResult2Chart(name));*/
  }
  
}



void BacktestingView::Observe(int type,
  const content::NotificationSource& source,
  const content::NotificationDetails& details) {

}

bool BacktestingView::OnMessageReceived(IPC::Message const & msg) {


  if (msg.routing_id() == id_) {
    // Dispatch control messages.
    bool msg_is_ok = false;
    IPC_BEGIN_MESSAGE_MAP_EX(BacktestingView, msg, msg_is_ok)

      IPC_MESSAGE_HANDLER(CTP_BacktestingInitProgress2UI, OnInitProgress)
      IPC_MESSAGE_HANDLER(CTP_BacktestingRunProgress2UI, OnRunProgress)
      IPC_MESSAGE_HANDLER(CTP_BacktestingExpectInitProgress2UI, OnExpectInitProgress)
      IPC_MESSAGE_HANDLER(CTP_BacktestingResult2DBProgress2UI, OnResultSave2DB)
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

void BacktestingView::OnInitProgress(double const index, double const total, double const high_index, double const high_total) {
  if (init_bar_) {
    std::wstring t = base::StringPrintf(L"回测样本数目: %lf / %lf", index, total);
    total_->SetText(t);
    t = base::StringPrintf(L"高等级数据加总数目: %lf / %lf", high_index, high_total);
    high_level_total_->SetText(t);

    base::TimeDelta diff = base::Time::Now() - start_time_;
    std::wstring c = base::StringPrintf(L"加载数据耗时: %d 秒", diff.ToInternalValue() / base::Time::kMicrosecondsPerSecond);
    cost_time_->SetText(c);
   
    init_bar_->SetValue(index / total);
  }
}

void BacktestingView::OnExpectInitProgress(double const index, double const total) {
  if (expect_bar_) {
    std::wstring t = base::StringPrintf(L"预期结果数目: %lf / %lf", index, total);
    expect_total_->SetText(t);
    //base::TimeDelta diff = base::Time::Now() - start_time_;
    //std::wstring c = base::StringPrintf(L"加载数据耗时: %d 秒", diff.ToInternalValue() / base::Time::kMicrosecondsPerSecond);
    //cost_time_->SetText(c);

    expect_bar_->SetValue(index / total);
  }
}

void BacktestingView::OnResultSave2DB(double const index, double const total) {
  if (result_2_db_bar_) {
    result_2_db_bar_->SetValue(index / total);
  }
}

void BacktestingView::OnRunProgress(PointerWrapper<backtesting::DataCenter> const &p) {
  if (backtesting_bar_) {
    backtesting::DataCenter const * pdc = p.get();

    std::wstring t = base::StringPrintf(L"已处理样本数目: %d / %d", pdc->digest.index, pdc->digest.total);
    progress_->SetText(t);
    //base::TimeDelta diff = base::Time::Now() - start_time2_;
    std::wstring c = base::StringPrintf(L"耗时: %d 秒", pdc->digest.cost_time.ToInternalValue() / base::Time::kMicrosecondsPerSecond);
    run_cost_time_->SetText(c);

    c = base::StringPrintf(L"C模块耗时: %d 秒", pdc->digest.c_cost_time.ToInternalValue() / base::Time::kMicrosecondsPerSecond);
    run_c_cost_time_->SetText(c);

    backtesting_bar_->SetValue((double)pdc->digest.index / (double)pdc->digest.total);

    t = base::StringPrintf(L"本金: %lf", pdc->origin_money);
    principal_->SetText(t);

    t = base::StringPrintf(L"最大: %lf", pdc->max_money);
    max_profit_->SetText(t);

    t = base::StringPrintf(L"最小: %lf", pdc->min_money);
    min_profit_->SetText(t);

    if (pdc->volumn == 0) {
      t = base::StringPrintf(L"终值: %lf", pdc->money);
    } else {
      t = base::StringPrintf(L"终值: %lf", pdc->pre_open_money);
    }
    profit_->SetText(t);

    t = base::StringPrintf(L"开多次数: %d", pdc->buy_times);
    buy_times_->SetText(t);

    t = base::StringPrintf(L"开空次数: %d", pdc->sell_times);
    sell_times_->SetText(t);

    t = base::StringPrintf(L"止损次数: %d", pdc->stop_lose_times);
    stop_times_->SetText(t);

    t = base::StringPrintf(L"止盈次数: %d", pdc->stop_win_times);
    stop_win_times_->SetText(t);

    t = base::StringPrintf(L"最大回撤: %lf", pdc->max_retracement);
    max_retracement_->SetText(t);
    t = base::StringPrintf(L"手续费: %lf", pdc->fee_total);
    sxf_->SetText(t);

    t = base::StringPrintf(L"冲击成本: %lf", pdc->pound_total);
    pound_total_->SetText(t);

    t = base::StringPrintf(L"最大连续亏损周期: %d", pdc->max_successive_deficit_period);
    max_successive_deficit_period_->SetText(t);

    t = base::StringPrintf(L"最大不创新高周期: %d", pdc->max_no_newhigh_period);
    max_no_newhigh_period_->SetText(t);

    t = base::StringPrintf(L"净赚累加: %lf", pdc->net_profit);
    net_profit_->SetText(t);

    t = base::StringPrintf(L"净亏累加: %lf", pdc->net_deficit);
    net_deficit_->SetText(t);

    t = base::StringPrintf(L"累计最大误差: %lf", pdc->max_diff);
    max_deviation_->SetText(t);

    t = base::StringPrintf(L"平均值: %lf", pdc->mean_value);
    mean_value_->SetText(t);

    t = base::StringPrintf(L"均方差: %lf", pdc->standard_deviation);
    standard_deviation_->SetText(t);

    t = base::StringPrintf(L"夏普率: %lf", pdc->sharp_radio);
    sharp_radio_->SetText(t);
    
    
  }
}


}  // namespace examples
}  // namespace views
