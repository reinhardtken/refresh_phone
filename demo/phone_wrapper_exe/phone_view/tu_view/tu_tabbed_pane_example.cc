// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "tu_tabbed_pane_example.h"


#include "base/string_number_conversions.h"
#include "base/utf_string_conversions.h"
#include "base/sys_string_conversions.h"
#include "ui/views/controls/tabbed_pane/tabbed_pane.h"
#include "ui/views/layout/grid_layout.h"
#include "content/public/common/content_switches.h"

#include "../../../phone_common/common/common_common.h"


#include "tu_table.h"
#include "tu_order_result_table.h"
#include "../tabbed_pane_example.h"
#include "tu_control_table.h"
#include "tu_marginrate_table.h"
#include "tu_commission_table.h"

namespace views {
namespace examples {

TUTabbedPane::TUTabbedPane(CTPTabbedPane *p, phone_module::TradeUnitInfo const &info) 
  :CTPViewBase("Tabbed Pane"),
  p_(p),
  info_(info),
  ThreadMessageFilter(false) {
}

TUTabbedPane::~TUTabbedPane() {
}

void TUTabbedPane::CreateExampleView(View* container) {
  tabbed_pane_ = new TabbedPane();
  tabbed_pane_->set_use_native_win_control(true);
  tabbed_pane_->set_listener(this);


  GridLayout* layout = new GridLayout(container);
  container->SetLayoutManager(layout);

  const int tabbed_pane_column = 0;
  ColumnSet* column_set = layout->AddColumnSet(tabbed_pane_column);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
                        1.0f, GridLayout::USE_PREF, 0, 0);
  layout->StartRow(1 /* expand */, tabbed_pane_column);
  layout->AddView(tabbed_pane_);

  //=============================================
  ///=============================================

  connect_bar_ = new ProgressBar();
  //connect_bar_->SetValue(0.5);

  column_set = layout->AddColumnSet(1);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  layout->StartRow(0 /* no expand */, 1);
  layout->AddView(connect_bar_);

  //==================================================
  #if 0
  connect_button_ = new TextButton(this, ASCIIToWide(bc_) + L" 登录");
  connect_button_->set_alignment(TextButton::ALIGN_CENTER);
  disconnect_button_ = new TextButton(this, ASCIIToWide(bc_) + L" 登出");
  disconnect_button_->set_alignment(TextButton::ALIGN_CENTER);

  column_set = layout->AddColumnSet(2);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  layout->StartRow(0 /* no expand */, 2);
  layout->AddView(connect_button_);
  layout->AddView(disconnect_button_);
  //==================================
  login_status_ = new Label;
  column_set = layout->AddColumnSet(3);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  layout->StartRow(0 /* no expand */, 3);
  layout->AddView(login_status_);

  //===================================
  allow_execute_ms_button_ = new Checkbox(L"允许执行策略");
  allow_execute_ms_button_->set_listener(this);
  allow_execute_ms_button_->SetChecked(true);
  column_set = layout->AddColumnSet(4);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  layout->StartRow(0 /* no expand */, 4);
  layout->AddView(allow_execute_ms_button_);
#endif
  //================================================================
  AddOneTab("control");
  AddOneTab("order_result");
  AddOneTab("tu");
  AddOneTab("marginrate");
  AddOneTab("commission");
  



}

void TUTabbedPane::ButtonPressed(Button* sender, const ui::Event& event) {
#if 0
  if (sender == connect_button_) {
    if (p_->bc_2_id_.find(bc_) != p_->bc_2_id_.end()) {
      login_status_->SetText(L"");
      ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::CTP, new CTP_TU_Login(p_->bc_2_id_[bc_], bc_));
    } else {
      DCHECK(false);
    }
  } else if (sender == disconnect_button_) {
    if (p_->bc_2_id_.find(bc_) != p_->bc_2_id_.end()) {
      login_status_->SetText(L"");
      ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::CTP, new CTP_TU_Logout(p_->bc_2_id_[bc_], bc_));
    } else {
      DCHECK(false);
    }
  } else if (sender == allow_execute_ms_button_) {
    bool checked = allow_execute_ms_button_->checked();
    if (p_->bc_2_id_.find(bc_) != p_->bc_2_id_.end()) {
      ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::CTP, new CTP_TU_Allow_MS(p_->bc_2_id_[bc_], bc_, checked));
    } else {
      DCHECK(false);
    }
  }
#endif
}

void TUTabbedPane::TabSelectedAt(int index) {
  if (tabs_.find(index) != tabs_.end()) {
    tabs_[index]->Selected();
  } else {
    DCHECK(false);
  }

}



void TUTabbedPane::AddOneTab(const std::string& label) {
  if (label == "tu") {
    TUTable* p = new TUTable(p_, info_.bc);
    
    tabs_[tabbed_pane_->GetTabCount()] = p;
    tabbed_pane_->AddTab(ASCIIToUTF16(label), p->example_view());
    p_trade_unit_ = p;
  } else if (label == "order_result") {
    OrderResultTable* p = new OrderResultTable(p_, info_.bc);

    tabs_[tabbed_pane_->GetTabCount()] = p;
    tabbed_pane_->AddTab(ASCIIToUTF16(label), p->example_view());
    p_order_result_ = p;
  } else if (label == "control") {
    TUControlTable* p = new TUControlTable(p_, info_);

    tabs_[tabbed_pane_->GetTabCount()] = p;
    tabbed_pane_->AddTab(ASCIIToUTF16(label), p->example_view());
    p_control_ = p;
  }  else if (label == "marginrate") {
    MarginRateTable* p = new MarginRateTable(p_, info_.bc);

    tabs_[tabbed_pane_->GetTabCount()] = p;
    tabbed_pane_->AddTab(ASCIIToUTF16(label), p->example_view());
    p_marginrate_ = p;
  }  else if (label == "commission") {
    CommissionRateTable* p = new CommissionRateTable(p_, info_.bc);

    tabs_[tabbed_pane_->GetTabCount()] = p;
    tabbed_pane_->AddTab(ASCIIToUTF16(label), p->example_view());
    p_commissionrate_ = p;
  }
  
}

bool TUTabbedPane::OnMessageReceived(IPC::Message const & msg) {

  if (msg.routing_id() == MSG_ROUTING_CONTROL) {
    // Dispatch control messages.
/*
    bool msg_is_ok = false;

    IPC_BEGIN_MESSAGE_MAP_EX(TUTabbedPane, msg, msg_is_ok)
      
      IPC_MESSAGE_HANDLER(CTP_TU_Login_Success_2UI, OnLoginSuccess)
      IPC_MESSAGE_HANDLER(CTP_TU_Logout_2UI, OnDisConnect)
      //IPC_MESSAGE_HANDLER(CTP_MQ_Logout_2UI, OnDisConnect)
      IPC_END_MESSAGE_MAP_EX()

      if (msg_is_ok) {
        return true;
      }*/
    DCHECK_RLOG(false);
    return false;
  }

  bool msg_is_ok = false;

  IPC_BEGIN_MESSAGE_MAP_EX(TUTabbedPane, msg, msg_is_ok)

    IPC_MESSAGE_HANDLER(CTP_TU_Login_Success_2UI, OnLoginSuccess)
    IPC_MESSAGE_HANDLER(CTP_TU_Login_Failed_2UI, OnLoginFailed)
    IPC_MESSAGE_HANDLER(CTP_TU_Disconnect_2UI, OnDisConnect)
    IPC_MESSAGE_HANDLER(CTP_TU_Logout_Success_2UI, OnLogoutSuccess)
    IPC_MESSAGE_HANDLER(CTP_MarginRate2UI, OnMarginRate)
    IPC_MESSAGE_HANDLER(CTP_CommissionRate2UI, OnCommissionRate)
  IPC_END_MESSAGE_MAP_EX()

  if (msg_is_ok) {
    return true;
  }

  // Dispatch incoming messages to the appropriate RenderView/WidgetHost.
  //RenderWidgetHost* rwh = render_widget_hosts_.Lookup(msg.routing_id());

  return false; 

}

void TUTabbedPane::OnMarginRate(PointerWrapper<CThostFtdcInstrumentMarginRateField> const & p) {
  if (p_marginrate_) {
    p_marginrate_->OnMarginRate(p);
  }
}

void TUTabbedPane::OnCommissionRate(PointerWrapper<CThostFtdcInstrumentCommissionRateField> const & p) {
  if (p_commissionrate_) {
    p_commissionrate_->OnCommissionRate(p);
  }
}

void TUTabbedPane::OnMQ(int32 const type, int32 const id, base::Time const & time, PointerWrapper<phone_module::LevelQuotationInfoBig> const & p) {
/*
  auto it = type_2_mqview_.find(p.get()->type);
  if (it != type_2_mqview_.end()) {
    it->second->OnMQ(type, id, time, p);
  }*/
}

void TUTabbedPane::OnLoginSuccess(std::string const & ba) {
  CHECK(ba == info_.bc);
  connect_bar_->SetValue(1);
  //p_control_->connect_bar_->SetValue(1);
}

void TUTabbedPane::OnLogoutSuccess(std::string const & ba) {
  CHECK(ba == info_.bc);
  connect_bar_->SetValue(0.5);
  //p_control_->connect_bar_->SetValue(0.5);
}

void TUTabbedPane::OnLoginFailed(std::string const & ba, uint32 const id, std::string const & msg) {
  CHECK(ba == info_.bc);
  //login_status_->SetText(ASCIIToUTF16(base::UintToString(id)) + L"," + base::SysNativeMBToWide(msg));
  p_control_->login_status_->SetText(ASCIIToUTF16(base::UintToString(id)) + L"," + base::SysNativeMBToWide(msg));
}


void TUTabbedPane::OnDisConnect(std::string const & ba) {
  CHECK(ba == info_.bc);
  connect_bar_->SetValue(0);
  //p_control_->connect_bar_->SetValue(0);
}


void TUTabbedPane::Observe(int type,
  const content::NotificationSource& source,
  const content::NotificationDetails& details) {
    
}

void TUTabbedPane::OnOrder(PointerWrapper<phone_module::OrderFootprints::OneRecord> const & p) {
  p_trade_unit_->OnOrder(p);
}

void TUTabbedPane::OnCash(PointerWrapper<CThostFtdcTradingAccountField> const & p) {
 p_trade_unit_->OnCash(p);
}

void TUTabbedPane::OnPosition(PointerWrapper<phone_module::PositionData> const & p) {
  p_order_result_->OnPosition(p);
}

//void TUTabbedPane::OnOrderResult(PointerWrapper<order_execute::OrderUnit2UI> const & p) {
//  p_order_result_->OnOrderResult(p);
//}

void TUTabbedPane::OnTypeList(std::vector<std::string> const &v) {
  p_trade_unit_->OnTypeList(v);
}

void TUTabbedPane::OnStopPrice(PointerWrapper<phone_module::StopPriceData> const & p) {
  p_control_->OnStopPrice(p);
}

}  // namespace examples
}  // namespace views
