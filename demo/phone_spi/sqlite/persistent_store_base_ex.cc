// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "persistent_store_base_ex.h"

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

#include "content/browser/notification_service_impl.h"



#include "../../phone_common/common/common_thread.h"
#include "../../phone_common/include/ctp_notification_types.h"
//#include "persistent_store_file.h"
//#include "persistent_store_memory.h"

namespace db_sqlite {


/*
bool TableSet::CreateTables(sql::Connection* db) {
  std::string table_name;
  if (!db->DoesTableExist(table_name.c_str())) {
    std::string create_table_statements = base::StringPrintf("CREATE TABLE %s ("
      "utc INTEGER NOT NULL UNIQUE PRIMARY KEY,"
      "utc_string LONGVARCHAR NOT NULL,"
      "open_price DOUBLE NOT NULL,"
      "high_price DOUBLE NOT NULL,"
      "low_price DOUBLE NOT NULL,"
      "close_price DOUBLE NOT NULL,"
      "volumn INTEGER NOT NULL,"
      "counter INTEGER NOT NULL"
      " )", table_name.c_str());


    if (!db->Execute(create_table_statements.c_str()))
      return false;

    std::string create_index = base::StringPrintf("CREATE INDEX %s_idx"
      " ON %s(utc)", table_name.c_str(), table_name.c_str());

    if (!db->Execute(create_index.c_str()))
      return false;
    }

    return true; 
}*/

/*
bool TableSet::CreateStatements(sql::Connection* db) {
  std::string s;
  std::string table_name;
  s.append("INSERT INTO ").append(table_name).append(" (utc, utc_string, price) VALUES (?,?,?)");
    statement_map_[table_name] = std::shared_ptr<sql::Statement>(new sql::Statement(db->GetCachedStatement(SQL_FROM_HERE,
    s.c_str())));

    return true;
}*/


void TableSet::ReleaseAllStatements() {
  statement_map_.clear();
}
  
/*
void TableSet::ExecuteOperation(Operation * p) {

}
*/



//===============================================================================

uint32 SQLitePersistentCTPStoreBackendEx::OperationType2Key(uint32 const type) {
  //每个table族拥有200个type值，应该够用了
  //0~199 属于一个族，这个族的key是0
  uint32 key = type / Operation::kTableSetKeyRange;
  return key;
}

static const int kCurrentVersionNumber = 5;
static const int kCompatibleVersionNumber = 5;


bool SQLitePersistentCTPStoreBackendEx::InitializeDatabase() {
  DCHECK(ThreadCheck());

  if (initialized_) {
    // Return false if we were previously initialized but the DB has since been
    // closed.
    return db_ != NULL;
  }

  base::Time start = base::Time::Now();

  DCHECK(PathCheck());



  db_.reset(new sql::Connection);
  //?????
  //db_->set_error_delegate(new KillDatabaseErrorDelegate(this));

  if (!db_->Open(path_)) {
    NOTREACHED() << "Unable to open DB.";
    db_.reset();
    return false;
  }

  bool ok = true;
  for (TableMap::iterator it = table_map_.begin(); it != table_map_.end(); ++it) {
    if (!it->second->CreateTables(db_.get())) {
      ok = false;
      break;
    }
    if (!it->second->CreateStatements(db_.get())) {
      ok = false;
      break;
    } 
  }

  if (!ok) {
    NOTREACHED() << "Unable to cookie DB.";
    db_.reset();
    return false;
  }





  initialized_ = true;
  return true;
}



void SQLitePersistentCTPStoreBackendEx::AddOperation(Operation * op) {
  BatchOperation(op);
}


void SQLitePersistentCTPStoreBackendEx::Commit() {
  DCHECK(CommonThread::CurrentlyOn(CommonThread::DB));

  OperationsList ops;
  {
    base::AutoLock locked(lock_);
    pending_.swap(ops);
    num_pending_ = 0;
  }

  // Maybe an old timer fired or we are already Close()'ed.
  if (!db_.get() || ops.empty())
    return;


  sql::Transaction transaction(db_.get());
  if (!transaction.Begin())
    return;

  for (OperationsList::iterator it = ops.begin(); it != ops.end(); ++it) {
    TableMap::iterator it_handler = table_map_.find(OperationType2Key((*it)->type));
    if (it_handler != table_map_.end()) {
      //DoStatement(*it);
      it_handler->second->ExecuteOperation(*it);
    } else {
      DCHECK_RLOG(false);
    }
    
  }
  bool succeeded = transaction.Commit();
}




void SQLitePersistentCTPStoreBackendEx::Flush(
    const base::Closure& callback) {
  DCHECK(!CommonThread::CurrentlyOn(CommonThread::DB));
  CommonThread::PostTask(
      CommonThread::DB, FROM_HERE, base::Bind(&SQLitePersistentCTPStoreBackendEx::Commit, this));
  if (!callback.is_null()) {
    // We want the completion task to run immediately after Commit() returns.
    // Posting it from here means there is less chance of another task getting
    // onto the message queue first, than if we posted it from Commit() itself.
    CommonThread::PostTask(CommonThread::DB, FROM_HERE, callback);
  }
}

void SQLitePersistentCTPStoreBackendEx::AddTable(uint32 const k, std::shared_ptr<TableSet> t) {
  DCHECK(table_map_.find(k) == table_map_.end());
  table_map_[k] = t;
}


}
