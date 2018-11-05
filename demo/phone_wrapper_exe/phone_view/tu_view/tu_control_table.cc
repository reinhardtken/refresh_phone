// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "tu_control_table.h"

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
#include "../../../phone_common/common/common_util.h"
#include "../tabbed_pane_example.h"

namespace {
  
}


namespace views {
namespace examples {

  
  //================================================================

TUControlTable::TUControlTable(CTPTabbedPane *p, phone_module::TradeUnitInfo const &info) 
  :CTPViewBase("Table"),
  p_(p),
  info_(info),
  ThreadMessageFilter(true) {

/*
    if (pane_->bc_2_id_.find(bc_) != pane_->bc_2_id_.end()) {
      ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::CTP,
        new CTP_TU_QryTypeList(pane_->bc_2_id_[bc_]));
    } else {
      DCHECK(false);
    }*/
    

}

TUControlTable::~TUControlTable() {
}

void TUControlTable::OnSelectedIndexChanged(Combobox* combobox) {
  
}

void TUControlTable::CreateExampleView(View* container) {
  GridLayout* layout = new GridLayout(container);
  container->SetLayoutManager(layout);

  uint32 index = 0;
  //connect_bar_ = new ProgressBar();
  //connect_bar_->SetValue(0.5);

  //ColumnSet* column_set = layout->AddColumnSet(0);
  //column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
 //   1.0f, GridLayout::USE_PREF, 0, 0);
  //layout->StartRow(0 /* no expand */, 0);
  //layout->AddView(connect_bar_);

  //==================================================
  connect_button_ = new TextButton(this, ASCIIToWide(info_.bc) + L" 登录");
  connect_button_->set_alignment(TextButton::ALIGN_CENTER);
  disconnect_button_ = new TextButton(this, ASCIIToWide(info_.bc) + L" 登出");
  disconnect_button_->set_alignment(TextButton::ALIGN_CENTER);

  ColumnSet*column_set = layout->AddColumnSet(index);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  layout->StartRow(0 /* no expand */, index);
  layout->AddView(connect_button_);
  layout->AddView(disconnect_button_);
  //==================================
  index++;
  login_status_ = new Label;
  column_set = layout->AddColumnSet(index);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  layout->StartRow(0 /* no expand */, index);
  layout->AddView(login_status_);

  //===================================
  index++;
  allow_execute_ms_button_ = new Checkbox(L"允许执行策略");
  allow_execute_ms_button_->set_listener(this);
  allow_execute_ms_button_->SetChecked(true);
  column_set = layout->AddColumnSet(index);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  layout->StartRow(0 /* no expand */, index);
  layout->AddView(allow_execute_ms_button_);

  //=========================================
  index++;
  query_position_button_ = new TextButton(this, ASCIIToWide(info_.bc) + L" 查询持仓");
  query_position_button_->set_alignment(TextButton::ALIGN_CENTER);

  column_set = layout->AddColumnSet(index);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  layout->StartRow(0 /* no expand */, index);
  layout->AddView(query_position_button_);

  //=========================================
  index++;
  server_ipandport_ = new Label(L"服务器ip及端口：");
  server_ipandport_->SetHorizontalAlignment(Label::ALIGN_LEFT);
  server_ipandport_content_ = new Label(ASCIIToWide(info_.server_ip));
  server_ipandport_content_->SetHorizontalAlignment(Label::ALIGN_LEFT);
  column_set = layout->AddColumnSet(index);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  layout->StartRow(0 /* no expand */, index);
  layout->AddView(server_ipandport_);
  layout->AddView(server_ipandport_content_);
  
  //=========================================
  index++;
  stop_percent_ = new Label(L"止损百分比：");
  stop_percent_->SetHorizontalAlignment(Label::ALIGN_LEFT);
  stop_percent_content_ = new Label(common_util::ToString(info_.stop_list));
  stop_percent_content_->SetHorizontalAlignment(Label::ALIGN_LEFT);
  column_set = layout->AddColumnSet(index);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  layout->StartRow(0 /* no expand */, index);
  layout->AddView(stop_percent_);
  layout->AddView(stop_percent_content_);
  
  //=========================================
  index++;
  stop_win_percent_ = new Label(L"止盈百分比：");
  stop_win_percent_->SetHorizontalAlignment(Label::ALIGN_LEFT);
  stop_win_percent_content_ = new Label(common_util::ToString(info_.stop_win_list));
  stop_win_percent_content_->SetHorizontalAlignment(Label::ALIGN_LEFT);
  column_set = layout->AddColumnSet(index);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  layout->StartRow(0 /* no expand */, index);
  layout->AddView(stop_win_percent_);
  layout->AddView(stop_win_percent_content_);
  
  //=========================================
  index++;
  strategy_ = new Label(L"允许执行的策略：");
  strategy_->SetHorizontalAlignment(Label::ALIGN_LEFT);
  strategy_content_ = new Label(common_util::ToString(info_.strategy));
  strategy_content_->SetHorizontalAlignment(Label::ALIGN_LEFT);
  column_set = layout->AddColumnSet(index);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  layout->StartRow(0 /* no expand */, index);
  layout->AddView(strategy_);
  layout->AddView(strategy_content_);
  
  //=========================================
  index++;
  volumn_count_ = new Label(L"最大持仓数量：");
  volumn_count_->SetHorizontalAlignment(Label::ALIGN_LEFT);
  volumn_count_content_ = new Label(common_util::ToString(info_.volumn_list));
  volumn_count_content_->SetHorizontalAlignment(Label::ALIGN_LEFT);
  column_set = layout->AddColumnSet(index);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  layout->StartRow(0 /* no expand */, index);
  layout->AddView(volumn_count_);
  layout->AddView(volumn_count_content_);


  //=========================================
  index++;
  type_msname_ = new Label(L"品种对应的策略名：");
  type_msname_->SetHorizontalAlignment(Label::ALIGN_LEFT);
  type_msname_content_ = new Label(common_util::ToString(info_.type_2_msname_list));
  type_msname_content_->SetHorizontalAlignment(Label::ALIGN_LEFT);
  column_set = layout->AddColumnSet(index);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  layout->StartRow(0 /* no expand */, index);
  layout->AddView(type_msname_);
  layout->AddView(type_msname_content_);

  //=========================================
  index++;
  new_stopprice_ = new Label(L"最新的动态止损点位：");
  new_stopprice_->SetHorizontalAlignment(Label::ALIGN_LEFT);
  new_stopprice_content_ = new Label();
  new_stopprice_content_->SetHorizontalAlignment(Label::ALIGN_LEFT);
  column_set = layout->AddColumnSet(index);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  layout->StartRow(0 /* no expand */, index);
  layout->AddView(new_stopprice_);
  layout->AddView(new_stopprice_content_);


  //=========================================
  index++;
  old_stopprice_ = new Label(L"之前的动态止损点位：");
  old_stopprice_->SetHorizontalAlignment(Label::ALIGN_LEFT);
  old_stopprice_content_ = new Label();
  old_stopprice_content_->SetHorizontalAlignment(Label::ALIGN_LEFT);
  column_set = layout->AddColumnSet(index);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  layout->StartRow(0 /* no expand */, index);
  layout->AddView(old_stopprice_);
  layout->AddView(old_stopprice_content_);
  
}

bool TUControlTable::RealAccountAlertOK() {
  if (info_.real_account == false) {
    return true;
  } else {
    std::string out = base::StringPrintf("券商：%s\r\n帐号：%s\r\n服务器：%s", info_.broker.c_str(), info_.account.c_str(), info_.server_ip.c_str());
    std::wstring wout = base::SysNativeMBToWide(out);
    
    if (IDOK ==MessageBox(example_view()->GetWidget()->GetNativeWindow(), 
      wout.c_str(), L"真实帐号登录确认", MB_OKCANCEL)) {
      return true;
    } else {
      return false;
    }
  }
}


void TUControlTable::ButtonPressed(Button* sender, const ui::Event& event) {
  if (sender == connect_button_) {
    if (p_->HasID(info_.bc) && RealAccountAlertOK()) {
      login_status_->SetText(L"");
      ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::CTP, new CTP_TU_Login(p_->GetID(info_.bc), info_.bc));
    } else {
      //DCHECK(false);
      LOG(INFO)<<info_.bc<<"give up login ";
    }
  } else if (sender == disconnect_button_) {
    if (p_->HasID(info_.bc)) {
      login_status_->SetText(L"");
      ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::CTP, new CTP_TU_Logout(p_->GetID(info_.bc), info_.bc));
    } else {
      DCHECK(false);
    }
  } else if (sender == allow_execute_ms_button_) {
    bool checked = allow_execute_ms_button_->checked();
    if (p_->HasID(info_.bc)) {
      ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::CTP, new CTP_TU_Allow_MS(p_->GetID(info_.bc), info_.bc, checked));
    } else {
      DCHECK(false);
    }
  } else if (sender == query_position_button_) {
    if (p_->HasID(info_.bc)) {
      ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::CTP, new CTP_TU_Query_Position(p_->GetID(info_.bc), info_.bc));
    } else {
      DCHECK(false);
    }
  }

  
}


void TUControlTable::OnStopPrice(PointerWrapper<phone_module::StopPriceData> const & p) {
  std::wstring content = common_util::ToString(p.get()->data);
  old_stopprice_content_->SetText(new_stopprice_content_->text());
  new_stopprice_content_->SetText(content);
}


void TUControlTable::Observe(int type,
  const content::NotificationSource& source,
  const content::NotificationDetails& details) {

}

bool TUControlTable::OnMessageReceived(IPC::Message const & msg) {


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



}  // namespace examples
}  // namespace views
