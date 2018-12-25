// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "mq_tabbed_pane_example.h"

#include "base/string_util.h"
#include "base/string_tokenizer.h"
#include "base/string_number_conversions.h"
#include "base/utf_string_conversions.h"
#include "base/sys_string_conversions.h"
#include "ui/views/controls/tabbed_pane/tabbed_pane.h"
#include "ui/views/layout/grid_layout.h"
#include "content/public/common/content_switches.h"

#include "../../phone_common/common/common_common.h"

#include "mq_table.h"
#include "ms_table.h"
//#include "tu_table.h"
#include "refresh_view.h"
#include "test_view.h"


namespace views {
namespace examples {

MQTabbedPane::MQTabbedPane() 
  :CTPViewBase(L"Tabbed Pane"),
  ThreadMessageFilter(true) {
}

MQTabbedPane::~MQTabbedPane() {
}

void MQTabbedPane::CreateExampleView(View* container) {
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
  connect_bar_ = new ProgressBar();
  //connect_bar_->SetValue(0.5);

  column_set = layout->AddColumnSet(2);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  layout->StartRow(0 /* no expand */, 2);
  layout->AddView(connect_bar_);

  //==================================================
  connect_button_ = new TextButton(this, L"登录行情服务器");
  connect_button_->set_alignment(TextButton::ALIGN_CENTER);
  column_set = layout->AddColumnSet(3);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  layout->StartRow(0 /* no expand */, 3);
  layout->AddView(connect_button_);

  //==================================
  login_status_ = new Label;
  column_set = layout->AddColumnSet(4);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  layout->StartRow(0 /* no expand */, 4);
  layout->AddView(login_status_);


}

void MQTabbedPane::ButtonPressed(Button* sender, const ui::Event& event) {
  if (sender == connect_button_) {
    login_status_->SetText(L"");
    ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::CTP, new CTP_MQLogin(0));
  }
}

void MQTabbedPane::TabSelectedAt(int index) {
  if (tabs_.find(index) != tabs_.end()) {
    tabs_[index]->Selected();
  } else {
    DCHECK(false);
  }

}



void MQTabbedPane::AddOneTab(const std::string& label) {
  MQTable* p = new MQTable;
  p->type(label);
  tabs_[tabbed_pane_->GetTabCount()] = p;
  type_2_mqview_[label] = p;
  tabbed_pane_->AddTab(ASCIIToUTF16(label), p->example_view());
}

bool MQTabbedPane::OnMessageReceived(IPC::Message const & msg) {


  //CommonThread::ID thread;

/*
  if (!CommonThread::GetCurrentThreadIdentifier(&thread) &&
    OverrideThreadForMessage(msg, &thread)) {

      CommonThread::PostTask(
        thread, FROM_HERE,
        base::Bind(base::IgnoreResult(&TUTable::OnMessageReceived),
        base::Unretained(this), msg));

      return true;
  }*/


  if (msg.routing_id() == MSG_ROUTING_CONTROL) {
    // Dispatch control messages.
    bool msg_is_ok = false;
    IPC_BEGIN_MESSAGE_MAP_EX(MQTabbedPane, msg, msg_is_ok)
      
      IPC_MESSAGE_HANDLER(CTP_MQLevel2UI, OnMQ)
      IPC_MESSAGE_HANDLER(CTP_OHLC2UI, OnOHLC_MQ)
      IPC_MESSAGE_HANDLER(CTP_MQ_Login_Success_2UI, OnLoginSuccess)
      IPC_MESSAGE_HANDLER(CTP_MQ_Login_Failed_2UI, OnLoginFailed)
      IPC_MESSAGE_HANDLER(CTP_MQ_Logout_2UI, OnDisConnect)
      IPC_MESSAGE_HANDLER(CTP_MQLevelParam, OnInitParam)
      IPC_END_MESSAGE_MAP_EX()

      if (msg_is_ok) {
        return true;
      }
  }

  // Dispatch incoming messages to the appropriate RenderView/WidgetHost.
  //RenderWidgetHost* rwh = render_widget_hosts_.Lookup(msg.routing_id());

  return false; 

}


void MQTabbedPane::OnInitParam(std::string const & s) {
  StringTokenizer type(s, ",");
  while(type.GetNext()) {
    //type_combobox_model_.Add(type.token());
    //type_combobox_->ModelChanged();
    AddOneTab(type.token());
  }
}

void MQTabbedPane::OnMQ(int32 const type, int32 const id, base::Time const & time, PointerWrapper<phone_module::LevelQuotationInfoBig> const & p) {
  auto it = type_2_mqview_.find(p.get()->type);
  if (it != type_2_mqview_.end()) {
    it->second->OnMQ(type, id, time, p);
  }
}

void MQTabbedPane::OnOHLC_MQ(std::string const & type, PointerWrapper<phone_module::OHLCT> const & p) {
  auto it = type_2_mqview_.find(type);
  if (it != type_2_mqview_.end()) {
    it->second->OnOHLC_MQ(type, p);
  }
}

void MQTabbedPane::OnLoginSuccess() {
  connect_bar_->SetValue(1);
}

void MQTabbedPane::OnLoginFailed(uint32 const id, std::string const & msg) {
  login_status_->SetText(ASCIIToUTF16(base::UintToString(id)) + L"," + base::SysNativeMBToWide(msg));
}


void MQTabbedPane::OnDisConnect() {
  connect_bar_->SetValue(0);
}


void MQTabbedPane::Observe(int type,
  const content::NotificationSource& source,
  const content::NotificationDetails& details) {
    
}

}  // namespace examples
}  // namespace views
