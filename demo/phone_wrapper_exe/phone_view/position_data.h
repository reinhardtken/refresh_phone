#pragma once

#include <string>
#include <list>
#include <vector>
#include <map>
#include <utility>
#include <memory>

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "base/utf_string_conversions.h"
#include "base/memory/scoped_ptr.h"
#include "base/scoped_native_library.h"
#include "base/i18n/icu_util.h"



#include "third_party/skia/include/core/SkBitmap.h"
#include "ui/base/models/table_model.h"
#include "ui/views/controls/button/button.h"
#include "ui/views/controls/table/table_view.h"
#include "ui/views/controls/table/table_view_observer.h"
#include "ui/base/models/combobox_model.h"
#include "ui/base/win/window_impl.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/views/controls/button/text_button.h"
#include "ui/views/controls/label.h"
#include "ui/views/controls/combobox/combobox.h"
#include "ui/views/controls/combobox/combobox_listener.h"
#include "ui/views/controls/textfield/textfield_controller.h"



#include "../../phone_common/common/common_thread.h"
//#include "profiler_data.h"
#include "../../phone_common/include/ctp_define.h"
#include "../../phone_common/common/common_main.h"
#include "../../phone_common/common/thread_message_filter.h"
#include "../../phone_common/include/ctp_messages.h"


namespace views {
  class Checkbox;
  class Event;
  class TableView;

  namespace examples {

    struct OrderResultData {

    };



    typedef std::map<std::string, std::shared_ptr<phone_module::PositionData>> PositionDataList;
    //typedef std::map<std::string, std::shared_ptr<order_execute::OrderUnit2UI>> OrderUnit2UIDataList;
    typedef std::map<std::string, SkColor> ColorMap;


    class OrderResultTable;

    class PositionDelegate {
    public:
      virtual int size() = 0;
      virtual string16 text(int row, int column_id) = 0;
    };




    class PositionModel : public ui::TableModel,
      public TableViewObserver {
    public:
      PositionModel(PositionDelegate *p)
        :delegate_(p) {}
      // ui::TableModel:
      virtual int RowCount() OVERRIDE;
      virtual string16 GetText(int row, int column_id) OVERRIDE;
      virtual gfx::ImageSkia GetIcon(int row) OVERRIDE;
      virtual void SetObserver(ui::TableModelObserver* observer) OVERRIDE {}

      // TableViewObserver:
      virtual void OnSelectionChanged() OVERRIDE {}
      virtual void OnDoubleClick() OVERRIDE {}
      virtual void OnMiddleClick() OVERRIDE {}
      virtual void OnKeyDown(ui::KeyboardCode virtual_keycode) OVERRIDE {}
      virtual void OnTableViewDelete(TableView* table_view) OVERRIDE {}
      virtual void OnTableView2Delete(TableView2* table_view) OVERRIDE {}

    private:
      PositionDelegate *delegate_;
    };



    void MakeupPositionColunms(std::vector<ui::TableColumn> &columns, bool const);



  }
}