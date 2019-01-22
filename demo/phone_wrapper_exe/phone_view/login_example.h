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
#include "ui/views/controls/label.h"
#include "example_base.h"

//#include "log_daemon.h"
#include "../../phone_common/common/common_thread.h"
//#include "profiler_data.h"
#include "../../phone_common/include/ctp_define.h"
#include "../../phone_common/common/common_main.h"
#include "../../phone_common/common/thread_message_filter.h"
#include "../../phone_common/include/ctp_messages.h"


#include "content/public/browser/notification_registrar.h"
#include "content/public/browser/notification_observer.h"
#include "content/browser/notification_service_impl.h"

namespace views {
namespace examples {

// LoginExample mimics login screen.
class LoginExample : public CTPViewBase,
                         public TextfieldController,
                         public content::NotificationObserver,
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

  virtual void Observe(int type,
    const content::NotificationSource& source,
    const content::NotificationDetails& details) OVERRIDE;

  // Textfields for name and password.
  Textfield* name_;
  Textfield* password_;

  // Various buttons to control textfield.
  TextButton* login_;
  TextButton* verify_code_;
  Label * status_;
  TextButton* append_;
  TextButton* set_;
  TextButton* set_style_;
  content::NotificationRegistrar registrar_;
  DISALLOW_COPY_AND_ASSIGN(LoginExample);
};

}  // namespace examples
}  // namespace views

#endif  // UI_VIEWS_EXAMPLES_TEXTFIELD_EXAMPLE_H_
