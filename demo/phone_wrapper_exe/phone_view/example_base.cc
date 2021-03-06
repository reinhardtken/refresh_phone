// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "example_base.h"

#include <stdarg.h>

#include "base/compiler_specific.h"
#include "base/stringprintf.h"
#include "ui/views/view.h"

namespace views {
namespace examples {

// Logs the specified string to the status area of the examples window.
// This function can only be called if there is a visible examples window.
void LogStatus(const std::wstring& status);

namespace {

// TODO(oshima): Check if this special container is still necessary.
class ContainerView : public View {
 public:
  explicit ContainerView(CTPViewBase* base)
      : example_view_created_(false),
        example_base_(base) {
  }

 private:
  // Overridden from View:
  virtual void ViewHierarchyChanged(bool is_add,
                                    View* parent,
                                    View* child) OVERRIDE {
    View::ViewHierarchyChanged(is_add, parent, child);
    // We're not using child == this because a Widget may not be
    // available when this is added to the hierarchy.
    if (is_add && GetWidget() && !example_view_created_) {
      example_view_created_ = true;
      example_base_->CreateExampleView(this);
    }
  }

  // True if the example view has already been created, or false otherwise.
  bool example_view_created_;

  CTPViewBase* example_base_;

  DISALLOW_COPY_AND_ASSIGN(ContainerView);
};

}  // namespace

CTPViewBase::~CTPViewBase() {}

CTPViewBase::CTPViewBase(const wchar_t* title) : example_title_(title) {
  container_ = new ContainerView(this);
}

// Prints a message in the status area, at the bottom of the window.
void CTPViewBase::PrintStatus(const wchar_t* format, ...) {
  va_list ap;
  va_start(ap, format);
  std::wstring msg;
  base::StringAppendV(&msg, format, ap);
  LogStatus(msg);
}

}  // namespace examples
}  // namespace views
