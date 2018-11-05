// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "../include/ctp_define.h"

#include "chrome/common/chrome_notification_types.h"


#define BEGIN_NOTIFICATION_TYPE_MACRO wchar_t* notification_type_string[] = {
#define HEAD_NOTIFICATION_TYPE_MACRO 
#define ITEM_NOTIFICATION_TYPE_MACRO(X, Y) L#Y,
#define TAIL_NOTIFICATION_TYPE_MACRO 
#define END_NOTIFICATION_TYPE_MACOR };

namespace phone_module {
#include "ctp_notification_types_internal.h"


  wchar_t const  PHONE_COMMON_EXPORT * NotificationTypeString(uint32 const);
}





namespace phone_module {

  wchar_t const * NotificationTypeString(uint32 const type) {


    if (type - chrome::NOTIFICATION_CHROME_END - 1 < sizeof(notification_type_string)) {
      return notification_type_string[type - chrome::NOTIFICATION_CHROME_END - 1];
    } else {
      return L"Unknown";
    }
  }

}


