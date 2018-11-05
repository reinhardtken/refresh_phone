// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "paths.h"

#include "base/file_util.h"
#include "base/stringprintf.h"
#include "base/path_service.h"

#include "../../phone_common/common/paths.h"

namespace phone_module {

bool PathProvider(int key, FilePath* result) {
  switch (key) {
    
    case LOCAL_STATE: {
      FilePath cur;
      if (!PathService::Get(common::CTP_DATA, &cur))
        return false;
      cur = cur.Append(FILE_PATH_LITERAL("local_state"));
      *result = cur;
      return true;
      break;
    }
    case DB_DIR: {
      FilePath cur;
      if (!PathService::Get(common::CTP_DATA, &cur))
        return false;
      cur = cur.Append(FILE_PATH_LITERAL("db"));
      *result = cur;
      return true;
      break;
    }
    case DB_FILE: {
      FilePath cur;
      if (!PathService::Get(DB_DIR, &cur))
        return false;
      cur = cur.Append(FILE_PATH_LITERAL("daily.db"));
      *result = cur;
      return true;
      break;
    }
    case DB_DAILY_FILE: {
      //文件设置为ctp_data/db/daily/2013-7-13.db 15-11-00
      FilePath cur;
      if (!PathService::Get(DB_DIR, &cur))
        return false;
      base::Time::Exploded e;
      base::Time time = base::Time::Now();
      time.LocalExplode(&e);
      std::wstring sub_dir = base::StringPrintf(L"%d-%d-%d", e.year, e.month, e.day_of_month);
      std::wstring name = base::StringPrintf(L"%d-%d-%d.db", e.hour, e.minute, e.second);
      cur = cur.Append(sub_dir);
      file_util::CreateDirectoryW(cur);
      cur = cur.Append(name);
      *result = cur;
      return true;
    }
/*
    case LOG_DIR: {
      FilePath cur;
      if (!PathService::Get(CTP_DATA, &cur))
        return false;
      cur = cur.Append(FILE_PATH_LITERAL("log"));
      *result = cur;
      return true;
      break;
    }*/
    default:
      return false;
  }

  return false;
}

// This cannot be done as a static initializer sadly since Visual Studio will
// eliminate this object file if there is no direct entry point into it.
void RegisterPathProvider() {
  PathService::RegisterProvider(PathProvider, PATH_START, PATH_END);
}

}  // namespace content
