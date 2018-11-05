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
#include "ui/views/controls/button/button.h"
#include "ui/views/controls/table/table_view.h"
#include "ui/views/controls/table/table_view_observer.h"
#include "ui/base/models/combobox_model.h"
#include "ui/base/win/window_impl.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/views/controls/combobox/combobox.h"
#include "ui/views/controls/combobox/combobox_listener.h"

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

  class CCTableView : public TableView {
  public:
    struct Delegate {
      virtual bool GetCellColors(
        CCTableView* who,
        int model_row,
        int column,
        ItemColor* foreground,
        ItemColor* background,
        LOGFONT* logfont) = 0;
    };

    CCTableView(Delegate * delegate, ui::TableModel* model, const std::vector<ui::TableColumn>& columns,
      TableTypes table_type, bool single_selection,
      bool resizable_columns, bool autosize_columns);
    virtual ~CCTableView() {}

    virtual bool GetCellColors(int model_row,
      int column,
      ItemColor* foreground,
      ItemColor* background,
      LOGFONT* logfont) OVERRIDE;

  private:
    Delegate * delegate_;
  };



}  // namespace examples
}  // namespace views


