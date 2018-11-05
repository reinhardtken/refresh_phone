// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "package_list_table.h"

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


PackageListTable::PackageListTable(CTPTabbedPane *p, std::wstring const &) 
  :CTPViewBase("Table"),
  pane_(p),
  table_(NULL),
  ThreadMessageFilter(true) {


    /*if (pane_->HasID(bc_)) {
      ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::CTP,
        new CTP_TU_QryTypeList(pane_->GetID(bc_)));
    } else {
      DCHECK(false);
    }*/

}

PackageListTable::~PackageListTable() {
  // Delete the view before the model.
  delete table_;
  table_ = NULL;

}

void PackageListTable::OnSelectedIndexChanged(Combobox* combobox) {

}

void PackageListTable::CreateExampleView(View* container) {

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
  columns2.push_back(ui::TableColumn(0, L"Package",
    ui::TableColumn::LEFT, 200));

  columns2.push_back(ui::TableColumn(1, L"App",
    ui::TableColumn::LEFT, 100));

  columns2.push_back(ui::TableColumn(2, L"Path",
    ui::TableColumn::LEFT, 700));
  
  

  table_ = new TableView(this, columns2, TEXT_ONLY, true, true, true);
  table_->SetObserver(this);



  ColumnSet* column_set = layout->AddColumnSet(0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL, 1,
                        GridLayout::USE_PREF, 0, 0);
  layout->StartRow(0.7f /* expand */, 0);
  layout->AddView(table_->CreateParentIfNecessary());

  
}

int PackageListTable::RowCount() {
  return date_.size();
}

string16 PackageListTable::GetText(int row, int column_id) {
  if ((uint32)row >= date_.size()) {
    return string16();
  }
  switch (column_id) {
      case 0: {
		  return date_[row].package;
        break;
      }
      case 1: {
        return date_[row].type == PKGT_THIRD_PARTY ? TEXT("User") : TEXT("System");
        break;
      }
      case 2: {
		  return date_[row].path;
        break;
      }
      //case 3: {
      //  return ASCIIToUTF16(list_[row]->InvestorID);
      //  break;
      //}
      /*case 4: {
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
      }*/
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



gfx::ImageSkia PackageListTable::GetIcon(int row) {
  DCHECK(false);
  return gfx::ImageSkia();
  //return row % 2 ? gfx::ImageSkia(icon1_) : gfx::ImageSkia(icon2_);
}

void PackageListTable::SetObserver(ui::TableModelObserver* observer) {}

void PackageListTable::OnSelectionChanged() {
  /*if (table_->FirstSelectedRow() >= 0) {
    PrintStatus("Selected: %s",
                UTF16ToASCII(GetText(table_->FirstSelectedRow(), 0)).c_str());
  }*/
}

void PackageListTable::OnDoubleClick() {
  /*if (table_->FirstSelectedRow() >= 0) {
    PrintStatus("Double Click: %s",
                UTF16ToASCII(GetText(table_->FirstSelectedRow(), 0)).c_str());
  }*/
}

void PackageListTable::OnMiddleClick() {}

void PackageListTable::OnKeyDown(ui::KeyboardCode virtual_keycode) {}

void PackageListTable::OnTableViewDelete(TableView* table_view) {}

void PackageListTable::OnTableView2Delete(TableView2* table_view) {}

void PackageListTable::ButtonPressed(Button* sender, const ui::Event& event) {
  
}



void PackageListTable::Observe(int type,
  const content::NotificationSource& source,
  const content::NotificationDetails& details) {

}

bool PackageListTable::OnMessageReceived(IPC::Message const & msg) {

	if (msg.routing_id() == MSG_ROUTING_CONTROL) {
		// Dispatch control messages.
		bool msg_is_ok = false;
		IPC_BEGIN_MESSAGE_MAP_EX(PackageListTable, msg, msg_is_ok)

			IPC_MESSAGE_HANDLER(L2U_PackageList, OnUpdatePackageList)

			//IPC_MESSAGE_UNHANDLED_ERROR()
			IPC_END_MESSAGE_MAP_EX()

			if (msg_is_ok) {
				return true;
			}
	}

  // Dispatch incoming messages to the appropriate RenderView/WidgetHost.
  //RenderWidgetHost* rwh = render_widget_hosts_.Lookup(msg.routing_id());

  return false; 

}


//TaskManagerTableModel
void PackageListTable::EnsureVisible() {

}



void PackageListTable::Selected() {
  
}

void PackageListTable::OnMarginRate(PointerWrapper<CThostFtdcInstrumentMarginRateField> const & p) {
  
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

void PackageListTable::OnUpdatePackageList(std::wstring const &, PointerWrapper<std::vector<ANDROID_PACKAGE_T>> const & p) {
	std::vector<ANDROID_PACKAGE_T> * date = p.get();
	date_.swap(*date);
	table_->OnModelChanged();
}

}  // namespace examples
}  // namespace views
