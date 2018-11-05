// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#pragma once

#include "../include/ctp_define.h"

namespace common {

enum {
  PATH_START = 4000,

  // Path of exe
  EXE_PATH = PATH_START,
  CTP_DATA,
  PATH_END
};

// Call once to register the provider for the path keys defined above.
PHONE_COMMON_EXPORT void RegisterPathProvider();

}  // namespace content

