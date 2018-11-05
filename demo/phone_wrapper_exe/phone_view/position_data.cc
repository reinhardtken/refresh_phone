#include "position_data.h"


namespace views {


  namespace examples {


    int PositionModel::RowCount() {
      return delegate_->size();
    }

    string16 PositionModel::GetText(int row, int column_id) {
      return delegate_->text(row, column_id);
    }


    gfx::ImageSkia PositionModel::GetIcon(int row) {
      DCHECK(false);
      return gfx::ImageSkia();
    }

    //=====================================================
    void MakeupPositionColunms(std::vector<ui::TableColumn> &columns, bool const global) {

      int index = 0;
      if (global) {
        columns.push_back(ui::TableColumn(index++, L"账号",
          ui::TableColumn::LEFT, 200));
      }
      columns.push_back(ui::TableColumn(index++, L"时间",
        ui::TableColumn::LEFT, 150));

      columns.push_back(ui::TableColumn(index++, L"品种",
        ui::TableColumn::LEFT, 100));
      columns.push_back(ui::TableColumn(index++, L"多/空",
        ui::TableColumn::LEFT, 100));
      columns.push_back(ui::TableColumn(index++, L"占用的保证金",
        ui::TableColumn::LEFT, 100));
      columns.push_back(ui::TableColumn(index++, L"对应的点数",
        ui::TableColumn::LEFT, 100));
      columns.push_back(ui::TableColumn(index++, L"止损的点数",
        ui::TableColumn::LEFT, 100));
      columns.push_back(ui::TableColumn(index++, L"止盈的点数",
        ui::TableColumn::LEFT, 100));
      columns.push_back(ui::TableColumn(index++, L"成本",
        ui::TableColumn::LEFT, 100));
      columns.push_back(ui::TableColumn(index++, L"总数量",
        ui::TableColumn::LEFT, 100));
      //  [7/20/2016 liuqingping]
      //增加今天持仓和隔夜持仓
      columns.push_back(ui::TableColumn(index++, L"今天数量",
        ui::TableColumn::LEFT, 100));
      columns.push_back(ui::TableColumn(index++, L"隔夜数量",
        ui::TableColumn::LEFT, 100));

      columns.push_back(ui::TableColumn(index++, L"CloseProfit",
        ui::TableColumn::LEFT, 100));
      columns.push_back(ui::TableColumn(index++, L"PositionProfit",
        ui::TableColumn::LEFT, 100));
      columns.push_back(ui::TableColumn(index++, L"MarginRateByMoney",
        ui::TableColumn::LEFT, 100));
      columns.push_back(ui::TableColumn(index++, L"Commission",
        ui::TableColumn::LEFT, 100));
      columns.push_back(ui::TableColumn(index++, L"自算手续费",
        ui::TableColumn::LEFT, 100));
    }
  }
}