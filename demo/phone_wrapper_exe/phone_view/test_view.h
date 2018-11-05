// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#pragma once


#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "base/memory/scoped_ptr.h"
#include "ui/views/controls/button/label_button.h"
#include "ui/views/controls/button/text_button.h"
#include "example_base.h"
#include "ui/views/controls/textfield/textfield.h"
#include "../../phone_common/channel/channel_host.h"
#include "../../phone_common/channel/codec/codec_util.h"

namespace views {

class ImageButton;

namespace examples {

  class CTPTabbedPane;

// TestView simply counts the number of clicks.
class TestView : public CTPViewBase,
                          public ButtonListener {
                          /*public channel::ChannelHost::Delegate {*/
 public:
  TestView(CTPTabbedPane *p, std::string const &);
  virtual ~TestView();

  // Overridden from ExampleBase:
  virtual void CreateExampleView(View* container) OVERRIDE;

  void Dispatch(std::string const &name, codec::MessagePtr const &, base::Time const&);

 private:
  void TextButtonPressed(const ui::Event& event);
  void LabelButtonPressed(const ui::Event& event);

  // Overridden from ButtonListener:
  virtual void ButtonPressed(Button* sender, const ui::Event& event) OVERRIDE;

  // Example buttons.
  TextButton* connect_button_;
  TextButton* send_button_;
  Label * label_;
  Textfield * field_;
  //LabelButton* label_button_;
  //ImageButton* image_button_;

  // Values used to modify the look and feel of the button.
  TextButton::TextAlignment alignment_;
  bool use_native_theme_border_;
  const gfx::ImageSkia* icon_;



  // The number of times the buttons are pressed.
  int count_;


  scoped_ptr<channel::ChannelHost> channel_host_;

  DISALLOW_COPY_AND_ASSIGN(TestView);
};

}  // namespace examples
}  // namespace views


