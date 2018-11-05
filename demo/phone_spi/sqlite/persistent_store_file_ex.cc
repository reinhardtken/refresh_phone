// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "persistent_store_file_ex.h"

#include <list>
#include <map>
#include <set>
#include <utility>
#include <memory>

#include "base/basictypes.h"
#include "base/bind.h"
#include "base/callback.h"
#include "base/file_path.h"
#include "base/file_util.h"
#include "base/logging.h"
#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "base/string_number_conversions.h"
//#include "base/metrics/histogram.h"
#include "base/string_util.h"
#include "base/stringprintf.h"
#include "base/string_tokenizer.h"
#include "base/synchronization/lock.h"
#include "base/threading/thread.h"
#include "base/threading/thread_restrictions.h"
#include "base/time.h"
#include "sql/error_delegate_util.h"
#include "sql/meta_table.h"
#include "sql/statement.h"
#include "sql/transaction.h"
#include "third_party/sqlite/sqlite3.h"
#include "content/browser/notification_service_impl.h"


#include "../phone_module/common.h"
#include "../../phone_common/common/common_thread.h"
#include "../../phone_common/include/ctp_notification_types.h"

namespace db_sqlite {
  void FileStoreEx::Load() {
      // This function should be called only once per instance.
      DCHECK(!db_.get());
      CommonThread::PostTask(
        CommonThread::DB, FROM_HERE,
        base::Bind(&FileStoreEx::LoadAndNotifyOnDBThread, this, base::Closure()));
  }


  void FileStoreEx::LoadAndNotifyOnDBThread(
      const base::Closure& loaded_callback) {
    DCHECK(CommonThread::CurrentlyOn(CommonThread::DB));
    //IncrementTimeDelta increment(&cookie_load_duration_);

    if (!InitializeDatabase()) {
      CommonThread::PostTask(
        CommonThread::IO, FROM_HERE,
        base::Bind(&FileStoreEx::CompleteLoadOnIOThread,
                   this, loaded_callback, false));
    } else {
      //ChainLoadCookies(loaded_callback);
    }
  }

  void FileStoreEx::CompleteLoadOnIOThread(
      const base::Closure& loaded_callback, bool load_success) {

  }


  void FileStoreEx::Close() {
      //DCHECK(ThreadCheck());
      if (CommonThread::CurrentlyOn(CommonThread::DB)) {
      InternalBackgroundClose();
    } else {
      // Must close the backend on the background thread.
      CommonThread::PostTask(
          CommonThread::DB, FROM_HERE,
          base::Bind(&FileStoreEx::InternalBackgroundClose, this));
    }
  }


  void FileStoreEx::InternalBackgroundClose() {
    DCHECK(CommonThread::CurrentlyOn(CommonThread::DB));
    // Commit any pending operations
    Commit();
    //
    for (TableMap::iterator it = table_map_.begin(); it != table_map_.end(); ++it) {
      it->second->ReleaseAllStatements();
    }
    db_.reset();
  }

  bool FileStoreEx::ThreadCheck() {
    return CommonThread::CurrentlyOn(CommonThread::DB);
  }


  bool FileStoreEx::PathCheck() {
    const FilePath dir = path_.DirName();
    return file_util::PathExists(dir) || file_util::CreateDirectory(dir);
    
  }



  void FileStoreEx::BatchOperation(Operation * op) {
      // Commit every 30 seconds.
    static const int kCommitIntervalMs = 30 * 1000;
    //static const int kCommitIntervalMs = 5 * 1000;
    // Commit right away if we have more than 512 outstanding operations.
    static const size_t kCommitAfterBatchSize = 512;
    DCHECK(!CommonThread::CurrentlyOn(CommonThread::DB));

    // We do a full copy of the cookie here, and hopefully just here.
    scoped_ptr<Operation> po(op);

    OperationsList::size_type num_pending;
    {
      base::AutoLock locked(lock_);
      pending_.push_back(po.release());
      num_pending = ++num_pending_;
    }

    if (num_pending == 1) {
      // We've gotten our first entry for this batch, fire off the timer.
      CommonThread::PostDelayedTask(
          CommonThread::DB, FROM_HERE,
          base::Bind(&FileStoreEx::Commit, this),
          base::TimeDelta::FromMilliseconds(kCommitIntervalMs).InMilliseconds());
    } else if (num_pending == kCommitAfterBatchSize) {
      // We've reached a big enough batch, fire off a commit now.
      CommonThread::PostTask(
          CommonThread::DB, FROM_HERE,
          base::Bind(&FileStoreEx::Commit, this));
    }
  }




}
