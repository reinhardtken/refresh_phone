// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc_table.h"

#include <vector>

#include "base/string_util.h"
#include "base/string_tokenizer.h"
#include "base/string_number_conversions.h"
#include "third_party/skia/include/core/SkBitmap.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "ui/gfx/image/image_skia.h"
#include "ui/views/controls/button/checkbox.h"
#include "ui/views/layout/grid_layout.h"


namespace {

}


namespace views {
namespace examples {
  
  CCTableView::CCTableView(Delegate * delegate, ui::TableModel* model, const std::vector<ui::TableColumn>& columns,
    TableTypes table_type, bool single_selection,
    bool resizable_columns, bool autosize_columns)
    :TableView(model, columns, table_type, single_selection, resizable_columns, autosize_columns),
    delegate_(delegate) {
    SetCustomColorsEnabled(true);
  }


  bool CCTableView::GetCellColors(int model_row,
    int column,
    ItemColor* foreground,
    ItemColor* background,
    LOGFONT* logfont) {
      if (delegate_) {
        return delegate_->GetCellColors(this, model_row, column, foreground, background, logfont);
      }

      return false;
  }

}  // namespace examples
}  // namespace views
