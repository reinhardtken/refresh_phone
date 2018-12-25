// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "mq_guard_view.h"

#include <vector>
#include <functional>

#include "base/command_line.h"
#include "base/base_paths.h"
#include "base/path_service.h"
#include "base/process_util.h"
#include "base/file_path.h"
#include "base/string_util.h"
#include "base/string_tokenizer.h"
#include "base/string_number_conversions.h"
#include "third_party/skia/include/core/SkBitmap.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "ui/gfx/image/image_skia.h"
#include "ui/views/widget/widget.h"
#include "ui/views/controls/button/checkbox.h"
#include "ui/views/layout/grid_layout.h"
#include "ui/base/theme_provider.h"
#include "content/browser/notification_service_impl.h"
#include "content/public/common/content_switches.h"


#include "../../phone_common/channel/channel_host.h"
#include "../../phone_common/channel/codec/protobuf_codec.h"
#include "../../phone_common/channel/codec/tlv_codec.h"
#include "../../phone_common/channel/codec/tlv_define.h"
#include "../../phone_common/common/common_common.h"
#include "../../phone_common/common/common_util.h"
#include "../../phone_common/common/pref_service.h"

#include "grit/theme_resources.h"

#include "../../phone_common/include/ctp_notification_types.h"

#include "../../protobuf/out/guard_comunication.pb.h"


namespace {
  
}


namespace views {
namespace examples {
  //================================================================

MQGuardView::MQGuardView() 
  :CTPViewBase(L"Table") ,
  table_(NULL),
  ThreadMessageFilter(true) {

  ProcessRecord pr;


  pr.alive = false;
  pr.name = L"交易进程";
  pr.process_id = 0;
  pr.socket_name = switches::kCommunicateMQ;
  process_vector_.push_back(pr);

  pr.alive = false;
  pr.name = L"Py行情存储进程";
  pr.process_id = 0;
  pr.socket_name = switches::kCommunicateMQSQL;
  process_vector_.push_back(pr);

}

MQGuardView::~MQGuardView() {
  delete table_;
  table_ = NULL;
}

void MQGuardView::OnSelectedIndexChanged(Combobox* combobox) {


}

void MQGuardView::CreateExampleView(View* container) {

  ui::ThemeProvider* tp = container->GetThemeProvider();
  alive_ = tp->GetImageSkiaNamed(IDR_UPDATE_UPTODATE);
  die_ = tp->GetImageSkiaNamed(IDR_UPDATE_FAIL);

  

  GridLayout* layout = new GridLayout(container);
  container->SetLayoutManager(layout);

  std::vector<ui::TableColumn> columns;
  columns.push_back(ui::TableColumn(0, L"状态",
                                    ui::TableColumn::LEFT, 100));

  columns.push_back(ui::TableColumn(1, L"进程说明",
                                    ui::TableColumn::LEFT, 200));
  columns.push_back(ui::TableColumn(2, L"进程ID",
                                    ui::TableColumn::LEFT, 100));
  /*columns.push_back(ui::TableColumn(3, L"开",
                                    ui::TableColumn::LEFT, 100));
  columns.push_back(ui::TableColumn(4, L"高",
    ui::TableColumn::LEFT, 100));
  columns.push_back(ui::TableColumn(5, L"低",
    ui::TableColumn::LEFT, 100));
  columns.push_back(ui::TableColumn(6, L"收",
    ui::TableColumn::LEFT, 100));
  columns.push_back(ui::TableColumn(7, L"量",
    ui::TableColumn::LEFT, 100));
  columns.push_back(ui::TableColumn(8, L"样本数",
    ui::TableColumn::LEFT, 100));*/

  table_ = new TableView(this, columns, ICON_AND_TEXT, true, true, true);
  table_->SetObserver(this);
  table_->set_context_menu_controller(this);
  
  ColumnSet* column_set = layout->AddColumnSet(0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL, 1,
                        GridLayout::USE_PREF, 0, 0);
  layout->StartRow(1 /* expand */, 0);
  layout->AddView(table_->CreateParentIfNecessary());

  

}

int MQGuardView::RowCount() {
  return process_vector_.size();
}

string16 MQGuardView::GetText(int row, int column_id) {
  
  switch (column_id) {
    case 0: {
      return process_vector_[row].alive == true ? L"已启动" : L"未启动";
      break;
    }
    case 1: {
      return process_vector_[row].name;
      break;
    }
    case 2: {
      return base::UintToString16(process_vector_[row].process_id);
      break;
    }
  }
  

  return string16();
}

string16 MQGuardView::GetColumnText(int id, phone_module::LevelQuotationInfoBig const & info) {
  switch (id) {
      case 0: {
        return ASCIIToUTF16(info.time_string);
        break;
      }
      case 1: {
        return ASCIIToUTF16(info.type);
        break;
      }
      case 2: {
        return L"";//ASCIIToUTF16(Enum2String(info.level));
        break;
      }
      case 3: {
        return ASCIIToUTF16(base::DoubleToString(info.open));
        break;
      }
      case 4: {
        return ASCIIToUTF16(base::DoubleToString(info.high));
        break;
      }
      case 5: {
        return ASCIIToUTF16(base::DoubleToString(info.low));
        break;
      }
      case 6: {
        return ASCIIToUTF16(base::DoubleToString(info.close));
        break;
      }
      case 7: {
        return ASCIIToUTF16(base::UintToString(info.volumn));
        break;
      }
      case 8: {
        return ASCIIToUTF16(base::UintToString(info.number));
        break;
      }
      default: {
        DCHECK(false);
      }
  }
 

  return string16();
}

gfx::ImageSkia MQGuardView::GetIcon(int row) {
  //DCHECK(false);
  //return gfx::ImageSkia();
  if ((uint32)row < process_vector_.size()) {
    return process_vector_[row].alive == true ? *alive_ : *die_;
  }

  return gfx::ImageSkia();
}

void MQGuardView::SetObserver(ui::TableModelObserver* observer) {}

void MQGuardView::OnSelectionChanged() {

}

void MQGuardView::OnDoubleClick() {
/*
  uint32 index = table_->FirstSelectedRow();
  if (index < process_vector_.size()) {
    if (process_vector_[index].alive == false) {
      if (process_vector_[index].socket_name == switches::kCommunicateCmdAndSQLAlive ||
        process_vector_[index].socket_name == switches::kCommunicateMQSQLAlive) {
          std::string cmd = common_util::g_pref_service->GetString(prefs::kPyPathAndCmd);
          DCHECK_RLOG(cmd.size());
          //FilePath run_cmd(ASCIIToWide(cmd));
          
          //CommandLine cmd_line(run_cmd);
 
          //LOG(INFO) << "Launching: " << cmd_line.GetCommandLineString();
          base::LaunchProcess(ASCIIToWide(cmd), base::LaunchOptions(), NULL);
      } else {
        FilePath exe_path;
        PathService::Get(base::FILE_EXE, &exe_path);
        CommandLine cmd_line(exe_path);
        cmd_line.AppendSwitchASCII(switches::kProcessType, common_util::SocketName2ProcessType(process_vector_[index].socket_name));
        LOG(INFO) << "Launching: " << cmd_line.GetCommandLineString();
        base::LaunchProcess(cmd_line, base::LaunchOptions(), NULL);
      }
      
    }
  }*/
}

void MQGuardView::OnMiddleClick() {}

void MQGuardView::OnKeyDown(ui::KeyboardCode virtual_keycode) {}

void MQGuardView::OnTableViewDelete(TableView* table_view) {}

void MQGuardView::OnTableView2Delete(TableView2* table_view) {}

void MQGuardView::ButtonPressed(Button* sender, const ui::Event& event) {

}



void MQGuardView::Observe(int type,
  const content::NotificationSource& source,
  const content::NotificationDetails& details) {
}

bool MQGuardView::OnMessageReceived(IPC::Message const & msg) {


  CommonThread::ID thread;
  
  if (!CommonThread::GetCurrentThreadIdentifier(&thread) &&
    OverrideThreadForMessage(msg, &thread)) {

      CommonThread::PostTask(
        thread, FROM_HERE,
        base::Bind(base::IgnoreResult(&MQGuardView::OnMessageReceived),
        base::Unretained(this), msg));

      return true;
  }


  if (msg.routing_id() == MSG_ROUTING_CONTROL) {
    // Dispatch control messages.
    bool msg_is_ok = false;

    IPC_BEGIN_MESSAGE_MAP_EX(MQGuardView, msg, msg_is_ok)
      IPC_MESSAGE_HANDLER(CTP_MQConnectStatus2UI, OnMQConnect)

      IPC_END_MESSAGE_MAP_EX()

      if (msg_is_ok) {
        return true;
      }
  }

  // Dispatch incoming messages to the appropriate RenderView/WidgetHost.
  //RenderWidgetHost* rwh = render_widget_hosts_.Lookup(msg.routing_id());

  return false; 

}

void MQGuardView::OnMQConnect(std::string const &name, bool const connected) {
  for(auto it = process_vector_.begin(); it != process_vector_.end(); ++it) {
    if (name == it->socket_name && connected != it->alive) {
      it->alive = connected;
      if (connected == false) {
        it->process_id = 0;
      }
      if (table_) {
        table_->OnModelChanged();
      }
      return;
    }
  }
}


void MQGuardView::OnLoginSuccess() {

}


void MQGuardView::OnDisConnect() {

}


//TaskManagerTableModel
void MQGuardView::EnsureVisible() {
  
}

void MQGuardView::OnMQ(int32 const type, int32 const id, base::Time const & time, PointerWrapper<phone_module::LevelQuotationInfoBig> const & p) {
    // table_->OnModelChanged();
    // EnsureVisible();
}

void MQGuardView::OnInitParam(std::string const & s) {

}


bool MQGuardView::OverrideThreadForMessage(IPC::Message const& message,
  CommonThread::ID* thread) {
    if (message.type() == CTP_CONSOLE_Command::ID || 
      message.type() == CTP_CONSOLE_CommandProtoBuf::ID) {
        *thread = CommonThread::UI;
        return true;
    }

    return false;
}

void MQGuardView::Selected() {

}


void MQGuardView::ShowContextMenuForView(views::View* source,
  const gfx::Point& point) {
    //UpdateStatsCounters();
    scoped_ptr<views::Menu> menu(views::Menu::Create(
      this, views::Menu::TOPLEFT, source->GetWidget()->GetNativeView()));

      menu->AppendMenuItem(0, L"启动",
          views::Menu::NORMAL);
      menu->AppendMenuItem(1, L"重新启动",
        views::Menu::NORMAL);

    menu->RunMenuAt(point.x(), point.y());
}

void MQGuardView::ExecuteCommand(int id) {
  //tab_table_->SetColumnVisibility(id, !tab_table_->IsColumnVisible(id));
}


void MQGuardView::DispatchProtobufMessage(std::string const & name, codec::MessagePtr const & p, base::Time const&) {
  ctp::guard::ReportPid const* report = static_cast<ctp::guard::ReportPid const*>(p.get());
  for(auto it = process_vector_.begin(); it != process_vector_.end(); ++it) {
    if (name == it->socket_name) {
      it->process_id = report->process_id();
      if (table_) {
        table_->OnModelChanged();
      }
      return;
    }
  }

}

void MQGuardView::DispatchTLVMessage(std::string const & name, codec::TLVMessagePtr const & p, base::Time const&) {
 
}

void MQGuardView::CheckAlive() {
  //check
  // bool dirty = false;
  // for(auto it = process_vector_.begin(); it != process_vector_.end(); ++it) {
  //   bool alive = channel_host_->HasConnection(it->socket_name);
  //   if (alive != it->alive) {
  //     it->alive = alive;
  //     dirty = true;
  //   }
  // }

  // if (dirty) {
  //   table_->OnModelChanged();
  // }

  // alive_check_.reset(
  //   new VoidVoidCallback(
  //   base::Bind(&MQGuardView::CheckAlive, base::Unretained(this))));
  // CommonThread::PostDelayedTask(CommonThread::UI,
  //   FROM_HERE,
  //   alive_check_->callback(),
  //   5 * 1000);
}


}  // namespace examples
}  // namespace views
