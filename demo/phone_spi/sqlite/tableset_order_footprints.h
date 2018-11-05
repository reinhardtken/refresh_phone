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
//#include "../order_execute/order_execute_statemachine_base.h"
#include "persistent_store_base_ex.h"

class FilePath;
class Task;



namespace db_sqlite {

  struct OderFootprintsOperations : public Operation {
    OderFootprintsOperations(std::shared_ptr<phone_module::OrderFootprints::OneRecord> r)
      :Operation(OrderFootPrintsAddOneRecord),
      record(r) {}

    std::shared_ptr<phone_module::OrderFootprints::OneRecord> record;
    
  };


  class TableSetOderFootprints : public TableSet{
  public:
    TableSetOderFootprints(std::vector<std::string> const & params)
      :params_(params) {}
    virtual bool CreateTables(sql::Connection* db) OVERRIDE;
    virtual bool CreateStatements(sql::Connection* db) OVERRIDE; 
    virtual void ReleaseAllStatements() OVERRIDE;
    virtual void ExecuteOperation(Operation * p) OVERRIDE;

  private:
    std::vector<std::string> params_;
  };


}


