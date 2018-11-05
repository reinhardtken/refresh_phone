// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#pragma once

#include "../../phone_common/include/ctp_define.h"

namespace phone_module {

enum {
  PATH_START = 6000,
   
  LOCAL_STATE = PATH_START,
  DB_DIR,
  DB_FILE,
  //LOG_DIR,
  DB_DAILY_FILE,

  PATH_END
};

// Call once to register the provider for the path keys defined above.
void RegisterPathProvider();

}  // namespace content

