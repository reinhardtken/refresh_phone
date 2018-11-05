// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#pragma once


#include <string>

#include "base/basictypes.h"
#include "ui/views/controls/button/text_button.h"


namespace views {
class View;

namespace examples {

class CTPViewBase {
 public:
  virtual ~CTPViewBase();
  virtual void Selected() {}
  // Sub-classes should creates and add the views to the given parent.
  virtual void CreateExampleView(View* parent) = 0;

  const std::string& example_title() const { return example_title_; }
  View* example_view() { return container_; }

 protected:
  explicit CTPViewBase(const char* title);

  // Prints a message in the status area, at the bottom of the window.
  void PrintStatus(const char* format, ...);

  // Converts an boolean value to "on" or "off".
  const char* BoolToOnOff(bool value) {
    return value ? "on" : "off";
  }

 private:
  // Name of the example - used as title in the combobox list.
  std::string example_title_;

  // The view that contains the views example.
  View* container_;

  DISALLOW_COPY_AND_ASSIGN(CTPViewBase);
};


// [8/23/2013 ken]
//Combobox 在xp下，下拉框无法显示，懒得查原因了。
class BigTextButton : public TextButton {
public:
  BigTextButton(ButtonListener* listener, const string16& text)
    :TextButton(listener, text) {}

  virtual ~ BigTextButton() {}
  virtual gfx::Size GetPreferredSize() OVERRIDE;
};




}  // namespace examples
}  // namespace views


