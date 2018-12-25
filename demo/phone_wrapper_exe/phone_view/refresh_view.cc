// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "refresh_view.h"

#include "base/base64.h"
#include "base/utf_string_conversions.h"
#include "base/string_tokenizer.h"
#include "base/string_util.h"
//#include "grit/ui_resources.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/gfx/image/image.h"
#include "ui/views/controls/button/image_button.h"
#include "ui/views/controls/button/text_button.h"
#include "ui/views/layout/box_layout.h"
#include "ui/views/layout/grid_layout.h"
#include "ui/views/view.h"
#include "tabbed_pane_example.h"
#include "examples_window.h"
#include "../../protobuf/out/cc_comunication.pb.h"
#include "../../protobuf/out/cp_comunication.pb.h"

#include "../../phone_common/channel/codec/protobuf_codec.h"
#include "../../phone_common/channel/codec/tlv_codec.h"

namespace {
const int kLayoutSpacing = 10;  // pixels
}  // namespace

namespace views {
namespace examples {

RefreshView::RefreshView(CTPTabbedPane *p, string16 const &)
    : CTPViewBase(L"Button") {
  //ui::ResourceBundle& rb = ui::ResourceBundle::GetSharedInstance();
  //icon_ = rb.GetImageNamed(IDR_CLOSE_SA_H).ToImageSkia();
}

RefreshView::~RefreshView() {
}

void RefreshView::CreateExampleView(View* container) {
  GridLayout* layout = new GridLayout(container);
  container->SetLayoutManager(layout);


  ColumnSet* column_set;// = layout->AddColumnSet(0);
  //column_set->AddColumn(GridLayout::FILL, GridLayout::FILL, 0.3f,
  //  GridLayout::USE_PREF, 0, 0);
  //column_set->AddColumn(GridLayout::FILL, GridLayout::FILL, 0.7f,
  //  GridLayout::USE_PREF, 0, 0);

  uint32 index = 0;

  reconnect_ = new TextButton(this, L" 扫描设备");
  reconnect_->set_alignment(TextButton::ALIGN_CENTER);



  column_set = layout->AddColumnSet(index);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
	  1.0f, GridLayout::USE_PREF, 0, 0);

  layout->StartRow(0 /* no expand */, index);
  layout->AddView(reconnect_);
  //=====================================================
  index++;
  refresh_ = new TextButton(this, L" 一键刷机");
  refresh_->set_alignment(TextButton::ALIGN_CENTER);
  refresh_->SetEnabled(false);


  column_set = layout->AddColumnSet(index);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);

  layout->StartRow(0 /* no expand */, index);
  layout->AddView(refresh_);


  //=====================================================
  index++;

  Label * pad = new Label();
  column_set = layout->AddColumnSet(index);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);

  layout->StartRow(1, index);
  layout->AddView(pad);
  //=============================

  index++;
  status_label_ = new Label();

  //calc_password_ = new TextButton(this, L" 计算密码");
  //calc_password_->set_alignment(TextButton::ALIGN_CENTER);

  //password_out_ = new Textfield();

  column_set = layout->AddColumnSet(index);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);


  layout->StartRow(0 /* no expand */, index);
  layout->AddView(status_label_);
  //layout->AddView(calc_password_);
  //layout->AddView(password_out_);

  
}



void RefreshView::ButtonPressed(Button* sender, const ui::Event& event) {
  if (sender == play_sound_login_) {
    ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::CTP, new CTP_PlaySound(phone_module::PLAYSOUND_LOGIN));
  } else if (sender == play_sound_logout_) {
    ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::CTP, new CTP_PlaySound(phone_module::PLAYSOUND_LOGOUT));
  } else if (sender == play_sound_order_) {
    ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::CTP, new CTP_PlaySound(phone_module::PLAYSOUND_ORDER));
  } else if (sender == play_sound_withdraw_) {
    ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::CTP, new CTP_PlaySound(phone_module::PLAYSOUND_WITHDRAW));
  } else if (sender == play_sound_clinch_) {
    ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::CTP, new CTP_PlaySound(phone_module::PLAYSOUND_CLINCH));
  } else if (sender == reconnect_) {
    ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::CTP, new U2L_ScanDevices());
  } else if (sender == refresh_) {
	  //ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::CTP, new U2L_Refresh(std::string()));
  } else if (sender == calc_password_) {
    std::string in = WideToASCII(password_in_->text());
    if (in.size() > 0) {
      std::vector<char> out;
      std::vector<char> in_vec;
      in_vec.assign(in.begin(), in.end());
      char * p = "430102lqp3.14159535";
      common_util::Encode((unsigned char*)p, in_vec, out);
      std::string out_string;
      out_string.assign(out.begin(), out.end());
      std::string out_base64;
      CHECK(base::Base64Encode(out_string, &out_base64));
      password_out_->SetText(ASCIIToWide(out_base64));

      {
        std::string base64_decode;
        CHECK(base::Base64Decode(out_base64, &base64_decode));
        std::vector<char> in;
        std::vector<char> out;
        in.assign(base64_decode.begin(), base64_decode.end());
        common_util::Decode((unsigned char*)p, in, out);
        std::string out_string;
        out_string.assign(out.begin(), out.end());
        char const * result = out_string.c_str();
        std::string real_result = result;
      }
    }
  } else {
    DCHECK_RLOG(false);
  }
}

void RefreshView::UpdateAdbInfo(std::wstring const & info) {
	status_label_->SetText(info);
}


void RefreshView::Dispatch(codec::MessagePtr const & p, base::Time const&) {

}


}  // namespace examples
}  // namespace views
