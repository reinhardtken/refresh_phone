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
        columns.push_back(ui::TableColumn(index++, L"�˺�",
          ui::TableColumn::LEFT, 200));
      }
      columns.push_back(ui::TableColumn(index++, L"ʱ��",
        ui::TableColumn::LEFT, 150));

      columns.push_back(ui::TableColumn(index++, L"Ʒ��",
        ui::TableColumn::LEFT, 100));
      columns.push_back(ui::TableColumn(index++, L"��/��",
        ui::TableColumn::LEFT, 100));
      columns.push_back(ui::TableColumn(index++, L"ռ�õı�֤��",
        ui::TableColumn::LEFT, 100));
      columns.push_back(ui::TableColumn(index++, L"��Ӧ�ĵ���",
        ui::TableColumn::LEFT, 100));
      columns.push_back(ui::TableColumn(index++, L"ֹ��ĵ���",
        ui::TableColumn::LEFT, 100));
      columns.push_back(ui::TableColumn(index++, L"ֹӯ�ĵ���",
        ui::TableColumn::LEFT, 100));
      columns.push_back(ui::TableColumn(index++, L"�ɱ�",
        ui::TableColumn::LEFT, 100));
      columns.push_back(ui::TableColumn(index++, L"������",
        ui::TableColumn::LEFT, 100));
      //  [7/20/2016 liuqingping]
      //���ӽ���ֲֺ͸�ҹ�ֲ�
      columns.push_back(ui::TableColumn(index++, L"��������",
        ui::TableColumn::LEFT, 100));
      columns.push_back(ui::TableColumn(index++, L"��ҹ����",
        ui::TableColumn::LEFT, 100));

      columns.push_back(ui::TableColumn(index++, L"CloseProfit",
        ui::TableColumn::LEFT, 100));
      columns.push_back(ui::TableColumn(index++, L"PositionProfit",
        ui::TableColumn::LEFT, 100));
      columns.push_back(ui::TableColumn(index++, L"MarginRateByMoney",
        ui::TableColumn::LEFT, 100));
      columns.push_back(ui::TableColumn(index++, L"Commission",
        ui::TableColumn::LEFT, 100));
      columns.push_back(ui::TableColumn(index++, L"����������",
        ui::TableColumn::LEFT, 100));
    }
  }
}