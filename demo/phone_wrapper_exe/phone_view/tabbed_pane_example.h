// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#pragma once


#include <string>

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "ui/views/controls/button/text_button.h"
#include "ui/views/controls/tabbed_pane/tabbed_pane_listener.h"
#include "example_base.h"
#include "content/public/browser/notification_registrar.h"
#include "content/public/browser/notification_observer.h"
#include "content/browser/notification_service_impl.h"


#include "../../phone_common/include/ctp_define.h"
#include "../../phone_common/common/common_main.h"
#include "../../phone_common/common/thread_message_filter.h"
#include "../../phone_common/include/ctp_messages.h"

#include "../../phone_spi/phone_module/common.h"


namespace views {
class TabbedPane;

namespace examples {
  class TUTabbedPane;
  class MSTable;
// A TabbedPane example tests adding/removing/selecting tabs.
class CTPTabbedPane : public CTPViewBase,
                          public ButtonListener,
                          public ThreadMessageFilter,
                          public content::NotificationObserver,
                          public TabbedPaneListener {
 public:
  CTPTabbedPane();
  virtual ~CTPTabbedPane();

  // CTPViewBase:
  virtual void CreateExampleView(View* container) OVERRIDE;

  virtual void Observe(int type,
    const content::NotificationSource& source,
    const content::NotificationDetails& details) OVERRIDE;

  virtual bool OnMessageReceived(IPC::Message const & message) OVERRIDE;



  enum TabTypeEnum {
    MQ_TAB,
    MS_TAB,
    MAIN_TAB,
    TU_TAB,
    TEST_TAB,
    MQ_CONNECT_TAB,
    BT_TAB,

  REFRESH_TAB,
	PACKAGE_LIST_TAB,
	INSTALL_APK_LIST_TAB,
	UPDATE_TAB,
  };
  void AddOneTab(TabTypeEnum const type, const string16& label);
  void AddTUTab(PointerWrapper<phone_module::TradeUnitInfo> const & p);
  void AddBacktestingTab(TabTypeEnum const type, const std::string& label, uint32 const id);

  bool HasID(std::string const &ba) {
    return bc_2_id_.find(ba) != bc_2_id_.end();
  }

  int32 GetID(std::string const &ba) {
    auto it = bc_2_id_.find(ba);
    if (it != bc_2_id_.end()) {
      return it->second;
    } else {
      DCHECK_RLOG(false);
      return -1;
    }
  }

 private:
   void OnTradeUnitCreated(PointerWrapper<phone_module::TradeUnitInfo> const & p);
   void OnBacktestingCreated(std::string const& bc, int32 const id);
  // ButtonListener:
  virtual void ButtonPressed(Button* sender, const ui::Event& event) OVERRIDE;

  // TabbedPaneListener:
  virtual void TabSelectedAt(int index) OVERRIDE;




  void OnOrder(std::string const & ba, PointerWrapper<phone_module::OrderFootprints::OneRecord> const & p);
  //void OnOrderResult(std::string const & ba, PointerWrapper<order_execute::OrderUnit2UI> const & p);
  
  //void OnCash(std::string const & ba, PointerWrapper<CThostFtdcTradingAccountField> const & p);

  void OnPosition(std::string const & ba, PointerWrapper<phone_module::PositionData> const & p);
  void OnTypeList(std::string const & ba, std::vector<std::string> const &v);
  void OnStopPrice(std::string const & ba, PointerWrapper<phone_module::StopPriceData> const & p);

  void OnAdbInfo(std::string const & id, std::wstring const & info);

  void OnUpdateDevicesList(PointerWrapper<phone_module::DevicesList> const & p);
  

  // The tabbed pane to be tested.
  TabbedPane* tabbed_pane_;

  // Control buttons to add, remove or select tabs.
  Button* add_;
  Button* add_at_;
  Button* remove_at_;
  Button* select_at_;


  //因为我觉得combox的实现好像有bug。当然也可能是我不会用，我需要记录tabindex和table对象的关系，然后在切换的时候通知combox模型改变了=
  std::map<int, CTPViewBase*> tabs_;


  //friend class TUTable;
  //friend class TUTabbedPane;
  std::map<std::string, int32> bc_2_id_;
  std::map<std::string, TUTabbedPane*> bc_2_tuview_;
  std::map<uint32, TUTabbedPane*> id_2_tuview_;
  MSTable* ms_table_;

  DISALLOW_COPY_AND_ASSIGN(CTPTabbedPane);
};

}  // namespace examples
}  // namespace views


