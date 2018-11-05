// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#pragma once


#include <string>

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "ui/views/controls/button/text_button.h"
#include "ui/views/controls/tabbed_pane/tabbed_pane_listener.h"
#include "ui/views/controls/label.h"
#include "example_base.h"
#include "content/public/browser/notification_registrar.h"
#include "content/public/browser/notification_observer.h"
#include "content/browser/notification_service_impl.h"
#include "ui/views/controls/progress_bar.h"
#include "ui/views/controls/button/text_button.h"

#include "../../phone_common/include/ctp_define.h"
#include "../../phone_common/common/common_main.h"
#include "../../phone_common/common/thread_message_filter.h"
#include "../../phone_common/include/ctp_messages.h"

#include "../../phone_spi/phone_module/common.h"


namespace views {
class TabbedPane;

namespace examples {

// A TabbedPane example tests adding/removing/selecting tabs.
class MQTabbedPane : public CTPViewBase,
                          public ButtonListener,
                          public ThreadMessageFilter,
                          public content::NotificationObserver,
                          public TabbedPaneListener {
 public:
  MQTabbedPane();
  virtual ~MQTabbedPane();

  // CTPViewBase:
  virtual void CreateExampleView(View* container) OVERRIDE;

  virtual void Observe(int type,
    const content::NotificationSource& source,
    const content::NotificationDetails& details) OVERRIDE;

  virtual bool OnMessageReceived(IPC::Message const & message) OVERRIDE;



  void AddOneTab(const std::string& label);

 private:
   
  // ButtonListener:
  virtual void ButtonPressed(Button* sender, const ui::Event& event) OVERRIDE;

  // TabbedPaneListener:
  virtual void TabSelectedAt(int index) OVERRIDE;

  // Print the status of the tab in the status area.
  void OnMQ(int32 const type, int32 const id, base::Time const & time, PointerWrapper<phone_module::LevelQuotationInfoBig> const & p);
  void OnOHLC_MQ(std::string const & type, PointerWrapper<phone_module::OHLCT> const & p);
  

  void OnLoginSuccess();
  void OnLoginFailed(uint32 const id, std::string const & msg);
  void OnDisConnect();
  void OnInitParam(std::string const & s);

  // The tabbed pane to be tested.
  TabbedPane* tabbed_pane_;
  ProgressBar * connect_bar_;
  TextButton* connect_button_;
  Label * login_status_;
  // Control buttons to add, remove or select tabs.

  //因为我觉得combox的实现好像有bug。当然也可能是我不会用，我需要记录tabindex和table对象的关系，然后在切换的时候通知combox模型改变了=
  std::map<int, CTPViewBase*> tabs_;


  friend class MQTable;
  std::map<std::string, MQTable*> type_2_mqview_;

  DISALLOW_COPY_AND_ASSIGN(MQTabbedPane);
};

}  // namespace examples
}  // namespace views


