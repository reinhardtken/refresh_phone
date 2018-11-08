// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "test_view.h"

#include <functional>

#include "base/utf_string_conversions.h"
#include "base/string_tokenizer.h"
#include "content/public/common/content_switches.h"
//#include "grit/ui_resources.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/gfx/image/image.h"
#include "ui/views/controls/button/image_button.h"
#include "ui/views/controls/button/text_button.h"
#include "ui/views/layout/box_layout.h"
#include "ui/views/view.h"
#include "tabbed_pane_example.h"
#include "examples_window.h"
#include "../../protobuf/out/cc_comunication.pb.h"
#include "../../protobuf/out/cp_comunication.pb.h"
#include "../../protobuf/out/guard_comunication.pb.h"


#include "../../phone_common/channel/codec/protobuf_codec.h"
#include "../../phone_common/channel/codec/tlv_codec.h"
#include "../../phone_common/common/common_common.h"


namespace {
const int kLayoutSpacing = 10;  // pixels
}  // namespace

namespace views {
namespace examples {

TestView::TestView(CTPTabbedPane *p, std::string const &)
    : CTPViewBase("Button"),
      connect_button_(NULL),
      send_button_(NULL),
      label_(NULL),
      field_(NULL),
      alignment_(TextButton::ALIGN_LEFT),
      use_native_theme_border_(false),
      icon_(NULL),
      count_(0) {
  //ui::ResourceBundle& rb = ui::ResourceBundle::GetSharedInstance();
  //icon_ = rb.GetImageNamed(IDR_CLOSE_SA_H).ToImageSkia();
  channel_host_.reset(new channel::ChannelHost());
  channel_host_->RegisterProtobufHandler(switches::kCommunicateTest, base::Bind(&TestView::Dispatch, base::Unretained(this)));
  channel_host_->StartKeepAlive(switches::kCommunicateTestAlive, prefs::kGuardServer);

  //=========
  channel_host_->RegisterConnectStatusHandler(switches::kCommunicateTestAlive, 
    [&](std::string const &name, bool connected) ->void {
      if (true == connected) {
        ////发送进程id=
        //ctp::guard::ReportPid* report= new ctp::guard::ReportPid;
        //report->set_process_id(GetCurrentProcessId());
        //codec::MessagePtr ptr(report);
        //if (channel_host_.get()) {
        //  channel_host_->SendProtobufMsg(switches::kCommunicateTestAlive, ptr);
        //}
      }
    }
  );

}

TestView::~TestView() {
}

void TestView::CreateExampleView(View* container) {
  container->SetLayoutManager(
      new BoxLayout(BoxLayout::kVertical, 0, 0, kLayoutSpacing));

  connect_button_ = new TextButton(this, ASCIIToUTF16("Connect Button"));
  std::string const process_type = GetCommandLine().GetSwitchValueASCII(switches::kProcessType);
  if (process_type != switches::kProcessTypeTest) {
    connect_button_->SetEnabled(false);
  }
  container->AddChildView(connect_button_);

  send_button_ = new TextButton(this, ASCIIToUTF16("Send Button"));
  send_button_->set_focusable(true);
  //text_button_->SetHoverColor(SK_ColorRED);
  container->AddChildView(send_button_);

  label_ = new Label(L"echo...");
  //label_button_->set_focusable(true);
  //label_button_->SetTextColor(CustomButton::BS_HOT, SK_ColorRED);
  container->AddChildView(label_);

  field_ = new Textfield();

  container->AddChildView(field_);
}

void TestView::TextButtonPressed(const ui::Event& event) {
  
}

void TestView::LabelButtonPressed(const ui::Event& event) {
 
}

void TestView::ButtonPressed(Button* sender, const ui::Event& event) {
  if (sender == send_button_) {
    std::wstring s = field_->text();
    std::string ss = WideToASCII(s);

    StringTokenizer list(ss, ",");
    std::vector<std::string> param;
    while(list.GetNext()) {
      param.push_back(list.token());
    }

/*
    ctp::c2c::Command * c2 = new ctp::c2c::Command;
    c2->set_cmd("teststring");
    c2->set_params(ss);
    MessagePtr ptr(c2);
    if (channel_host_.get()) {
      channel_host_->Send(ptr);
    }*/

    /*ctp::cp::Command* cmd= new ctp::cp::Command;
    cmd->set_cmd("teststring");
    for (uint32 i =0; i < param.size(); i++) {
      cmd->add_params(param[i]);
    }
    codec::MessagePtr ptr(cmd);
    if (channel_host_.get()) {
      channel_host_->SendProtobufMsg(switches::kCommunicateTest, ptr);
    }
    
  } else if (sender == connect_button_) {
    if (channel_host_.get()) {
      channel_host_->InitializeClient(switches::kCommunicateTest, prefs::kMQTUServer);

    }*/

  }
}


void TestView::Dispatch(std::string const &name, codec::MessagePtr const & p, base::Time const&) {
  DCHECK(name == switches::kCommunicateTest);
/*
  ctp::c2c::Command const* p1 = static_cast<ctp::c2c::Command const *>(p.get());
  std::string s = base::StringPrintf("answer: %s, %s", p1->cmd().c_str(), p1->params().c_str());
  label_->SetText(ASCIIToWide(s));*/

  ctp::cp::Command const* p1 = static_cast<ctp::cp::Command const *>(p.get());
  std::string s = base::StringPrintf("answer: %s:", p1->cmd().c_str());
  for (int32 i =0; i < p1->params_size(); i++) {
    s.append(p1->params(i)).append(",");
  }
  label_->SetText(ASCIIToWide(s));

}


}  // namespace examples
}  // namespace views
