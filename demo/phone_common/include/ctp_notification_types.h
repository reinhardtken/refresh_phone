// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once
#include "../include/ctp_define.h"

#include "chrome/common/chrome_notification_types.h"


#define BEGIN_NOTIFICATION_TYPE_MACRO enum NotificationType {
#define HEAD_NOTIFICATION_TYPE_MACRO NOTIFICATION_CTP_START = chrome::NOTIFICATION_CHROME_END,
#define ITEM_NOTIFICATION_TYPE_MACRO(X, Y) X##Y,
#define TAIL_NOTIFICATION_TYPE_MACRO NOTIFICATION_CTP_END,
#define END_NOTIFICATION_TYPE_MACOR };

namespace phone_module {
#include "ctp_notification_types_internal.h"
}


namespace phone_module {



//extern PHONE_COMMON_EXPORT wchar_t* g_notification_type_string[];
// i do not know why below is wrong
//wchar_t const*  PHONE_COMMON_EXPORT NotificationTypeString(NotificationType const);
wchar_t const  PHONE_COMMON_EXPORT * NotificationTypeString(uint32 const);
}


