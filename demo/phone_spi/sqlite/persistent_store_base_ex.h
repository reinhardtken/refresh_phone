// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// A sqlite implementation of a cookie monster persistent store.

#pragma once

#include <string>
#include <vector>
#include <map>
#include <utility>
#include <memory>

#include "base/callback_forward.h"
#include "base/compiler_specific.h"
#include "base/memory/ref_counted.h"

#include "sql/error_delegate_util.h"
#include "sql/meta_table.h"
#include "sql/statement.h"
#include "sql/transaction.h"
#include "third_party/sqlite/sqlite3.h"


#include "../phone_module/common.h"

class FilePath;
class Task;

// namespace phone_module {
//   //struct QuotationInfo;
//   struct CTPStoreInitParams;

// }


namespace db_sqlite {
  

struct Operation {
  enum {
    kTableSetKeyRange = 200
  };
  enum TypeEnum {
    //[start, end)
    OrderFootKey = 0,// x / kTableSetKeyRange == OrderFootKey都归那个tableset处理
    OrderFootPrintsStart = 0,
    OrderFootPrintsAddOneRecord,
    OrderFootPrintsEnd = 1*kTableSetKeyRange,
  };

  Operation(TypeEnum const t)
    :type(t) {}



  uint32 type;
};



typedef std::map<std::string, std::shared_ptr<sql::Statement>> StatementMap;




class TableSet {
public:
  virtual ~TableSet() {}
  virtual bool CreateTables(sql::Connection* db) = 0;
  virtual bool CreateStatements(sql::Connection* db) = 0; 
  virtual void ReleaseAllStatements() = 0;
  virtual void ExecuteOperation(Operation * p) = 0;

protected:
  StatementMap statement_map_;

};



class SQLitePersistentCTPStoreBackendEx
    : public base::RefCountedThreadSafe<SQLitePersistentCTPStoreBackendEx> {
 public:
  SQLitePersistentCTPStoreBackendEx(const FilePath& path)
      : path_(path),
        db_(NULL),
        num_pending_(0),
        initialized_(false) {
  }

  // Creates or loads the SQLite database.
  virtual void Load() = 0;
  virtual void Close() = 0;

  // Commit our pending operations to the database.
  void Commit();

  void AddTable(uint32 const, std::shared_ptr<TableSet>);
  

  // Close() executed on the background thread.
  //void InternalBackgroundClose();
 
  void AddOperation(Operation * op);


  // Commit pending operations as soon as possible.
  void Flush(const base::Closure& callback);





  protected:
  friend class base::RefCountedThreadSafe<SQLitePersistentCTPStoreBackendEx>;

  
  //std::vector<std::string> type_vec_;
  //std::vector<std::string> level_vec_;
  

  //typedef std::pair<uint32, uint32> Range;
  typedef std::map<uint32, std::shared_ptr<TableSet>> TableMap;
  TableMap table_map_;


  // You should call Close() before destructing this object.
  ~SQLitePersistentCTPStoreBackendEx() {
    DCHECK(!db_.get()) << "Close should have already been called.";
    DCHECK(num_pending_ == 0 && pending_.empty());
  }

  // Database upgrade statements.
  //bool EnsureDatabaseVersion();



  
  //virtual void Notify(LevelQuotationInfoBig const &) = 0;
  virtual void BatchOperation(Operation * op) = 0;
  //virtual void InitTables() = 0;
  virtual bool ThreadCheck() = 0;
  virtual bool PathCheck() = 0;
  

  protected:

  // Initialize the data base.
  bool InitializeDatabase();


  

  FilePath path_;
  //CTPStoreInitParams params_;
  

  





  void DoStatement(Operation * p);

  scoped_ptr<sql::Connection> db_;
  sql::MetaTable meta_table_;

  typedef std::list<Operation*> OperationsList;
  OperationsList pending_;
  OperationsList::size_type num_pending_;

  // Guard |cookies_|, |pending_|, |num_pending_|, |force_keep_session_state_|
  base::Lock lock_;

  // Temporary buffer for cookies loaded from DB. Accumulates cookies to reduce
  // the number of messages sent to the IO thread. Sent back in response to
  // individual load requests for domain keys or when all loading completes.
  //std::vector<QuotationInfo*> cookies_;

  // Map of domain keys(eTLD+1) to domains/hosts that are to be loaded from DB.


  // Map of (domain keys(eTLD+1), is secure cookie) to number of cookies in the
  // database.


  // Indicates if DB has been initialized.
  bool initialized_;



  
  // The time when |num_priority_waiting_| incremented to 1.
  base::Time current_priority_wait_start_;
  // The cumulative duration of time when |num_priority_waiting_| was greater
  // than 1.
  base::TimeDelta priority_wait_duration_;

  //memory db use this to dump info to disk
  std::vector<std::string> table_name_vec_;


  static uint32 OperationType2Key(uint32 const type);

  DISALLOW_COPY_AND_ASSIGN(SQLitePersistentCTPStoreBackendEx);
};


}


