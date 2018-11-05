// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "paths.h"

#include "base/file_util.h"
#include "base/path_service.h"

namespace common {

bool PathProvider(int key, FilePath* result) {
   switch (key) {
     case CTP_DATA: {
       FilePath cur;
       if (!PathService::Get(base::DIR_EXE, &cur))
         return false;
       cur = cur.Append(FILE_PATH_LITERAL("ctp_data"));
       *result = cur;
       return true;
       break;
     }
//     case EXE_PATH:
//       return PathService::Get(base::FILE_EXE, result);
//     case DIR_TEST_DATA: {
//       FilePath cur;
//       if (!PathService::Get(base::DIR_SOURCE_ROOT, &cur))
//         return false;
//       cur = cur.Append(FILE_PATH_LITERAL("content"));
//       cur = cur.Append(FILE_PATH_LITERAL("test"));
//       cur = cur.Append(FILE_PATH_LITERAL("data"));
//       if (!file_util::PathExists(cur))  // we don't want to create this
//         return false;

//       *result = cur;
//       return true;
//       break;
//     }
//     case DIR_MEDIA_LIBS: {
// #if defined(OS_MACOSX)
//       *result = base::mac::FrameworkBundlePath();
//       *result = result->Append("Libraries");
//       return true;
// #else
//       return PathService::Get(base::DIR_MODULE, result);
// #endif
//     }
//     case DIR_LAYOUT_TESTS: {
//       FilePath cur;
//       if (!PathService::Get(DIR_TEST_DATA, &cur))
//         return false;
//       cur = cur.Append(FILE_PATH_LITERAL("layout_tests"));
//       cur = cur.Append(FILE_PATH_LITERAL("LayoutTests"));
//       if (file_util::DirectoryExists(cur)) {
//         *result = cur;
//         return true;
//       }
//       if (!PathService::Get(base::DIR_SOURCE_ROOT, &cur))
//         return false;
//       cur = cur.Append(FILE_PATH_LITERAL("third_party"));
//       cur = cur.Append(FILE_PATH_LITERAL("WebKit"));
//       cur = cur.Append(FILE_PATH_LITERAL("LayoutTests"));
//       *result = cur;
//       return true;
//     }
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
