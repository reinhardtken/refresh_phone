// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "tableset_order_footprints.h"

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

namespace {
  char * const kSperatorTable = "sperator";
}

namespace db_sqlite {


  bool TableSetOderFootprints::CreateTables(sql::Connection* db) {
    for (std::vector<std::string>::iterator it = params_.begin(); it != params_.end(); ++it) {
      std::string table_name = "order_footprints$";
      table_name.append(*it);
      if (!db->DoesTableExist(table_name.c_str())) {
        std::string create_table_statements = base::StringPrintf("CREATE TABLE %s ("
          //"utc INTEGER NOT NULL UNIQUE PRIMARY KEY,"
          "utc INTEGER NOT NULL ,"
          "utc_string LONGVARCHAR NOT NULL,"
          "operation INTEGER NOT NULL,"
          "price DOUBLE NOT NULL DEFAULT 0,"
          "volumn INTEGER NOT NULL DEFAULT 0,"
          "oc INTEGER NOT NULL DEFAULT 0,"
          "bs INTEGER NOT NULL DEFAULT 0,"
          "order_ref LONGVARCHAR NOT NULL DEFAULT '',"
          "status INTEGER NOT NULL DEFAULT 0,"
          "submit_status INTEGER NOT NULL DEFAULT 0,"
          "msg LONGVARCHAR NOT NULL DEFAULT '' "
          " )", table_name.c_str());


        if (!db->Execute(create_table_statements.c_str()))
          return false;

        std::string create_index = base::StringPrintf("CREATE INDEX %s_idx"
          " ON %s(utc)", table_name.c_str(), table_name.c_str());

        if (!db->Execute(create_index.c_str()))
          return false;
      }
    }
    

    return true; 
  }

  bool TableSetOderFootprints::CreateStatements(sql::Connection* db) {
    int32 k = 0;
    for (std::vector<std::string>::iterator it = params_.begin(); it != params_.end(); ++it) {
      k += 100;
      std::string table_name = "order_footprints$";
      table_name.append(*it);
      std::string s;
      s.append("INSERT INTO ").append(table_name).append(" (utc, utc_string, operation, price, volumn, oc, bs, order_ref, status, submit_status, msg) "
        "VALUES (?,?,?,?,?,?,?,?,?,?,?)");
      statement_map_[table_name] = std::shared_ptr<sql::Statement>(new sql::Statement(db->GetCachedStatement(sql::StatementID(__FILE__, __LINE__ + k),
        s.c_str())));

      std::string ss;
      ss.append("INSERT INTO ").append(table_name).append(" (utc, utc_string, operation) "
        "VALUES (?,?,?)");
      statement_map_[table_name + "_" + kSperatorTable] = std::shared_ptr<sql::Statement>(new sql::Statement(db->GetCachedStatement(sql::StatementID(__FILE__, __LINE__ + k + 20),
        ss.c_str())));
    }

    return true;
  }


  void TableSetOderFootprints::ReleaseAllStatements() {
    statement_map_.clear();
  }

  void TableSetOderFootprints::ExecuteOperation(Operation * p) {
    switch(p->type) {
      case Operation::OrderFootPrintsAddOneRecord: {
        OderFootprintsOperations * real = reinterpret_cast<OderFootprintsOperations*>(p);

        std::string type;
        base::Time utc;
        std::string utc_string;
        int32 operation = real->record->type;
        std::string order_ref;
        int32 status(5678);
        int32 submit_status(5678);
        std::string msg;
        double price;
        int32 volumn(0);
        int32 oc(5678);
        int32 bs(5678);
        switch(real->record->type) {
          case phone_module::OrderFootprints::OneRecord::OrderRequest: {
              type = real->record->order_request.InstrumentID;
              price = real->record->order_request.LimitPrice;
              volumn = real->record->order_request.VolumeTotalOriginal;
              oc = real->record->order_request.CombOffsetFlag[0];
              bs = real->record->order_request.Direction;
              order_ref = real->record->order_request.OrderRef;
              utc = real->record->time;
              utc_string = real->record->time_string;
              break;
          }
          case phone_module::OrderFootprints::OneRecord::OrderResponse: {
            type = real->record->order_response.InstrumentID;
            price = real->record->order_response.LimitPrice;
            volumn = real->record->order_response.VolumeTotalOriginal;
            oc = real->record->order_response.CombOffsetFlag[0];
            bs = real->record->order_response.Direction;
            order_ref = real->record->order_response.OrderRef;
            utc = real->record->time;
            utc_string = real->record->time_string;
            status = real->record->order_response.OrderStatus;
            submit_status = real->record->order_response.OrderSubmitStatus;
            msg = real->record->order_response.StatusMsg;
            break;
          }
          case phone_module::OrderFootprints::OneRecord::OrderResponseError: {
            type = real->record->order_request.InstrumentID;
            price = real->record->order_request.LimitPrice;
            volumn = real->record->order_request.VolumeTotalOriginal;
            oc = real->record->order_request.CombOffsetFlag[0];
            bs = real->record->order_request.Direction;
            order_ref = real->record->order_request.OrderRef;
            utc = real->record->time;
            utc_string = real->record->time_string;

            status = real->record->common_filed.ErrorID;
            msg = real->record->common_filed.ErrorMsg;
            break;
          }
          case phone_module::OrderFootprints::OneRecord::ActionRequest: {
            type = real->record->action_request.InstrumentID;
            price = real->record->action_request.LimitPrice;
            order_ref = real->record->action_request.OrderRef;
            utc = real->record->time;
            utc_string = real->record->time_string;

            break;
          }
          case phone_module::OrderFootprints::OneRecord::ActionResponseError: {
            type = real->record->action_request.InstrumentID;
            price = real->record->action_request.LimitPrice;
            order_ref = real->record->action_request.OrderRef;
            utc = real->record->time;
            utc_string = real->record->time_string;

            status = real->record->common_filed.ErrorID;
            msg = real->record->common_filed.ErrorMsg;
            break;
          }
          case phone_module::OrderFootprints::OneRecord::TradeResponse: {
            type = real->record->trade_response.InstrumentID;
            price = real->record->trade_response.Price;
            volumn = real->record->trade_response.Volume;
            oc = real->record->trade_response.OffsetFlag;
            bs = real->record->trade_response.Direction;
            order_ref = real->record->trade_response.OrderRef;
            utc = real->record->time;
            utc_string = real->record->time_string;
            
            //status = real->record->trade_response.OrderStatus;
            //submit_status = real->record->trade_response.OrderSubmitStatus;
            //msg = real->record->trade_response.StatusMsg;
            break;
          }
          case phone_module::OrderFootprints::OneRecord::OrderSperator: {
            std::string table_name = "order_footprints$";
            type = table_name + real->record->type_pinzhong + "_" + kSperatorTable;
            break;
          }
          default: {
            DCHECK_RLOG(false);
            return;
          }
        }

        if (type.size() && type.find(kSperatorTable) == std::string::npos) {
          std::string table_name = "order_footprints$";
          table_name.append(type);
          if (statement_map_.find(table_name) != statement_map_.end()) {
            std::shared_ptr<sql::Statement> add_smt = statement_map_[table_name];

            // (utc, utc_string, operation, price, volumn, oc, bs, order_ref, status, submit_status, msg)
            add_smt->Reset(true);
            add_smt->BindInt64(0, utc.ToInternalValue());
            add_smt->BindString(1, utc_string);
            add_smt->BindInt(2, operation);
            add_smt->BindDouble(3, price);
            add_smt->BindInt(4, volumn);
            add_smt->BindInt(5, oc);
            add_smt->BindInt(6, bs);
            add_smt->BindString(7, order_ref);
            add_smt->BindInt(8, status);
            add_smt->BindInt(9, submit_status);
            add_smt->BindString(10, msg);
            if (!add_smt->Run()) {
              DCHECK_RLOG(false);
            }
          } else {
            DCHECK_RLOG(false);
          }
        } else if (type.size()) {
          if (statement_map_.find(type) != statement_map_.end()) {
            std::shared_ptr<sql::Statement> add_smt = statement_map_[type];

            // (utc, utc_string, operation, price, volumn, oc, bs, order_ref, status, submit_status, msg)
            add_smt->Reset(true);
            add_smt->BindInt64(0, utc.ToInternalValue());
            add_smt->BindString(1, utc_string);
            add_smt->BindInt(2, operation);


            if (!add_smt->Run()) {
              DCHECK_RLOG(false);
            }
          }

        } else {
          DCHECK_RLOG(false);
        }
        break;
        }
      default: {
        DCHECK_RLOG(false);
      }
    }
  }


}
