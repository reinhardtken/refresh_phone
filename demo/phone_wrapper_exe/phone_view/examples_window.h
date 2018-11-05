// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#pragma once


//#include "ui/views/examples/views_examples_export.h"
#include "base/command_line.h"

namespace views {
namespace examples {

enum Operation {
  DO_NOTHING_ON_CLOSE = 0,
  QUIT_ON_CLOSE,
};

// Shows a window with the views examples in it.
//VIEWS_EXAMPLES_EXPORT 
void ShowExamplesWindow(Operation operation, CommandLine const & c);
CommandLine const & GetCommandLine();
}  // namespace examples
}  // namespace views


