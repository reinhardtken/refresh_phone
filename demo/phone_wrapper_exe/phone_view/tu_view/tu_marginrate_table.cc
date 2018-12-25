// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "tu_marginrate_table.h"

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


MarginRateTable::MarginRateTable(CTPTabbedPane *p, std::string const &bc) 
  :CTPViewBase(L"Table"),
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

MarginRateTable::~MarginRateTable() {
  // Delete the view before the model.
  delete table_;
  table_ = NULL;

}

void MarginRateTable::OnSelectedIndexChanged(Combobox* combobox) {

}

void MarginRateTable::CreateExampleView(View* container) {

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
  ///投机套保标志
  TThostFtdcHedgeFlagType	HedgeFlag;
  ///多头保证金率
  TThostFtdcRatioType	LongMarginRatioByMoney;
  ///多头保证金费
  TThostFtdcMoneyType	LongMarginRatioByVolume;
  ///空头保证金率
  TThostFtdcRatioType	ShortMarginRatioByMoney;
  ///空头保证金费
  TThostFtdcMoneyType	ShortMarginRatioByVolume;
  ///是否相对交易所收取
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

  columns2.push_back(ui::TableColumn(4, L"投机套保标志",
    ui::TableColumn::LEFT, 100));
  columns2.push_back(ui::TableColumn(5, L"多头保证金率",
    ui::TableColumn::LEFT, 100));
  columns2.push_back(ui::TableColumn(6, L"多头保证金费",
    ui::TableColumn::LEFT, 100));
  columns2.push_back(ui::TableColumn(7, L"空头保证金率",
    ui::TableColumn::LEFT, 100));
  columns2.push_back(ui::TableColumn(8, L"空头保证金费",
    ui::TableColumn::LEFT, 100));
  columns2.push_back(ui::TableColumn(9, L"是否相对交易所收取",
    ui::TableColumn::LEFT, 100));
  

  table_ = new TableView(this, columns2, TEXT_ONLY, true, true, true);
  table_->SetObserver(this);



  ColumnSet* column_set = layout->AddColumnSet(0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL, 1,
                        GridLayout::USE_PREF, 0, 0);
  layout->StartRow(0.7f /* expand */, 0);
  layout->AddView(table_->CreateParentIfNecessary());

  
}

int MarginRateTable::RowCount() {
  return list_.size();
}

string16 MarginRateTable::GetText(int row, int column_id) {
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
        if (list_[row]->HedgeFlag == THOST_FTDC_HF_Speculation) {
          return L"投机";
        } else if (list_[row]->HedgeFlag == THOST_FTDC_HF_Hedge) {
          return L"套保";
        } else {
          return L"未知";
        }
        
        break;
      }
      case 5: {
        return ASCIIToUTF16(base::DoubleToString(list_[row]->LongMarginRatioByMoney));
        break;
      }
      case 6: {
        return ASCIIToUTF16(base::DoubleToString(list_[row]->LongMarginRatioByVolume));
        break;
      }
      case 7: {
        return ASCIIToUTF16(base::DoubleToString(list_[row]->ShortMarginRatioByMoney));
        break;
      }
      case 8: {
        return ASCIIToUTF16(base::DoubleToString(list_[row]->ShortMarginRatioByVolume));
        break;
      }
      case 9: {
        return (list_[row]->IsRelative == 1 ? L"是" : L"否");
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



gfx::ImageSkia MarginRateTable::GetIcon(int row) {
  DCHECK(false);
  return gfx::ImageSkia();
  //return row % 2 ? gfx::ImageSkia(icon1_) : gfx::ImageSkia(icon2_);
}

void MarginRateTable::SetObserver(ui::TableModelObserver* observer) {}

void MarginRateTable::OnSelectionChanged() {

}

void MarginRateTable::OnDoubleClick() {

}

void MarginRateTable::OnMiddleClick() {}

void MarginRateTable::OnKeyDown(ui::KeyboardCode virtual_keycode) {}

void MarginRateTable::OnTableViewDelete(TableView* table_view) {}

void MarginRateTable::OnTableView2Delete(TableView2* table_view) {}

void MarginRateTable::ButtonPressed(Button* sender, const ui::Event& event) {
  
}



void MarginRateTable::Observe(int type,
  const content::NotificationSource& source,
  const content::NotificationDetails& details) {

}

bool MarginRateTable::OnMessageReceived(IPC::Message const & msg) {

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
void MarginRateTable::EnsureVisible() {

}



void MarginRateTable::Selected() {
  
}

void MarginRateTable::OnMarginRate(PointerWrapper<CThostFtdcInstrumentMarginRateField> const & p) {
  
  uint32 const old_size = map_.size();
  std::shared_ptr<CThostFtdcInstrumentMarginRateField> sp(new CThostFtdcInstrumentMarginRateField(*p.get()));
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
