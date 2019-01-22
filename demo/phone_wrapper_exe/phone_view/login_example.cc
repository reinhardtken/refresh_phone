// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "login_example.h"

#include "base/utf_string_conversions.h"
#include "ui/base/events/event.h"
#include "ui/base/range/range.h"
#include "ui/gfx/render_text.h"
#include "ui/views/controls/label.h"
#include "ui/views/controls/textfield/textfield.h"
#include "ui/views/layout/grid_layout.h"
#include "ui/views/view.h"

#include "../../phone_common/include/ctp_notification_types.h"

namespace views {
namespace examples {

LoginExample::LoginExample() : CTPViewBase(L"Textfield") {
  registrar_.Add(this, phone_module::NOTIFICATION_PHONE_LOGIN_FAILED_STATUS,
    content::NotificationService::AllSources());
}

LoginExample::~LoginExample() {
}


void LoginExample::Observe(int type,
  const content::NotificationSource& source,
  const content::NotificationDetails& details) {
  if (type == phone_module::NOTIFICATION_PHONE_LOGIN_FAILED_STATUS) {
    status_->SetText(content::Details<std::wstring const>(details)->c_str());
  } 

}


void LoginExample::CreateExampleView(View* container) {
  name_ = new Textfield();
  password_ = new Textfield();//Textfield::STYLE_OBSCURED
  password_->set_placeholder_text(L"验证码");
  login_ = new TextButton(this, L"登录");
  login_->set_alignment(TextButton::ALIGN_CENTER);
  verify_code_ = new TextButton(this, L"获取验证码");
  verify_code_->set_alignment(TextButton::ALIGN_CENTER);
  status_ = new Label;
  //clear_all_ = new TextButton(this, ASCIIToUTF16("Clear All"));
  //append_ = new TextButton(this, ASCIIToUTF16("Append"));
  //set_ = new TextButton(this, ASCIIToUTF16("Set"));
  //set_style_ = new TextButton(this, ASCIIToUTF16("Set Styles"));
  name_->SetController(this);
  password_->SetController(this);

  GridLayout* layout = new GridLayout(container);
  container->SetLayoutManager(layout);

  ColumnSet* column_set = layout->AddColumnSet(0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
                        0.3f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
                        0.7f, GridLayout::USE_PREF, 0, 0);

  ColumnSet* column_set2 = layout->AddColumnSet(1);
  column_set2->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);

  layout->StartRow(0, 0);
  layout->AddView(new Label(L"手机号"));
  layout->AddView(name_);
  layout->StartRow(0, 0);
  layout->AddView(verify_code_);
  layout->AddView(password_);

  layout->StartRow(0, 1);
  layout->AddView(login_);

  layout->StartRow(0, 1);
  layout->AddView(status_);

  //layout->StartRow(0, 0);
  //layout->AddView(clear_all_);
  //layout->StartRow(0, 0);
  //layout->AddView(append_);
  //layout->StartRow(0, 0);
  //layout->AddView(set_);
  //layout->StartRow(0, 0);
  //layout->AddView(set_style_);
}

void LoginExample::ContentsChanged(Textfield* sender,
                                       const string16& new_contents) {
  if (sender == name_) {
    //PrintStatus("Name [%s]", UTF16ToUTF8(new_contents).c_str());
  } else if (sender == password_) {
    //PrintStatus("Password [%s]", UTF16ToUTF8(new_contents).c_str());
  }
}

bool LoginExample::HandleKeyEvent(Textfield* sender,
                                      const ui::KeyEvent& key_event) {
  return false;
}

void LoginExample::ButtonPressed(Button* sender, const ui::Event& event) {
  if (sender == login_) {
    std::wstring name = name_->text();
    std::wstring password = password_->text();
    ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::CTP, new U2L_Login(name, password));
  } else if (sender == verify_code_) {
    std::wstring name = name_->text();
    ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::CTP, new U2L_VerfiyCode(name));
  } else if (sender == append_) {
    name_->AppendText(ASCIIToUTF16("[append]"));
  } else if (sender == set_) {
    name_->SetText(ASCIIToUTF16("[set]"));
  } else if (sender == set_style_) {
    if (!name_->text().empty()) {
      gfx::StyleRange color;
      color.foreground = SK_ColorYELLOW;
      color.range = ui::Range(0, name_->text().length());
      name_->ApplyStyleRange(color);

      if (name_->text().length() >= 5) {
        size_t fifth = name_->text().length() / 5;
        gfx::StyleRange underline;
        underline.underline = true;
        underline.foreground = SK_ColorBLUE;
        underline.range = ui::Range(1 * fifth, 4 * fifth);
        name_->ApplyStyleRange(underline);

        gfx::StyleRange strike;
        strike.strike = true;
        strike.foreground = SK_ColorRED;
        strike.range = ui::Range(2 * fifth, 3 * fifth);
        name_->ApplyStyleRange(strike);
      }
    }
  }
}

}  // namespace examples
}  // namespace views
