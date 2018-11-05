// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "phone_common/system_monitor/removable_device_constants.h"

namespace chrome {

const char kFSUniqueIdPrefix[] = "UUID:";
const char kVendorModelSerialPrefix[] = "VendorModelSerial:";

#if defined(OS_LINUX)
const char kLeftParen[] = "(";
const char kRightParen[] = ")";
const char kVendorModelVolumeStoragePrefix[] = "VendorModelVolumeStorage:";
#endif

const char kNonSpaceDelim[] = ":";
const char kSpaceDelim[] = " ";

}  // namespace chrome
