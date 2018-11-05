// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
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
#include "ui/views/controls/label.h"
#include "ui/views/controls/button/button.h"
#include "ui/views/controls/table/table_view.h"
#include "ui/views/controls/table/table_view_observer.h"
#include "ui/base/models/combobox_model.h"
#include "ui/base/win/window_impl.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/views/controls/combobox/combobox.h"
#include "ui/views/controls/combobox/combobox_listener.h"
#include "ui/views/controls/progress_bar.h"
#include "ui/views/controls/button/text_button.h"

#include "content/public/browser/notification_registrar.h"
#include "content/public/browser/notification_observer.h"
#include "content/browser/notification_service_impl.h"


//#include "log_daemon.h"
#include "../../phone_common/common/common_thread.h"
//#include "profiler_data.h"
#include "../../phone_common/include/ctp_define.h"
#include "../../phone_common/common/common_main.h"
#include "../../phone_common/common/thread_message_filter.h"
#include "../../phone_common/include/ctp_messages.h"

#include "../../phone_spi/phone_module/common.h"
#include "example_base.h"

namespace gfx {
class ImageSkia;
}

namespace views {
class Checkbox;
class Event;
class TableView;

namespace examples {

  //所有品种，所有级别=
  typedef std::vector<std::shared_ptr<phone_module::LevelQuotationInfoBig>> DataList;
  typedef std::vector<phone_module::LevelQuotationInfoBig const*> DataListIndex;
  //所有品种，某个级别，索引=
  typedef std::map<phone_module::MQLevel, DataListIndex> LevelMap;
  //某个品种，所有级别，索引=
  typedef std::map<std::string, DataListIndex> TypeMap;
  //某个品种，某个级别，索引=
  typedef std::map<std::pair<phone_module::MQLevel, std::string>, DataListIndex> LevelTypeMap;


  class LevelComboboxModel : public ui::ComboboxModel {
  public:
    LevelComboboxModel() {}
    virtual ~LevelComboboxModel() {}

    // Overridden from ui::ComboboxModel:
    virtual int GetItemCount() const OVERRIDE { return data_.size(); }
    virtual string16 GetItemAt(int index) OVERRIDE ;
/*
    View* GetItemViewAt(int index) {
      return example_list_[index]->example_view();
    }

    void AddExample(CTPViewBase* example) {
      example_list_.push_back(example);
    }*/

    void Add(phone_module::MQLevel const d) {
      data_.push_back(d);
    }

    phone_module::MQLevel Get(uint32 const i) {
      DCHECK(data_.size() > i);
      return data_[i];
    }

  private:
    std::vector<phone_module::MQLevel> data_;

    DISALLOW_COPY_AND_ASSIGN(LevelComboboxModel);
  };

  class TypeComboboxModel : public ui::ComboboxModel {
  public:
    TypeComboboxModel() {}
    virtual ~TypeComboboxModel() {}

    // Overridden from ui::ComboboxModel:
    virtual int GetItemCount() const OVERRIDE { return data_.size(); }
    virtual string16 GetItemAt(int index) OVERRIDE {
      return ASCIIToUTF16(data_[index]);
    }
/*
    View* GetItemViewAt(int index) {
      return example_list_[index]->example_view();
    }

    void AddExample(CTPViewBase* example) {
      example_list_.push_back(example);
    }*/

    void Add(std::string const &d) {
      data_.push_back(d);
    }
    std::string Get(uint32 const i) {
      DCHECK(data_.size() > i);
      return data_[i];
    }

  private:
    std::vector<std::string> data_;

    DISALLOW_COPY_AND_ASSIGN(TypeComboboxModel);
  };


class MQTable : public CTPViewBase,
                     public ui::TableModel,
                     public TableViewObserver,
                     public ButtonListener,
                     public ThreadMessageFilter,
                     public content::NotificationObserver,
                     public ComboboxListener {
 public:
  MQTable();
  virtual ~MQTable();
  virtual void Selected() OVERRIDE;
  // CTPViewBase:
  virtual void CreateExampleView(View* container) OVERRIDE;


  virtual void Observe(int type,
    const content::NotificationSource& source,
    const content::NotificationDetails& details) OVERRIDE;

  virtual bool OnMessageReceived(IPC::Message const & message) OVERRIDE;

  // ui::TableModel:
  virtual int RowCount() OVERRIDE;
  virtual string16 GetText(int row, int column_id) OVERRIDE;
  virtual gfx::ImageSkia GetIcon(int row) OVERRIDE;
  virtual void SetObserver(ui::TableModelObserver* observer) OVERRIDE;

  // TableViewObserver:
  virtual void OnSelectionChanged() OVERRIDE;
  virtual void OnDoubleClick() OVERRIDE;
  virtual void OnMiddleClick() OVERRIDE;
  virtual void OnKeyDown(ui::KeyboardCode virtual_keycode) OVERRIDE;
  virtual void OnTableViewDelete(TableView* table_view) OVERRIDE;
  virtual void OnTableView2Delete(TableView2* table_view) OVERRIDE;

  // ButtonListener:
  virtual void ButtonPressed(Button* sender, const ui::Event& event) OVERRIDE;

  //
  virtual void OnSelectedIndexChanged(Combobox* combobox) OVERRIDE;

  void type(std::string const &type) {
    type_ = type;
  }

  std::string const & type() {
    return type_;
  }

  void OnMQ(int32 const type, int32 const id, base::Time const & time, PointerWrapper<phone_module::LevelQuotationInfoBig> const & p);
  void OnOHLC_MQ(std::string const & type, PointerWrapper<phone_module::OHLCT> const & p);
 private:
   std::string type_;
   Label * price_open_;
   Label * price_open_time_;

   Label * price_high_;
   Label * price_high_time_;

   Label * price_low_;
   Label * price_low_time_;

   Label * price_close_;
   Label * price_close_time_;
  // The table to be tested.
  TableView* table_;

  Checkbox* column1_visible_checkbox_;
  Checkbox* column2_visible_checkbox_;
  Checkbox* column3_visible_checkbox_;
  Checkbox* column4_visible_checkbox_;
  BigTextButton * noused_button_;

  
  TypeComboboxModel type_combobox_model_;
  LevelComboboxModel level_combobox_model_;
  Combobox* level_combobox_;
  Combobox* type_combobox_;

  /*ProgressBar * connect_bar_;
  TextButton* connect_button_;*/

  DataList temp_data_;
  DataList list_data_;
  LevelMap level_map_;
  TypeMap type_map_;
  LevelTypeMap level_type_map_;


  
  void OnInitParam(std::string const & s);
  void OnLoginSuccess();
  void OnDisConnect();

  string16 GetColumnText(int id, phone_module::LevelQuotationInfoBig const & info);
  void EnsureVisible();


  bool OverrideThreadForMessage(IPC::Message const& message,
    CommonThread::ID* thread);

  DISALLOW_COPY_AND_ASSIGN(MQTable);
};

}  // namespace examples
}  // namespace views


