// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "tu_commission_table.h"

#include <vector>

#include "base/string_util.h"
#include "base/string_tokenizer.h"
#include "base/string_number_conversions.h"
#include "base/sys_string_conversions.h"
#include "base/stringprintf.h"
#include "third_party/skia/include/core/SkBitmap.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "ui/gfx/image/image_skia.h"
#include "ui/views/controls/button/checkbox.h"
#include "ui/views/layout/grid_layout.h"
#include "ui/views/controls/textfield/textfield.h"

#include "../../../phone_common/include/ctp_notification_types.h"
#include "../tabbed_pane_example.h"

namespace {

}


namespace views {
namespace examples {


CommissionRateTable::CommissionRateTable(CTPTabbedPane *p, std::string const &bc) 
  :CTPViewBase("Table"),
  pane_(p),
  bc_(bc),
  table_(NULL),
  ThreadMessageFilter(false) {


    /*if (pane_->HasID(bc_)) {
      ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::CTP,
        new CTP_TU_QryTypeList(pane_->GetID(bc_)));
    } else {
      DCHECK(false);
    }*/

}

CommissionRateTable::~CommissionRateTable() {
  // Delete the view before the model.
  delete table_;
  table_ = NULL;

}

void CommissionRateTable::OnSelectedIndexChanged(Combobox* combobox) {

}

void CommissionRateTable::CreateExampleView(View* container) {

  GridLayout* layout = new GridLayout(container);
  container->SetLayoutManager(layout);

  /*
  ///合约代码
  TThostFtdcInstrumentIDType	InstrumentID;
  ///投资者范围
  TThostFtdcInvestorRangeType	InvestorRange;
  ///经纪公司代码
  TThostFtdcBrokerIDType	BrokerID;
  ///投资者代码
  TThostFtdcInvestorIDType	InvestorID;
  ///开仓手续费率
  TThostFtdcRatioType	OpenRatioByMoney;
  ///开仓手续费
  TThostFtdcRatioType	OpenRatioByVolume;
  ///平仓手续费率
  TThostFtdcRatioType	CloseRatioByMoney;
  ///平仓手续费
  TThostFtdcRatioType	CloseRatioByVolume;
  ///平今手续费率
  TThostFtdcRatioType	CloseTodayRatioByMoney;
  ///平今手续费
  TThostFtdcRatioType	CloseTodayRatioByVolume;
  */
  //====================================
  std::vector<ui::TableColumn> columns2;
  columns2.push_back(ui::TableColumn(0, L"品种",
    ui::TableColumn::LEFT, 70));

  columns2.push_back(ui::TableColumn(1, L"投资者范围",
    ui::TableColumn::LEFT, 70));

  columns2.push_back(ui::TableColumn(2, L"经纪公司代码",
    ui::TableColumn::LEFT, 100));
  columns2.push_back(ui::TableColumn(3, L"投资者代码",
    ui::TableColumn::LEFT, 50));

  columns2.push_back(ui::TableColumn(4, L"开仓手续费率",
    ui::TableColumn::LEFT, 100));
  columns2.push_back(ui::TableColumn(5, L"开仓手续费",
    ui::TableColumn::LEFT, 100));
  columns2.push_back(ui::TableColumn(6, L"平仓手续费率",
    ui::TableColumn::LEFT, 100));
  columns2.push_back(ui::TableColumn(7, L"平仓手续费",
    ui::TableColumn::LEFT, 100));
  columns2.push_back(ui::TableColumn(8, L"平今手续费率",
    ui::TableColumn::LEFT, 100));
  columns2.push_back(ui::TableColumn(9, L"平今手续费",
    ui::TableColumn::LEFT, 100));
  

  table_ = new TableView(this, columns2, TEXT_ONLY, true, true, true);
  table_->SetObserver(this);



  ColumnSet* column_set = layout->AddColumnSet(0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL, 1,
                        GridLayout::USE_PREF, 0, 0);
  layout->StartRow(0.7f /* expand */, 0);
  layout->AddView(table_->CreateParentIfNecessary());

  
}

int CommissionRateTable::RowCount() {
  return list_.size();
}

string16 CommissionRateTable::GetText(int row, int column_id) {
  if ((uint32)row >= list_.size()) {
    return string16();
  }
  switch (column_id) {
      case 0: {
        return ASCIIToUTF16(list_[row]->InstrumentID);
        break;
      }
      case 1: {
        return ASCIIToUTF16("");
        break;
      }
      case 2: {
        return ASCIIToUTF16(list_[row]->BrokerID);
        break;
      }
      case 3: {
        return ASCIIToUTF16(list_[row]->InvestorID);
        break;
      }
      case 4: {
        return ASCIIToUTF16(base::DoubleToString(list_[row]->OpenRatioByMoney));
        break;
      }
      case 5: {
        return ASCIIToUTF16(base::DoubleToString(list_[row]->OpenRatioByVolume));
        break;
      }
      case 6: {
        return ASCIIToUTF16(base::DoubleToString(list_[row]->CloseRatioByMoney));
        break;
      }
      case 7: {
        return ASCIIToUTF16(base::DoubleToString(list_[row]->CloseRatioByVolume));
        break;
      }
      case 8: {
        return ASCIIToUTF16(base::DoubleToString(list_[row]->CloseTodayRatioByMoney));
        break;
      }
      case 9: {
        return ASCIIToUTF16(base::DoubleToString(list_[row]->CloseTodayRatioByVolume));
        break;
      }
      /*case 8: {
        return ASCIIToUTF16(base::DoubleToString(info.sell_limit_price));
        break;
      }
      case 9: {
        return ASCIIToUTF16(base::DoubleToString(info.force_close_percent));
        break;
      }
      case 10: {
        return ASCIIToUTF16(base::DoubleToString(info.position_cash));
        break;
      }
      case 11: {
        return info.auto_close == true ? L"是" : L"否";
        break;
      }*/
      default: {
        DCHECK(false);
      }
  }
 

  return string16();
}



gfx::ImageSkia CommissionRateTable::GetIcon(int row) {
  DCHECK(false);
  return gfx::ImageSkia();
  //return row % 2 ? gfx::ImageSkia(icon1_) : gfx::ImageSkia(icon2_);
}

void CommissionRateTable::SetObserver(ui::TableModelObserver* observer) {}

void CommissionRateTable::OnSelectionChanged() {
  /*if (table_->FirstSelectedRow() >= 0) {
    PrintStatus("Selected: %s",
                UTF16ToASCII(GetText(table_->FirstSelectedRow(), 0)).c_str());
  }*/
}

void CommissionRateTable::OnDoubleClick() {
  /*if (table_->FirstSelectedRow() >= 0) {
    PrintStatus("Double Click: %s",
                UTF16ToASCII(GetText(table_->FirstSelectedRow(), 0)).c_str());
  }*/
}

void CommissionRateTable::OnMiddleClick() {}

void CommissionRateTable::OnKeyDown(ui::KeyboardCode virtual_keycode) {}

void CommissionRateTable::OnTableViewDelete(TableView* table_view) {}

void CommissionRateTable::OnTableView2Delete(TableView2* table_view) {}

void CommissionRateTable::ButtonPressed(Button* sender, const ui::Event& event) {
  
}



void CommissionRateTable::Observe(int type,
  const content::NotificationSource& source,
  const content::NotificationDetails& details) {

}

bool CommissionRateTable::OnMessageReceived(IPC::Message const & msg) {

  if (msg.routing_id() == MSG_ROUTING_CONTROL) {
    // Dispatch control messages.
    bool msg_is_ok = false;
    

      if (msg_is_ok) {
        return true;
      }
  }

  // Dispatch incoming messages to the appropriate RenderView/WidgetHost.
  //RenderWidgetHost* rwh = render_widget_hosts_.Lookup(msg.routing_id());

  return false; 

}


//TaskManagerTableModel
void CommissionRateTable::EnsureVisible() {

}



void CommissionRateTable::Selected() {
  
}

void CommissionRateTable::OnCommissionRate(PointerWrapper<CThostFtdcInstrumentCommissionRateField> const & p) {
  
  uint32 const old_size = map_.size();
  std::shared_ptr<CThostFtdcInstrumentCommissionRateField> sp(new CThostFtdcInstrumentCommissionRateField(*p.get()));
  map_[p.get()->InstrumentID] = sp;
  uint32 const new_size = map_.size();
  if (old_size != new_size) {
    list_.push_back(sp);
  }
  

  if (old_size == new_size) {
    table_->OnItemsChanged(0, map_.size());
  } else {
    table_->OnModelChanged();
  }
}

}  // namespace examples
}  // namespace views
