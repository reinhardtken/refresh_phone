// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "tabbed_pane_example.h"

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

namespace views {
namespace examples {

CTPTabbedPane::CTPTabbedPane() 
  :CTPViewBase("Tabbed Pane"),
  ThreadMessageFilter(true) {
}

CTPTabbedPane::~CTPTabbedPane() {
}

void CTPTabbedPane::CreateExampleView(View* container) {
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
  AddOneTab(MAIN_TAB, L"设备列表");
  AddOneTab(UPDATE_TAB, L"包管理");//所有网络更新检查在这里
  AddOneTab(REFRESH_TAB, L"恢复出厂设置");
  AddOneTab(PACKAGE_LIST_TAB, L"已安装包名列表");
  AddOneTab(INSTALL_APK_LIST_TAB, L"包安装/删除");//所有adb装包，删包在这里
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

void CTPTabbedPane::ButtonPressed(Button* sender, const ui::Event& event) {
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
  PrintStatus();
}

void CTPTabbedPane::TabSelectedAt(int index) {
  if (tabs_.find(index) != tabs_.end()) {
    tabs_[index]->Selected();
  } else {
    DCHECK(false);
  }
  // Just print the status when selection changes.
  PrintStatus();
}

void CTPTabbedPane::PrintStatus() {
  CTPViewBase::PrintStatus("Tab Count:%d, Selected Tab:%d",
                           tabbed_pane_->GetTabCount(),
                           tabbed_pane_->GetSelectedTabIndex());
}

void CTPTabbedPane::AddOneTab(TabTypeEnum const type, const string16& label) {
  if (type == MQ_TAB) {
  //  MQTabbedPane* p = new MQTabbedPane;
  // 
  //  tabs_[tabbed_pane_->GetTabCount()] = p;
  //  tabbed_pane_->AddTab(ASCIIToUTF16(label), p->example_view());
  //} else if (type == MS_TAB) {
  //  MSTable* p = new MSTable;
  //  ms_table_ = p;
  //  tabs_[tabbed_pane_->GetTabCount()] = p;
  //  tabbed_pane_->AddTab(ASCIIToUTF16(label), p->example_view());
    
  } else if (type == MAIN_TAB) {
    MainView* p = new MainView(NULL, "");

    tabs_[tabbed_pane_->GetTabCount()] = p;
    tabbed_pane_->AddTab((label), p->example_view());
  } else if (type == REFRESH_TAB) {
    RefreshView* p = new RefreshView(this ,label);

    tabs_[tabbed_pane_->GetTabCount()] = p;
    tabbed_pane_->AddTab((label), p->example_view());
  }  else if (type == TEST_TAB) {
    //TestView* p = new TestView(this ,label);

    //tabs_[tabbed_pane_->GetTabCount()] = p;
    //tabbed_pane_->AddTab(ASCIIToUTF16(label), p->example_view());
  } else if (type == MQ_CONNECT_TAB) {
    //MQGuardView* p = new MQGuardView();
    //tabs_[tabbed_pane_->GetTabCount()] = p;
    //tabbed_pane_->AddTab(ASCIIToUTF16(label), p->example_view());
  } else if (type == PACKAGE_LIST_TAB) {
	  PackageListTable* p = new PackageListTable(this, label);

	  tabs_[tabbed_pane_->GetTabCount()] = p;
	  tabbed_pane_->AddTab((label), p->example_view());
  } else if (type == INSTALL_APK_LIST_TAB) {
	  InstallApkListTable* p = new InstallApkListTable(this, label);

	  tabs_[tabbed_pane_->GetTabCount()] = p;
	  tabbed_pane_->AddTab((label), p->example_view());
  } else if (type == UPDATE_TAB) {
	  UpdateView* p = new UpdateView();

	  tabs_[tabbed_pane_->GetTabCount()] = p;
	  tabbed_pane_->AddTab((label), p->example_view());
  } else {
  }

}


void CTPTabbedPane::AddTUTab(PointerWrapper<phone_module::TradeUnitInfo> const & p_info) {

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

void CTPTabbedPane::AddBacktestingTab(TabTypeEnum const type, const std::string& label, uint32 const id) {
  if (type == BT_TAB) {
    BacktestingView * p = new BacktestingView(this, label, id);
    tabs_[tabbed_pane_->GetTabCount()] = p;
    tabbed_pane_->AddTab(ASCIIToUTF16(label), p->example_view());
  } else {
    DCHECK_RLOG(false);
  }
}

void CTPTabbedPane::OnTradeUnitCreated(PointerWrapper<phone_module::TradeUnitInfo> const & p) {
  AddTUTab(p);
}

void CTPTabbedPane::OnBacktestingCreated(std::string const& bc, int32 const id) {
  AddBacktestingTab(BT_TAB, bc, id);
  //AddOneTab(BT_TAB, bc);
}

bool CTPTabbedPane::OnMessageReceived(IPC::Message const & msg) {


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
    IPC_BEGIN_MESSAGE_MAP_EX(CTPTabbedPane, msg, msg_is_ok)
      
      IPC_MESSAGE_HANDLER(CTP_TradeUnitCreatedNew, OnTradeUnitCreated)

      IPC_MESSAGE_HANDLER(CTP_Position2UI, OnPosition)
      IPC_MESSAGE_HANDLER(CTP_StopPrice2UI, OnStopPrice)
      IPC_MESSAGE_HANDLER(CTP_Order2UI, OnOrder)
      //IPC_MESSAGE_HANDLER(CTP_Cash2UI, OnCash)
      //IPC_MESSAGE_HANDLER(CTP_OrderResult2UI, OnOrderResult)
      IPC_MESSAGE_HANDLER(CTP_TU_TypeList, OnTypeList)
      IPC_MESSAGE_HANDLER(CTP_BacktestingCreated, OnBacktestingCreated)
      //IPC_MESSAGE_HANDLER(CTP_MQLevelParam, OnInitParam)
      //IPC_MESSAGE_HANDLER(CTP_CONSOLE_CommandProtoBuf, OnConsoleCommandProtoBuf)
      //IPC_MESSAGE_HANDLER(CTP_TradeUnitCreated, OnTradeUnitCreated)
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


void CTPTabbedPane::OnOrder(std::string const & ba, PointerWrapper<phone_module::OrderFootprints::OneRecord> const & p) {
  if (bc_2_tuview_.find(ba) != bc_2_tuview_.end()) {
    bc_2_tuview_[ba]->OnOrder(p);
  } else {
    DCHECK(false);
  }
}

//void CTPTabbedPane::OnOrderResult(std::string const & ba, PointerWrapper<order_execute::OrderUnit2UI> const & p) {
//  if (bc_2_tuview_.find(ba) != bc_2_tuview_.end()) {
//    bc_2_tuview_[ba]->OnOrderResult(p);
//  } else {
//    DCHECK(false);
//  }
//}

//void CTPTabbedPane::OnCash(std::string const & ba, PointerWrapper<CThostFtdcTradingAccountField> const & p) {
//  if (bc_2_tuview_.find(ba) != bc_2_tuview_.end()) {
//    bc_2_tuview_[ba]->OnCash(p);
//  } else {
//    DCHECK(false);
//  }
//}

void CTPTabbedPane::OnPosition(std::string const & ba, PointerWrapper<phone_module::PositionData> const & p) {
  if (bc_2_tuview_.find(ba) != bc_2_tuview_.end()) {
    bc_2_tuview_[ba]->OnPosition(p);
    ms_table_->OnPosition(ba, p);
  } else {
    DCHECK(false);
  }
}

void CTPTabbedPane::OnStopPrice(std::string const & ba, PointerWrapper<phone_module::StopPriceData> const & p) {
  if (bc_2_tuview_.find(ba) != bc_2_tuview_.end()) {
    bc_2_tuview_[ba]->OnStopPrice(p);
  } else {
    DCHECK(false);
  }
}

void CTPTabbedPane::OnTypeList(std::string const & ba, std::vector<std::string> const &v) {
  if (bc_2_tuview_.find(ba) != bc_2_tuview_.end()) {
    bc_2_tuview_[ba]->OnTypeList(v);
  } else {
    DCHECK(false);
  }
}

void CTPTabbedPane::OnAdbInfo(std::string const & id, std::wstring const & info) {
  RefreshView * p = (RefreshView*)tabs_[2];
	p->UpdateAdbInfo(info);
	//L"恢复出厂设置"
}


void CTPTabbedPane::Observe(int type,
  const content::NotificationSource& source,
  const content::NotificationDetails& details) {
    
}

}  // namespace examples
}  // namespace views
