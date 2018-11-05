// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// A sqlite implementation of a cookie monster persistent store.

#pragma once

#include <string>
#include <vector>

#include "base/callback_forward.h"
#include "base/compiler_specific.h"
#include "base/memory/ref_counted.h"

#include "persistent_store_base_ex.h"

class FilePath;
class Task;


namespace db_sqlite {
  class FileStoreEx : public SQLitePersistentCTPStoreBackendEx {
  public:
    FileStoreEx(const FilePath& path)
      : SQLitePersistentCTPStoreBackendEx(path) {
    }

    void CompleteLoadOnIOThread(const base::Closure& loaded_callback, bool load_success);
    void LoadAndNotifyOnDBThread(const base::Closure& loaded_callback);

    virtual void Load() OVERRIDE;
    virtual void Close() OVERRIDE;
  private:

    virtual void BatchOperation(Operation * op) OVERRIDE;
    //virtual void InitTables() = 0;
    virtual bool ThreadCheck() OVERRIDE;
    virtual bool PathCheck() OVERRIDE;


    void InternalBackgroundClose();


    DISALLOW_COPY_AND_ASSIGN(FileStoreEx);
  };



}


