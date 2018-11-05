// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#pragma once


#include <string>

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "ui/views/controls/button/text_button.h"
#include "ui/views/controls/button/checkbox.h"
#include "ui/views/controls/label.h"
#include "ui/views/controls/tabbed_pane/tabbed_pane_listener.h"
#include "../example_base.h"
#include "content/public/browser/notification_registrar.h"
#include "content/public/browser/notification_observer.h"
#include "content/browser/notification_service_impl.h"
#include "ui/views/controls/progress_bar.h"
#include "ui/views/controls/button/text_button.h"

#include "../../../phone_common/include/ctp_define.h"
#include "../../../phone_common/common/common_main.h"
#include "../../../phone_common/common/thread_message_filter.h"
#include "../../../phone_common/include/ctp_messages.h"

#include "../../../phone_spi/phone_module/common.h"


namespace views {
class TabbedPane;

namespace examples {
  class CTPTabbedPane;
  class OrderResultTable;
  class TUTable;
  class TUControlTable;
  class MarginRateTable;
  class CommissionRateTable;
// A TabbedPane example tests adding/removing/selecting tabs.
class TUTabbedPane : public CTPViewBase,
                          public ButtonListener,
                          public ThreadMessageFilter,
                          public content::NotificationObserver,
                          public TabbedPaneListener {
 public:
  TUTabbedPane(CTPTabbedPane * p, phone_module::TradeUnitInfo const &);
  virtual ~TUTabbedPane();

  // CTPViewBase:
  virtual void CreateExampleView(View* container) OVERRIDE;

  virtual void Observe(int type,
    const content::NotificationSource& source,
    const content::NotificationDetails& details) OVERRIDE;

  virtual bool OnMessageReceived(IPC::Message const & message) OVERRIDE;



  void AddOneTab(const std::string& label);

  void OnPosition(PointerWrapper<phone_module::PositionData> const & p);
  void OnOrder(PointerWrapper<phone_module::OrderFootprints::OneRecord> const & p);
  void OnCash(PointerWrapper<CThostFtdcTradingAccountField> const & p);
  //void OnOrderResult(PointerWrapper<order_execute::OrderUnit2UI> const & p);
  void OnTypeList(std::vector<std::string> const &v);
  void OnStopPrice(PointerWrapper<phone_module::StopPriceData> const & p);

 private:
   
  // ButtonListener:
  virtual void ButtonPressed(Button* sender, const ui::Event& event) OVERRIDE;

  // TabbedPaneListener:
  virtual void TabSelectedAt(int index) OVERRIDE;

  // Print the status of the tab in the status area.
  void OnMQ(int32 const type, int32 const id, base::Time const & time, PointerWrapper<phone_module::LevelQuotationInfoBig> const & p);

  void OnLoginSuccess(std::string const & ba);
  void OnLoginFailed(std::string const & ba, uint32 const id, std::string const & msg);
  void OnLogoutSuccess(std::string const & ba);
  void OnDisConnect(std::string const & ba);
  void OnMarginRate(PointerWrapper<CThostFtdcInstrumentMarginRateField> const & p);
  void OnCommissionRate(PointerWrapper<CThostFtdcInstrumentCommissionRateField> const & p);
  // The tabbed pane to be tested.
  
  TabbedPane* tabbed_pane_;
  //ProgressBar * connect_bar_;
  //TextButton* connect_button_;
  // Control buttons to add, remove or select tabs.
  ProgressBar * connect_bar_;
  //TextButton* connect_button_;
  //TextButton* disconnect_button_;
  //Label * login_status_;
  //Checkbox* allow_execute_ms_button_;
  //因为我觉得combox的实现好像有bug。当然也可能是我不会用，我需要记录tabindex和table对象的关系，然后在切换的时候通知combox模型改变了=
  std::map<int, CTPViewBase*> tabs_;
  CTPTabbedPane * p_;
  //std::string const bc_;
  phone_module::TradeUnitInfo const info_;
  //friend class MQTable;
  //std::map<std::string, MQTable*> type_2_mqview_;
  OrderResultTable * p_order_result_;
  TUTable* p_trade_unit_;
  TUControlTable* p_control_;
  MarginRateTable * p_marginrate_;
  CommissionRateTable *p_commissionrate_;

  DISALLOW_COPY_AND_ASSIGN(TUTabbedPane);
};

}  // namespace examples
}  // namespace views


