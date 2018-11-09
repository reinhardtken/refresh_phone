// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "update_view.h"

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
#include "net\base\escape.h"
#include "third_party/skia/include/core/SkBitmap.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "ui/gfx/image/image_skia.h"
#include "ui/views/widget/widget.h"
#include "ui/views/controls/button/checkbox.h"
#include "ui/views/layout/grid_layout.h"
#include "ui/base/theme_provider.h"
#include "content/browser/notification_service_impl.h"
#include "content/public/common/content_switches.h"


#include "phone_common/channel/channel_host.h"
#include "phone_common/channel/codec/protobuf_codec.h"
#include "phone_common/channel/codec/tlv_codec.h"
#include "phone_common/channel/codec/tlv_define.h"
#include "phone_common/common/common_common.h"
#include "phone_common/common/common_util.h"
#include "phone_common/common/pref_service.h"

#include "grit/theme_resources.h"

#include "phone_common/include/ctp_notification_types.h"

#include "protobuf/out/guard_comunication.pb.h"


namespace {

}


namespace views {
namespace examples {
	struct ProcessRecord {
		ProcessRecord()
			:process_handler(INVALID_HANDLE_VALUE) {
			socket_name = "1234";
		}
		std::wstring name;
		std::wstring description;
		bool alive;
		uint32 process_id;
		std::string socket_name;
		HANDLE process_handler;
		//std::string socket_name;
		enum Status {
			MANUAL,//手动
			AUTO_ALIVE,//自动（已启动）
			AUTO_WAIT_BOOT,//自动（启动中）
			AUTO_DIED,//自动（未启动）
		};
		bool is_auto;
		Status status;
	};
  //================================================================

UpdateView::UpdateView() 
  :CTPViewBase("Table") ,
  table_(NULL),
  ALLOW_THIS_IN_INITIALIZER_LIST(weak_factory_(this)),
  ThreadMessageFilter(true) {
    
  //================================================








}

UpdateView::~UpdateView() {

  // Delete the view before the model.
  delete table_;
  table_ = NULL;
}

void UpdateView::OnSelectedIndexChanged(Combobox* combobox) {


}

void UpdateView::CreateExampleView(View* container) {

  ui::ThemeProvider* tp = container->GetThemeProvider();
  alive_ = tp->GetImageSkiaNamed(IDR_UPDATE_UPTODATE);
  die_ = tp->GetImageSkiaNamed(IDR_UPDATE_FAIL);

  

  GridLayout* layout = new GridLayout(container);
  container->SetLayoutManager(layout);

  std::vector<ui::TableColumn> columns;
  columns.push_back(ui::TableColumn(0, L"时间",
                                    ui::TableColumn::LEFT, 100));

  columns.push_back(ui::TableColumn(1, L"操作",
                                    ui::TableColumn::LEFT, 150));
  columns.push_back(ui::TableColumn(2, L"结果",
                                    ui::TableColumn::LEFT, 150));
  columns.push_back(ui::TableColumn(4, L"具体信息",
    ui::TableColumn::LEFT, 250));
  columns.push_back(ui::TableColumn(3, L"包名",
    ui::TableColumn::LEFT, 300));
  
  //columns.push_back(ui::TableColumn(5, L"价格",
  //  ui::TableColumn::LEFT, 100));



  table_ = new TableView(this, columns, ICON_AND_TEXT, true, true, true);
  table_->SetObserver(this);
  table_->set_context_menu_controller(this);
  
  int index = 0;
  ColumnSet* column_set = layout->AddColumnSet(index);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL, 1,
                        GridLayout::USE_PREF, 0, 0);
  layout->StartRow(1 /* expand */, index);
  layout->AddView(table_->CreateParentIfNecessary());
  //========================================
  ++index;
  launch_py_ = new TextButton(this, L"启动包更新模块");
  launch_py_->set_alignment(TextButton::ALIGN_CENTER);
  check_update_ = new TextButton(this, L"检查网络包更新");
  check_update_->set_alignment(TextButton::ALIGN_CENTER);
  remove_local_ = new TextButton(this, L"清除本地包数据");
  remove_local_->set_alignment(TextButton::ALIGN_CENTER);
  clear_table_ = new TextButton(this, L"清除显示");
  clear_table_->set_alignment(TextButton::ALIGN_CENTER);
  

  column_set = layout->AddColumnSet(index);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
	  1.0f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);

  layout->StartRow(0 /* expand */, index);
  layout->AddView(launch_py_);
  layout->AddView(check_update_);
  layout->AddView(remove_local_);
  layout->AddView(clear_table_);
  //=========================================
  ++index;
  module_status_ = new Label(L"包管理模块-未连接");
  column_set = layout->AddColumnSet(index);
  column_set->AddColumn(GridLayout::FILL, GridLayout::FILL,
    1.0f, GridLayout::USE_PREF, 0, 0);
  layout->StartRow(0 /* expand */, index);
  layout->AddView(module_status_);

}

int UpdateView::RowCount() {
  return data_.size();
}

string16 UpdateView::GetText(int row, int column_id) {
  
  switch (column_id) {
    case 0: {
      return data_[row].time_string;
      break;
    }
    case 1: {
      return data_[row].op;
      break;
    }
    case 2: {
      return data_[row].success == 0 ? L"成功" : StringPrintf(L"失败：错误码：%d", data_[row].success);
      break;
    }
    case 3: {
      return data_[row].package_name;
      break;
    }         
    case 4: {
      return data_[row].info;
      break;
    }
  }
  

  return string16();
}



gfx::ImageSkia UpdateView::GetIcon(int row) {
  if ((uint32)row < data_.size()) {
    return data_[row].success == 0 ? *alive_ : *die_;
  }

  return gfx::ImageSkia();
}

void UpdateView::SetObserver(ui::TableModelObserver* observer) {}

void UpdateView::OnSelectionChanged() {

}

void UpdateView::OnDoubleClick() {
  uint32 index = table_->FirstSelectedRow();

}

void UpdateView::InitLaunch(base::WeakPtr<UpdateView> weak_ptr, std::vector<std::pair<uint64, std::string>> const & vec) {


}



void UpdateView::TimingLaunchOrTerminate(base::WeakPtr<UpdateView> weak_ptr, std::vector<std::pair<uint64, std::string>> const & vec, bool start) {
  CommonThread::CurrentlyOn(CommonThread::FILE);

  if (start) {
    for (auto it = vec.begin(); it != vec.end(); ++it) {
      CommonThread::PostTask(CommonThread::UI,
        FROM_HERE,
        base::Bind(&UpdateView::LaunchWrapper, weak_ptr, it->second));

      Sleep(1000 * 15);

    }
  } else {
    for (auto it = vec.begin(); it != vec.end(); ++it) {
      CommonThread::PostTask(CommonThread::UI,
        FROM_HERE,
        base::Bind(&UpdateView::Terminate, weak_ptr, it->second));

      Sleep(1000 * 15);

    }
  }
}

void UpdateView::LaunchWrapper(std::string const & socket_name) {
  //for (uint32 i = 0; i < process_vector_->size(); i++) {
  //  if (process_vector_->operator[](i).socket_name == socket_name) {
  //    Launch(i);
  //    break;
  //  }
  //}
}

void UpdateView::Launch(uint32 const index) {
  //if (process_vector_->size() > index) {
  //  std::string socket_name = process_vector_->operator[](index).socket_name;
  //  LOG(INFO)<<"UpdateView::Launch =====>"<<socket_name;

  //  if (socket_name == switches::kCommunicateMQSQLAlive) {
  //    std::string cmd = common_util::g_pref_service->GetString(prefs::kMQPyPathAndCmd);
  //    DCHECK_RLOG(cmd.size());
  //    LOG(INFO) << "Launching: " << cmd;
  //    base::LaunchProcess(ASCIIToWide(cmd), base::LaunchOptions(), &process_vector_->operator[](index).process_handler);
  //  } else if (socket_name == switches::kCommunicateCmdAndSQLAlive) {
  //    std::string cmd = common_util::g_pref_service->GetString(prefs::kTUPyPathAndCmd);
  //    DCHECK_RLOG(cmd.size());
  //    LOG(INFO) << "Launching: " << cmd;
  //    base::LaunchProcess(ASCIIToWide(cmd), base::LaunchOptions(), &process_vector_->operator[](index).process_handler);
  //  } else if (socket_name == switches::kCommunicatePyBacktestingAlive) {
  //    std::string cmd = common_util::g_pref_service->GetString(prefs::kBacktestingPyPathAndCmd);
  //    DCHECK_RLOG(cmd.size());
  //    LOG(INFO) << "Launching: " << cmd;
  //    base::LaunchProcess(ASCIIToWide(cmd), base::LaunchOptions(), &process_vector_->operator[](index).process_handler);
  //  } else if (socket_name == switches::kCommunicatePyMQCheckAlive) {
  //    std::string cmd = common_util::g_pref_service->GetString(prefs::kMQCheckPyPathAndCmd);
  //    DCHECK_RLOG(cmd.size());
  //    LOG(INFO) << "Launching: " << cmd;
  //    base::LaunchProcess(ASCIIToWide(cmd), base::LaunchOptions(), &process_vector_->operator[](index).process_handler);
  //  } else if (socket_name == switches::kCommunicatePyToolboxAlive) {
  //    std::string cmd = common_util::g_pref_service->GetString(prefs::kPyToolboxPyPathAndCmd);
  //    DCHECK_RLOG(cmd.size());
  //    LOG(INFO) << "Launching: " << cmd;
  //    //toolbox不维持alive状态
  //    HANDLE process_handler;
  //    base::LaunchProcess(ASCIIToWide(cmd), base::LaunchOptions(), &process_handler);
  //  } else {
  //    FilePath exe_path;
  //    PathService::Get(base::FILE_EXE, &exe_path);
  //    CommandLine cmd_line(exe_path);
  //    cmd_line.AppendSwitchASCII(switches::kProcessType, common_util::SocketName2ProcessType(socket_name));
  //    if (auto_run_.size()) {
  //      auto it = auto_run_.find(socket_name);
  //      if (it != auto_run_.end() && it->second.params.size()) {
  //        cmd_line.AppendSwitch(it->second.params);
  //      }
  //    } else if (timing_run_.size()) {
  //      auto it = timing_run_.find(socket_name);
  //      if (it != timing_run_.end() && it->second.params.size()) {
  //        cmd_line.AppendSwitch(it->second.params);
  //      }
  //    }

  //    LOG(INFO) << "Launching: " << cmd_line.GetCommandLineString();
  //    base::LaunchProcess(cmd_line, base::LaunchOptions(), &process_vector_->operator[](index).process_handler);
  //  }
  //} else {
  //  DCHECK_RLOG(false);
  //}
}

void UpdateView::Terminate(std::string const & socket_name) {
  //LOG(INFO)<<"UpdateView::Terminate =====>"<<socket_name;

  //for(auto it = process_vector_->begin(); it != process_vector_->end(); ++it) {
  //  if (socket_name == it->socket_name && true == it->alive) {
  //    HANDLE handle = OpenProcess(PROCESS_TERMINATE, FALSE, it->process_id);
  //    TerminateProcess(handle, 0);
  //    return;
  //  }
  //}
}


void UpdateView::OnMiddleClick() {}

void UpdateView::OnKeyDown(ui::KeyboardCode virtual_keycode) {}

void UpdateView::OnTableViewDelete(TableView* table_view) {}

void UpdateView::OnTableView2Delete(TableView2* table_view) {}

void UpdateView::ButtonPressed(Button* sender, const ui::Event& event) {
	if (sender == check_update_) {
    ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::CTP, new U2L_GetNetInstallApkList());
	} else if (sender == remove_local_) {
    ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::CTP, new U2L_RemoveLocalInstallApkList());
  } else if (sender == clear_table_) {
    data_.clear();
    table_->OnModelChanged();
  } else if (sender == launch_py_) {
    ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::CTP, new U2L_LaunchPY());
  }
 
}



void UpdateView::Observe(int type,
  const content::NotificationSource& source,
  const content::NotificationDetails& details) {
}

bool UpdateView::OnMessageReceived(IPC::Message const & msg) {
  if (msg.routing_id() == MSG_ROUTING_CONTROL) {
    // Dispatch control messages.
    bool msg_is_ok = false;
    IPC_BEGIN_MESSAGE_MAP_EX(UpdateView, msg, msg_is_ok)
      IPC_MESSAGE_HANDLER(L2U_ApkUpdateInfo, OnApkUpdateInfo)
      IPC_MESSAGE_HANDLER(L2U_ModuleUpdate, OnModuleUpdate)
      IPC_END_MESSAGE_MAP_EX()

      if (msg_is_ok) {
        return true;
      }
  }

  // Dispatch incoming messages to the appropriate RenderView/WidgetHost.
  //RenderWidgetHost* rwh = render_widget_hosts_.Lookup(msg.routing_id());

  return false;

}


void UpdateView::OnApkUpdateInfo(PointerWrapper<phone_module::ApkUpdateInfo> const & p) {
  data_.push_back(*p.get());
  table_->OnItemsAdded(data_.size() - 1, 1);
  EnsureVisible();
}

void UpdateView::OnModuleUpdate(PointerWrapper<phone_module::DeviceData> const & p) {
  module_status_->SetText(p.get()->info);
}


//TaskManagerTableModel
void UpdateView::EnsureVisible() {
  if (data_.size()) {
    table_->Select(data_.size() - 1);
  }
}




bool UpdateView::OverrideThreadForMessage(IPC::Message const& message,
  CommonThread::ID* thread) {
    if (message.type() == CTP_CONSOLE_Command::ID || 
      message.type() == CTP_CONSOLE_CommandProtoBuf::ID) {
        *thread = CommonThread::UI;
        return true;
    }

    return false;
}

void UpdateView::Selected() {

}


void UpdateView::ShowContextMenuForView(views::View* source,
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

void UpdateView::ExecuteCommand(int id) {
  //tab_table_->SetColumnVisibility(id, !tab_table_->IsColumnVisible(id));
}


void UpdateView::DispatchProtobufMessage(std::string const & name, codec::MessagePtr const & p, base::Time const&) {

}

void UpdateView::DispatchTLVMessage(std::string const & name, codec::TLVMessagePtr const & p, base::Time const&) {
 
}



}  // namespace examples
}  // namespace views
