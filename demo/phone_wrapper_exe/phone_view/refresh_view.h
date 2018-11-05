// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#pragma once


#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "base/memory/scoped_ptr.h"
#include "ui/views/controls/button/label_button.h"
#include "ui/views/controls/button/text_button.h"
#include "ui/views/controls/progress_bar.h"

#include "example_base.h"
#include "ui/views/controls/textfield/textfield.h"
#include "../../phone_common/channel/channel_host.h"
#include "../../phone_common/channel/codec/codec_util.h"

namespace views {

class ImageButton;

namespace examples {

  class CTPTabbedPane;

// RefreshView simply counts the number of clicks.
class RefreshView : public CTPViewBase,
                          public ButtonListener {
                          /*public channel::ChannelHost::Delegate {*/
 public:
  RefreshView(CTPTabbedPane *p, string16 const &);
  virtual ~RefreshView();

  // Overridden from ExampleBase:
  virtual void CreateExampleView(View* container) OVERRIDE;

  virtual void Dispatch(codec::MessagePtr const &, base::Time const&);

  void UpdateAdbInfo(std::wstring const & info);

 private:
  

  // Overridden from ButtonListener:
  virtual void ButtonPressed(Button* sender, const ui::Event& event) OVERRIDE;

  // Example buttons.
  TextButton* play_sound_login_;
  TextButton* play_sound_logout_;
  TextButton* play_sound_order_;
  TextButton* play_sound_withdraw_;
  TextButton* play_sound_clinch_;
  Label * python_label_;
  ProgressBar * python_bar_;

  TextButton* reconnect_;
  TextButton* refresh_;
  Label * status_label_;


  Textfield* password_in_;
  TextButton* calc_password_;
  Textfield* password_out_;  
  //Textfield * field_;
  //LabelButton* label_button_;
  //ImageButton* image_button_;

  // Values used to modify the look and feel of the button.
  //TextButton::TextAlignment alignment_;
  //bool use_native_theme_border_;
  //const gfx::ImageSkia* icon_;

  

  // The number of times the buttons are pressed.
  //int count_;


  scoped_ptr<channel::ChannelHost> channel_host_;

  DISALLOW_COPY_AND_ASSIGN(RefreshView);
};

}  // namespace examples
}  // namespace views


