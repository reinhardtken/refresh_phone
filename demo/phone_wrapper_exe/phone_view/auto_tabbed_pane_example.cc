// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "auto_tabbed_pane_example.h"

#include "base/utf_string_conversions.h"
#include "ui/views/controls/tabbed_pane/tabbed_pane.h"
#include "ui/views/layout/grid_layout.h"
#include "content/public/common/content_switches.h"

#include "../../phone_common/common/common_common.h"

#include "mq_table.h"
#include "ms_table.h"
#include "tu_view/tu_tabbed_pane_example.h"
#include "refresh_view.h"
#include "test_view.h"
#include "mq_tabbed_pane_example.h"
#include "mq_guard_view.h"
#include "backtesting_view/backtesting_table.h"
#include "adb_view/package_list_table.h"
#include "adb_view/install_apk_list_table.h"
#include "adb_view/update_view.h"
#include "adb_view/main_view.h"
#include "adb_view/auto_install_apk_list_table.h"

namespace views {
namespace examples {

AutoTabbedPane::AutoTabbedPane() 
  :CTPViewBase(L"自动模式"),
  ThreadMessageFilter(true) {
}

AutoTabbedPane::~AutoTabbedPane() {
}

void AutoTabbedPane::CreateExampleView(View* container) {
  tabbed_pane_ = new TabbedPane();
  tabbed_pane_->set_use_native_win_control(true);
  tabbed_pane_->set_listener(this);
  add_ = new TextButton(this, ASCIIToUTF16("Add"));
  add_at_ = new TextButton(this, ASCIIToUTF16("Add At 1"));
  remove_at_ = new TextButton(this, ASCIIToUTF16("Remove At 1"));
  select_at_ = new TextButton(this, ASCIIToUTF16("Select At 1"));

  GridLayout* layout = new GridLayout(container);
  container->SetLayoutManager(layout);

  const int tabbed_pane_column = 0;
  ColumnSet* column_set = layout->AddColumnSet(tabbed_pane_column);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
                        1.0f, GridLayout::USE_PREF, 0, 0);
  layout->StartRow(1 /* expand */, tabbed_pane_column);
  layout->AddView(tabbed_pane_);

  // Create a few tabs with a button first.
  //AddButton("Tab 1");
  //AddButton("Tab 2");
  

  CommandLine const& command_line = *CommandLine::ForCurrentProcess();
  std::string const process_type = command_line.GetSwitchValueASCII(switches::kProcessType);
  AddOneTab(AUTO_INSTALL_TAB, L"自动安装");
  //if (process_type == switches::kProcessTypeTest) {
  //  AddOneTab(TEST_TAB, "Test");
  //} else if (process_type == switches::kProcessTypeMQ) {
  //  //AddOneTab(MQ_CONNECT_TAB, "Connect");
  //  AddOneTab(MAIN_TAB, "Main");
  //  AddOneTab(MQ_TAB, "MQ");
  //} else if (process_type == switches::kProcessTypeMQTU) {
  //  AddOneTab(MAIN_TAB, "Main");
  //  AddOneTab(MQ_TAB, "MQ");
  //  AddOneTab(MS_TAB, "MS");
  //} else if (process_type == switches::kProcessTypeTU) {
  //  AddOneTab(MAIN_TAB, "Main");
  //  AddOneTab(MS_TAB, "MS");
  //} else if (process_type == switches::kProcessTypeBacktesting) {
  //  AddOneTab(MAIN_TAB, "Main");
  //  AddOneTab(MS_TAB, "MS");
  //}

}

void AutoTabbedPane::ButtonPressed(Button* sender, const ui::Event& event) {
  if (sender == add_) {
    //AddButton("Added");
  } else if (sender == add_at_) {
    const string16 label = ASCIIToUTF16("Added at 1");
    //tabbed_pane_->AddTabAtIndex(1, label, new TextButton(NULL, label), true);
  } else if (sender == remove_at_) {
    //if (tabbed_pane_->GetTabCount() > 1)
    //  delete tabbed_pane_->RemoveTabAtIndex(1);
  } else if (sender == select_at_) {
    if (tabbed_pane_->GetTabCount() > 1)
      tabbed_pane_->SelectTabAt(1);
  }

}

void AutoTabbedPane::TabSelectedAt(int index) {
  if (tabs_.find(index) != tabs_.end()) {
    tabs_[index]->Selected();
  } else {
    DCHECK(false);
  }
  // Just print the status when selection changes.

}



void AutoTabbedPane::AddOneTab(TabTypeEnum const type, const string16& label) {
  if (type == MQ_TAB) {

    
  } else if (type == MAIN_TAB) {

  } else if (type == REFRESH_TAB) {

  }  else if (type == TEST_TAB) {
    //TestView* p = new TestView(this ,label);

    //tabs_[tabbed_pane_->GetTabCount()] = p;
    //tabbed_pane_->AddTab(ASCIIToUTF16(label), p->example_view());
  } else if (type == MQ_CONNECT_TAB) {
    //MQGuardView* p = new MQGuardView();
    //tabs_[tabbed_pane_->GetTabCount()] = p;
    //tabbed_pane_->AddTab(ASCIIToUTF16(label), p->example_view());
  } else if (type == PACKAGE_LIST_TAB) {

  } else if (type == INSTALL_APK_LIST_TAB) {

  } else if (type == UPDATE_TAB) {

  } else if (type == AUTO_INSTALL_TAB) {
    AutoInstallApkListTable* p = new AutoInstallApkListTable(this, true);

    tabs_[tabbed_pane_->GetTabCount()] = p;
    tabbed_pane_->AddTab((label), p->example_view());
  } else {
  }

}


void AutoTabbedPane::AddTUTab(PointerWrapper<phone_module::TradeUnitInfo> const & p_info) {

  //TUTabbedPane* p = new TUTabbedPane(this, *(p_info.get()));
  //if (bc_2_tuview_.find(p_info.get()->bc) == bc_2_tuview_.end()) {
  //  bc_2_tuview_[p_info.get()->bc] = p;
  //} else {
  //  CHECK(false);
  //}

  //bc_2_id_[p_info.get()->bc] = p_info.get()->id;
  //id_2_tuview_[p_info.get()->id] = p;

  //tabs_[tabbed_pane_->GetTabCount()] = p;
  //tabbed_pane_->AddTab(ASCIIToUTF16(p_info.get()->bc), p->example_view());
}

void AutoTabbedPane::AddBacktestingTab(TabTypeEnum const type, const std::string& label, uint32 const id) {

}

void AutoTabbedPane::OnTradeUnitCreated(PointerWrapper<phone_module::TradeUnitInfo> const & p) {
  AddTUTab(p);
}

void AutoTabbedPane::OnBacktestingCreated(std::string const& bc, int32 const id) {
  AddBacktestingTab(BT_TAB, bc, id);
  //AddOneTab(BT_TAB, bc);
}

bool AutoTabbedPane::OnMessageReceived(IPC::Message const & msg) {


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
    IPC_BEGIN_MESSAGE_MAP_EX(AutoTabbedPane, msg, msg_is_ok)
      
      IPC_MESSAGE_HANDLER(CTP_TradeUnitCreatedNew, OnTradeUnitCreated)

      IPC_MESSAGE_HANDLER(CTP_BacktestingCreated, OnBacktestingCreated)

      IPC_MESSAGE_HANDLER(L2U_AdbInfo, OnAdbInfo)
      

      //IPC_MESSAGE_UNHANDLED_ERROR()
      IPC_END_MESSAGE_MAP_EX()

      if (msg_is_ok) {
        return true;
      }
  }

  //路由消息，发往tu===
  auto it = id_2_tuview_.find(msg.routing_id());
  if (it != id_2_tuview_.end()) {
    bool re = it->second->OnMessageReceived(msg);
    DCHECK_RLOG(re);
    return re;
  } /*else {
    DCHECK_RLOG(false);
  }*/
  // Dispatch incoming messages to the appropriate RenderView/WidgetHost.
  //RenderWidgetHost* rwh = render_widget_hosts_.Lookup(msg.routing_id());

  return false; 

}


void AutoTabbedPane::OnOrder(std::string const & ba, PointerWrapper<phone_module::OrderFootprints::OneRecord> const & p) {
  if (bc_2_tuview_.find(ba) != bc_2_tuview_.end()) {
    bc_2_tuview_[ba]->OnOrder(p);
  } else {
    DCHECK(false);
  }
}


void AutoTabbedPane::OnAdbInfo(std::string const & id, std::wstring const & info) {
  RefreshView * p = (RefreshView*)tabs_[2];
	p->UpdateAdbInfo(info);
	//L"恢复出厂设置"
}

void AutoTabbedPane::OnUpdateDevicesList(PointerWrapper< phone_module::DevicesList> const & p) {
  std::wstring device(L"当前设备ID: ");
  phone_module::DevicesList & list = *p.get();
  for (auto it = list.begin(); it != list.end(); ++it) {
    device.append(it->serial_no).append(L", ");
  }
  this->PrintStatus(L"%ls", device.c_str());
}


void AutoTabbedPane::Observe(int type,
  const content::NotificationSource& source,
  const content::NotificationDetails& details) {
    
}

}  // namespace examples
}  // namespace views
