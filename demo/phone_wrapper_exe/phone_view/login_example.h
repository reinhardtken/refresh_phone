// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_VIEWS_EXAMPLES_TEXTFIELD_EXAMPLE_H_
#define UI_VIEWS_EXAMPLES_TEXTFIELD_EXAMPLE_H_

#include <string>

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "base/string16.h"
#include "ui/views/controls/button/text_button.h"
#include "ui/views/controls/textfield/textfield_controller.h"
#include "example_base.h"

//#include "log_daemon.h"
#include "../../phone_common/common/common_thread.h"
//#include "profiler_data.h"
#include "../../phone_common/include/ctp_define.h"
#include "../../phone_common/common/common_main.h"
#include "../../phone_common/common/thread_message_filter.h"
#include "../../phone_common/include/ctp_messages.h"

namespace views {
namespace examples {

// LoginExample mimics login screen.
class LoginExample : public CTPViewBase,
                         public TextfieldController,
                         public ButtonListener {
 public:
  LoginExample();
  virtual ~LoginExample();

  // ExampleBase:
  virtual void CreateExampleView(View* container) OVERRIDE;

 private:
  // TextfieldController:
  virtual void ContentsChanged(Textfield* sender,
                               const string16& new_contents) OVERRIDE;
  virtual bool HandleKeyEvent(Textfield* sender,
                              const ui::KeyEvent& key_event) OVERRIDE;

  // ButtonListener:
  virtual void ButtonPressed(Button* sender, const ui::Event& event) OVERRIDE;

  // Textfields for name and password.
  Textfield* name_;
  Textfield* password_;

  // Various buttons to control textfield.
  TextButton* show_password_;
  TextButton* clear_all_;
  TextButton* append_;
  TextButton* set_;
  TextButton* set_style_;

  DISALLOW_COPY_AND_ASSIGN(LoginExample);
};

}  // namespace examples
}  // namespace views

#endif  // UI_VIEWS_EXAMPLES_TEXTFIELD_EXAMPLE_H_
