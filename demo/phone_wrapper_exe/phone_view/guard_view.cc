// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "guard_view.h"

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

GuardView::GuardView() 
  :CTPViewBase("Table") ,
  table_(NULL),
  auto_run_finished_(false),
  ALLOW_THIS_IN_INITIALIZER_LIST(weak_factory_(this)),
  ThreadMessageFilter(true) {
    //report_error_url_str += net::EscapeUrlEncodedData(page_url.spec(), true);
    //读配置文件== 
    //自动运行================
    const base::ListValue* list = common_util::g_pref_service->GetList(prefs::kGuardAutoRun);
    if (!list) {
      DCHECK_RLOG(false);
    } else {
      std::size_t size = list->GetSize();
      for (std::size_t i = 0; i < size; ++i) {
        const base::DictionaryValue* item;
        if (list->GetDictionary(i, &item)) {
          AutoRun info;
          //double money;
          if (item->GetString("socket_name", &info.socket_name) &&
            item->GetString("params", &info.params) &&
            item->GetInteger("rank", &info.rank) &&
            item->GetBoolean("auto-reboot", &info.auto_reboot)) {
              auto_run_[info.socket_name] = info;
          }
        }
      }
    }
   //周期运行===========================
    timing_interval_ = common_util::g_pref_service->GetString(prefs::kGuardTimingInterval);
    DCHECK_RLOG(timing_interval_.size());
    const base::ListValue* list2 = common_util::g_pref_service->GetList(prefs::kGuardTimingRun);
    if (!list2) {
      DCHECK_RLOG(false);
    } else {
      std::size_t size = list2->GetSize();
      for (std::size_t i = 0; i < size; ++i) {
        const base::DictionaryValue* item;
        if (list2->GetDictionary(i, &item)) {
          TimingRun info;
          int32 start;
          int32 over;
          if (item->GetString("socket_name", &info.socket_name) &&
            item->GetString("params", &info.params) &&
            item->GetInteger("start", &start) &&
            item->GetInteger("over", &over) &&
            item->GetBoolean("auto-reboot", &info.auto_reboot)) {
              info.start = base::TimeDelta::FromMinutes(start);
              info.over = base::TimeDelta::FromMinutes(over);
              timing_run_[info.socket_name] = info;
          }
        }
      }
    }
  bool auto_size = auto_run_.size() ? true : false;
  bool timing_size = timing_run_.size() ? true : false;
  //按照目前设计，auto和timing不能同时配置，要么是guard起来，自动运行一次==
  //要么是guard定时启动进程，定时关闭，按照天的间隔来（就是用来收行情的）==
  //但可以都为0
  CHECK((auto_size ^ timing_size) || (!auto_size && !timing_size));

  //没有需要自动运行的进程===
  if (!auto_run_.size()) {
    auto_run_finished_ = true;
  } else {
    //根rank字段排序==
    
    for (auto it = auto_run_.begin(); it != auto_run_.end(); ++it) {
      auto_run_rank_.push_back(std::make_pair(it->second.rank, it->first));
    }
    std::sort(auto_run_rank_.begin(), auto_run_rank_.end());

  }
  //===============================================
  if (timing_run_.size()) {
    for (auto it = timing_run_.begin(); it != timing_run_.end(); ++it) {
      timing_start_rank_.push_back(std::make_pair(it->second.start.ToInternalValue(), it->first));
      timing_over_rank_.push_back(std::make_pair(it->second.over.ToInternalValue(), it->first));
    }
    std::sort(timing_start_rank_.begin(), timing_start_rank_.end());
    std::sort(timing_over_rank_.begin(), timing_over_rank_.end());

    //计算第一个交易日==
    NextInterval();
  }
  //================================================

  auto FillRest =  [&](ProcessRecord &pr) -> void {
    auto it = auto_run_.find(pr.socket_name);
    auto it2 = timing_run_.find(pr.socket_name);
    if (it != auto_run_.end()) {
      pr.status = ProcessRecord::AUTO_WAIT_BOOT;
      pr.is_auto = true;
    } else if (it2 != timing_run_.end()) {
      pr.status = ProcessRecord::AUTO_WAIT_BOOT;
      pr.is_auto = true;
    } else {
      pr.status = ProcessRecord::MANUAL;
      pr.is_auto = false;
    }



  };

  process_vector_.reset(new ProcessVector);


  ProcessRecord pr;
   pr.alive = false;
  pr.name = L"行情进程";
  pr.process_id = 0;
  //pr.socket_name2 = switches::kCommunicateMQAlive;
  //pr.socket_name.assign(switches::kCommunicateMQAlive);
  pr.socket_name.assign("protobuf://cpp/mq-alive");
  
  FillRest(pr);
  process_vector_->push_back(pr);


  pr.alive = false;
  pr.name = L"交易进程";
  pr.process_id = 0;
  pr.socket_name = switches::kCommunicateTUAlive;
  FillRest(pr);
  process_vector_->push_back(pr);

  pr.alive = false;
  pr.name = L"行情交易混合进程";
  pr.process_id = 0;
  pr.socket_name = switches::kCommunicateMQAndTUAlive;
  FillRest(pr);
  process_vector_->push_back(pr);

  pr.alive = false;
  pr.name = L"Test进程";
  pr.process_id = 0;
  pr.socket_name = switches::kCommunicateTestAlive;
  FillRest(pr);
  process_vector_->push_back(pr);

  pr.alive = false;
  pr.name = L"Py行情存储进程";
  pr.process_id = 0;
  pr.socket_name = switches::kCommunicateMQSQLAlive;
  FillRest(pr);
  process_vector_->push_back(pr);

  pr.alive = false;
  pr.name = L"Py控制台进程";
  pr.process_id = 0;
  pr.socket_name = switches::kCommunicateCmdAlive;
  FillRest(pr);
  process_vector_->push_back(pr);

  pr.alive = false;
  pr.name = L"Py控制台交易存储进程混合进程";
  pr.process_id = 0;
  pr.socket_name = switches::kCommunicateCmdAndSQLAlive;
  FillRest(pr);
  process_vector_->push_back(pr);

  pr.alive = false;
  pr.name = L"R策略进程";
  pr.process_id = 0;
  pr.socket_name = switches::kCommunicateMSAlive;
  FillRest(pr);
  process_vector_->push_back(pr);

  pr.alive = false;
  pr.name = L"回测进程";
  pr.process_id = 0;
  pr.socket_name = switches::kCommunicateBacktestingAlive;
  FillRest(pr);
  process_vector_->push_back(pr);

  pr.alive = false;
  pr.name = L"Py回测进程";
  pr.process_id = 0;
  pr.socket_name = switches::kCommunicatePyBacktestingAlive;
  FillRest(pr);
  process_vector_->push_back(pr);

  pr.alive = false;
  pr.name = L"Py行情多点检查合并";
  pr.process_id = 0;
  pr.socket_name = switches::kCommunicatePyMQCheckAlive;
  FillRest(pr);
  process_vector_->push_back(pr);

  pr.alive = false;
  pr.name = L"Py工具箱";
  pr.process_id = 0;
  pr.socket_name = switches::kCommunicatePyToolboxAlive;
  FillRest(pr);
  process_vector_->push_back(pr);


  //开启连接服务器================================
  channel_host_.reset(new channel::ChannelHost());
  channel_host_->RegisterProtobufHandler(switches::kCommunicateCmdAlive, base::Bind(&GuardView::DispatchProtobufMessage, base::Unretained(this)));
  channel_host_->RegisterProtobufHandler(switches::kCommunicateCmdAndSQLAlive, base::Bind(&GuardView::DispatchProtobufMessage, base::Unretained(this)));
  channel_host_->RegisterProtobufHandler(switches::kCommunicateMQSQLAlive, base::Bind(&GuardView::DispatchProtobufMessage, base::Unretained(this)));
  channel_host_->RegisterProtobufHandler(switches::kCommunicateTestAlive, base::Bind(&GuardView::DispatchProtobufMessage, base::Unretained(this)));
  channel_host_->RegisterProtobufHandler(switches::kCommunicateTUAlive, base::Bind(&GuardView::DispatchProtobufMessage, base::Unretained(this)));
  channel_host_->RegisterProtobufHandler(switches::kCommunicateMQAlive, base::Bind(&GuardView::DispatchProtobufMessage, base::Unretained(this)));
  channel_host_->RegisterProtobufHandler(switches::kCommunicateMQAndTUAlive, base::Bind(&GuardView::DispatchProtobufMessage, base::Unretained(this)));
  channel_host_->RegisterProtobufHandler(switches::kCommunicateBacktestingAlive, base::Bind(&GuardView::DispatchProtobufMessage, base::Unretained(this)));
  channel_host_->RegisterProtobufHandler(switches::kCommunicatePyBacktestingAlive, base::Bind(&GuardView::DispatchProtobufMessage, base::Unretained(this)));
  channel_host_->RegisterProtobufHandler(switches::kCommunicatePyMQCheckAlive, base::Bind(&GuardView::DispatchProtobufMessage, base::Unretained(this)));
  
  channel_host_->RegisterTLVHandler(switches::kCommunicateMSAlive, base::Bind(&GuardView::DispatchTLVMessage, base::Unretained(this)));
  //连接检查=============
  std::function<void(std::string const &, bool)> keep_alive_checker = [&](std::string const &name, bool connected) ->void {

      for(auto it = process_vector_->begin(); it != process_vector_->end(); ++it) {
        if (name == it->socket_name && connected != it->alive) {
          it->alive = connected;
          if (connected == false) {
            it->process_id = 0;
            //  [3/9/2014 ken] 如果是自动运行的，tcp断开，就把进程干掉==
            if (timing_run_.find(it->socket_name) != timing_run_.end()) {
              if (it->process_handler != INVALID_HANDLE_VALUE) {
                TerminateProcess(it->process_handler, 0);
                LOG(WARNING)<<"disconnection , kill process "<<it->socket_name;
              }
            }
            it->process_handler = INVALID_HANDLE_VALUE;
          }
          //----------------------------------
          if (it->status != ProcessRecord::MANUAL) {
            if (it->is_auto == true) {
              if (connected == false) {
                //需要自动重启==
                it->status = ProcessRecord::AUTO_WAIT_BOOT;
              } else {
                it->status = ProcessRecord::AUTO_ALIVE;
              }
            } else {
              if (connected == false) {
                //需要自动重启==
                it->status = ProcessRecord::AUTO_DIED;
              } else {
                it->status = ProcessRecord::AUTO_ALIVE;
              }
            }
          }
          //----------------------------------
          if (table_) {
            table_->OnModelChanged();
          }
          return;
        }
      }
    };

  channel_host_->RegisterConnectStatusHandler(switches::kCommunicateCmdAlive, keep_alive_checker);
  channel_host_->RegisterConnectStatusHandler(switches::kCommunicateCmdAndSQLAlive, keep_alive_checker);
  channel_host_->RegisterConnectStatusHandler(switches::kCommunicateMQSQLAlive, keep_alive_checker);
  channel_host_->RegisterConnectStatusHandler(switches::kCommunicateTestAlive, keep_alive_checker);
  channel_host_->RegisterConnectStatusHandler(switches::kCommunicateTUAlive, keep_alive_checker);
  channel_host_->RegisterConnectStatusHandler(switches::kCommunicateMQAlive, keep_alive_checker);
  channel_host_->RegisterConnectStatusHandler(switches::kCommunicateMQAndTUAlive, keep_alive_checker);
  channel_host_->RegisterConnectStatusHandler(switches::kCommunicateMSAlive, keep_alive_checker);
  channel_host_->RegisterConnectStatusHandler(switches::kCommunicateBacktestingAlive, keep_alive_checker);
  channel_host_->RegisterConnectStatusHandler(switches::kCommunicatePyBacktestingAlive, keep_alive_checker);
  channel_host_->RegisterConnectStatusHandler(switches::kCommunicatePyMQCheckAlive, keep_alive_checker);
  
  //===================================
  CommandLine const& command_line = *CommandLine::ForCurrentProcess();
  std::string const process_type = command_line.GetSwitchValueASCII(switches::kProcessType);
  channel_host_->InitializeServer(common_util::ProcessType2ServerName(process_type));
  //CheckAlive();

  if (auto_run_.size()) {
    //有需要自动启动的进程，先对启动进程排序，然后自动启动，15s一个==
/*
    auto_run_task_.reset(
      new VoidVoidCallback(
      base::Bind(&GuardView::InitLaunch, base::Unretained(this), auto_run_rank_)));*/

    CommonThread::PostDelayedTask(CommonThread::FILE,
      FROM_HERE,
      base::Bind(&GuardView::InitLaunch, 
      base::Unretained(this), 
      weak_factory_.GetWeakPtr(),
      auto_run_rank_),
      15 * 1000);
  }


  if (timing_run_.size()) {
    //有需要自动启动的进程，先对启动进程排序，然后自动启动，15s一个==


    //程序启动后15s开始干活，10分钟检查1次===
    timing_monitor_task_.reset(
      new VoidVoidCallback(
      base::Bind(&GuardView::TimingMonitor, base::Unretained(this))));

    CommonThread::PostDelayedTask(CommonThread::UI,
      FROM_HERE,
      timing_monitor_task_->callback(),
      15 * 1000);
  }

}

GuardView::~GuardView() {
/*
  if (auto_run_task_.get()) {
    auto_run_task_->Cancel();
    auto_run_task_.reset(NULL);
  }

  if (timing_run_task_.get()) {
    timing_run_task_->Cancel();
    timing_run_task_.reset(NULL);
  }*/

  if (timing_monitor_task_.get()) {
    timing_monitor_task_->Cancel();
    timing_monitor_task_.reset(NULL);
  }
/*
  if (alive_check_.get()) {
    alive_check_->Cancel();
    alive_check_.reset(NULL);
  }*/

  // Delete the view before the model.
  delete table_;
  table_ = NULL;
}

void GuardView::OnSelectedIndexChanged(Combobox* combobox) {


}

void GuardView::CreateExampleView(View* container) {

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
  columns.push_back(ui::TableColumn(3, L"运行状况",
                                    ui::TableColumn::LEFT, 100));
  /*
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

int GuardView::RowCount() {
  return process_vector_->size();
}

string16 GuardView::GetText(int row, int column_id) {
  
  switch (column_id) {
    case 0: {
		return process_vector_->operator[](row).alive == true ? L"已启动" : L"未启动";
      break;
    }
    case 1: {
      return process_vector_->operator[](row).name;
      break;
    }
    case 2: {
      return base::UintToString16(process_vector_->operator[](row).process_id);
      break;
    }
    case 3: {
      if (process_vector_->operator[](row).status == ProcessRecord::MANUAL) {
        return L"手动";
      } else if (process_vector_->operator[](row).status == ProcessRecord::AUTO_ALIVE) {
        return L"自动（已启动）";
      } else if (process_vector_->operator[](row).status == ProcessRecord::AUTO_WAIT_BOOT) {
        return L"自动（启动中）";
      } else if (process_vector_->operator[](row).status == ProcessRecord::AUTO_DIED) {
        return L"自动（未启动）";
      } else {
        DCHECK_RLOG(false);
      }
      
      break;
    }              
  }
  

  return string16();
}

string16 GuardView::GetColumnText(int id, phone_module::LevelQuotationInfoBig const & info) {
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

gfx::ImageSkia GuardView::GetIcon(int row) {
  //DCHECK(false);
  //return gfx::ImageSkia();
  if ((uint32)row < process_vector_->size()) {
    return process_vector_->operator[](row).alive == true ? *alive_ : *die_;
  }

  return gfx::ImageSkia();
}

void GuardView::SetObserver(ui::TableModelObserver* observer) {}

void GuardView::OnSelectionChanged() {

}

void GuardView::OnDoubleClick() {
  uint32 index = table_->FirstSelectedRow();
  if (index < process_vector_->size()) {
    DCHECK_RLOG((process_vector_->operator[](index).alive == false && process_vector_->operator[](index).process_handler == INVALID_HANDLE_VALUE) ||
      (process_vector_->operator[](index).alive == true && process_vector_->operator[](index).process_handler != INVALID_HANDLE_VALUE));

    if (process_vector_->operator[](index).alive == false &&
      auto_run_finished_ == true) {
      Launch(index);
    }
  }

}

void GuardView::InitLaunch(base::WeakPtr<GuardView> weak_ptr, std::vector<std::pair<uint64, std::string>> const & vec) {
  //这个函数就在file上跑了。启动一个，睡15s，简化代码逻辑===
  CommonThread::CurrentlyOn(CommonThread::FILE);

  for (auto it = vec.begin(); it != vec.end(); ++it) {
    CommonThread::PostTask(CommonThread::UI,
      FROM_HERE,
      base::Bind(&GuardView::LaunchWrapper, weak_ptr, it->second));

    Sleep(1000 * 15);
    
  }
  
  CommonThread::PostTask(CommonThread::UI,
    FROM_HERE,
    base::Bind(&GuardView::AutoRunFinish, weak_ptr));

}

void GuardView::AutoRunFinish() {
  LOG(INFO)<<"GuardView::AutoRunFinish()";
  auto_run_finished_ = true;
}

void GuardView::TimingLaunchOrTerminate(base::WeakPtr<GuardView> weak_ptr, std::vector<std::pair<uint64, std::string>> const & vec, bool start) {
  CommonThread::CurrentlyOn(CommonThread::FILE);

  if (start) {
    for (auto it = vec.begin(); it != vec.end(); ++it) {
      CommonThread::PostTask(CommonThread::UI,
        FROM_HERE,
        base::Bind(&GuardView::LaunchWrapper, weak_ptr, it->second));

      Sleep(1000 * 15);

    }
  } else {
    for (auto it = vec.begin(); it != vec.end(); ++it) {
      CommonThread::PostTask(CommonThread::UI,
        FROM_HERE,
        base::Bind(&GuardView::Terminate, weak_ptr, it->second));

      Sleep(1000 * 15);

    }
  }
}

void GuardView::LaunchWrapper(std::string const & socket_name) {
  for (uint32 i = 0; i < process_vector_->size(); i++) {
    if (process_vector_->operator[](i).socket_name == socket_name) {
      Launch(i);
      break;
    }
  }
}

void GuardView::Launch(uint32 const index) {
  if (process_vector_->size() > index) {
    std::string socket_name = process_vector_->operator[](index).socket_name;
    LOG(INFO)<<"GuardView::Launch =====>"<<socket_name;

    if (socket_name == switches::kCommunicateMQSQLAlive) {
      std::string cmd = common_util::g_pref_service->GetString(prefs::kMQPyPathAndCmd);
      DCHECK_RLOG(cmd.size());
      LOG(INFO) << "Launching: " << cmd;
      base::LaunchProcess(ASCIIToWide(cmd), base::LaunchOptions(), &process_vector_->operator[](index).process_handler);
    } else if (socket_name == switches::kCommunicateCmdAndSQLAlive) {
      std::string cmd = common_util::g_pref_service->GetString(prefs::kTUPyPathAndCmd);
      DCHECK_RLOG(cmd.size());
      LOG(INFO) << "Launching: " << cmd;
      base::LaunchProcess(ASCIIToWide(cmd), base::LaunchOptions(), &process_vector_->operator[](index).process_handler);
    } else if (socket_name == switches::kCommunicatePyBacktestingAlive) {
      std::string cmd = common_util::g_pref_service->GetString(prefs::kBacktestingPyPathAndCmd);
      DCHECK_RLOG(cmd.size());
      LOG(INFO) << "Launching: " << cmd;
      base::LaunchProcess(ASCIIToWide(cmd), base::LaunchOptions(), &process_vector_->operator[](index).process_handler);
    } else if (socket_name == switches::kCommunicatePyMQCheckAlive) {
      std::string cmd = common_util::g_pref_service->GetString(prefs::kMQCheckPyPathAndCmd);
      DCHECK_RLOG(cmd.size());
      LOG(INFO) << "Launching: " << cmd;
      base::LaunchProcess(ASCIIToWide(cmd), base::LaunchOptions(), &process_vector_->operator[](index).process_handler);
    } else if (socket_name == switches::kCommunicatePyToolboxAlive) {
      std::string cmd = common_util::g_pref_service->GetString(prefs::kPyToolboxPyPathAndCmd);
      DCHECK_RLOG(cmd.size());
      LOG(INFO) << "Launching: " << cmd;
      //toolbox不维持alive状态
      HANDLE process_handler;
      base::LaunchProcess(ASCIIToWide(cmd), base::LaunchOptions(), &process_handler);
    } else {
      FilePath exe_path;
      PathService::Get(base::FILE_EXE, &exe_path);
      CommandLine cmd_line(exe_path);
      cmd_line.AppendSwitchASCII(switches::kProcessType, common_util::SocketName2ProcessType(socket_name));
      if (auto_run_.size()) {
        auto it = auto_run_.find(socket_name);
        if (it != auto_run_.end() && it->second.params.size()) {
          cmd_line.AppendSwitch(it->second.params);
        }
      } else if (timing_run_.size()) {
        auto it = timing_run_.find(socket_name);
        if (it != timing_run_.end() && it->second.params.size()) {
          cmd_line.AppendSwitch(it->second.params);
        }
      }

      LOG(INFO) << "Launching: " << cmd_line.GetCommandLineString();
      base::LaunchProcess(cmd_line, base::LaunchOptions(), &process_vector_->operator[](index).process_handler);
    }
  } else {
    DCHECK_RLOG(false);
  }
}

void GuardView::Terminate(std::string const & socket_name) {
  LOG(INFO)<<"GuardView::Terminate =====>"<<socket_name;

  for(auto it = process_vector_->begin(); it != process_vector_->end(); ++it) {
    if (socket_name == it->socket_name && true == it->alive) {
      HANDLE handle = OpenProcess(PROCESS_TERMINATE, FALSE, it->process_id);
      TerminateProcess(handle, 0);
      return;
    }
  }
}


void GuardView::TimingMonitor() {
/*
  if (timing_run_task_.get()) {
    DCHECK_RLOG(false);
    timing_run_task_->Cancel();
    timing_run_task_.reset(NULL);
  }*/
  //那我要如何知道现在没有一个task在运行？？？====

  if (TimingNeedStart()) {
/*
    timing_run_task_.reset(
      new VoidVoidCallback(
      base::Bind(&GuardView::TimingLaunchOrTerminate, base::Unretained(this), auto_run_rank_, true)));*/

    CommonThread::PostTask(CommonThread::FILE,
      FROM_HERE,
      base::Bind(&GuardView::TimingLaunchOrTerminate, 
      base::Unretained(this), 
      weak_factory_.GetWeakPtr(),
      timing_start_rank_, true));
  } else if (TimingNeedStop()) {
/*
    timing_run_task_.reset(
      new VoidVoidCallback(
      base::Bind(&GuardView::TimingLaunchOrTerminate, base::Unretained(this), auto_run_rank_, false)));*/

    CommonThread::PostTask(CommonThread::FILE,
      FROM_HERE,
      base::Bind(&GuardView::TimingLaunchOrTerminate, 
      base::Unretained(this), 
      weak_factory_.GetWeakPtr(),
      timing_over_rank_, false));
  }

  timing_monitor_task_.reset(
    new VoidVoidCallback(
    base::Bind(&GuardView::TimingMonitor, base::Unretained(this))));

  CommonThread::PostDelayedTask(CommonThread::UI,
    FROM_HERE,
    timing_monitor_task_->callback(),
    180 * 1000);
}

base::TimeDelta GuardView::NextStartTime(base::TimeDelta const &start) {
  return base::TimeDelta();
}

base::TimeDelta GuardView::NextOverTime(base::TimeDelta const &over) {
  return base::TimeDelta();
}

bool GuardView::TimingNeedInner(bool const start, base::TimeDelta const b_start_diff, base::TimeDelta const b_end_diff) {
  base::Time now = base::Time::Now();
  base::Time::Exploded e;
  now.LocalExplode(&e);

  base::Time base_diff;
  uint32 m = b_start_diff.ToInternalValue() / base::Time::kMicrosecondsPerMinute;
  uint32 m2 = b_end_diff.ToInternalValue() / base::Time::kMicrosecondsPerMinute;

  std::string output2 = base::StringPrintf("%d-%d-%d %d:%d (%d,%d)", e.year, e.month, e.day_of_month, e.hour, e.minute, m, m2);
  LOG(INFO)<<"TimingNeedInner daily==== "<<output2;

  if (timing_interval_ == "1d") {
    e.hour = 0;
    e.minute = 0;
    e.second = 0;
  } else if (timing_interval_ == "1h") {
    e.minute = 0;
    e.second = 0;
  } else {
    CHECK(false);
  }

  base_diff = base::Time::FromLocalExploded(e);
  if ((start && base_diff + b_start_diff < now && base_diff + b_end_diff > now) ||
    (!start && base_diff + b_end_diff < now)) {

    std::string output = base::StringPrintf("%d-%d-%d %d, %d-%d", e.year, e.month, e.day_of_month, e.hour, m, m2);
    LOG(INFO)<<"TimingNeedInner run==== "<<output;
    return true;
  }

  return false;
}

bool GuardView::TimingNeedStart() {
  //启动的标准是，mq不是alive的，而且时间过了mq要启动的时间==
  //这个业务逻辑就是为了周期启动程序接收行情和测试交易状况，目前不考虑更加通用的情况==
  
  if (!channel_host_->HasConnection(switches::kCommunicateMQAlive)) {
    auto it = timing_run_.find(switches::kCommunicateMQAlive);
    if (it != timing_run_.end()) {
      return TimingNeedInner(true, it->second.start, it->second.over);
    }
  }

  return false;
}

bool GuardView::TimingNeedStop() {
  if (channel_host_->HasConnection(switches::kCommunicateMQAlive)) {
    auto it = timing_run_.find(switches::kCommunicateMQAlive);
    if (it != timing_run_.end()) {
      return TimingNeedInner(false, it->second.start, it->second.over);
    }
  }

  return false;
}

void GuardView::NextInterval() {
#if 0
  //根据已经使用过的交易日，计算下一个交易日，并把这个计算出来的交易日加入到已经使用过的队列中==
  used_trading_day_;
  timing_interval_;

  if (timing_interval_ == "1d") {
    //计算下一个交易日的天，天的0点0分0秒存入used_trading_day_
    if (used_trading_day_.size()) {
      used_trading_day_.push_back(NextDay(used_trading_day_.back()));
    } else {
      //程序刚启动，如果是12点之前，base是今天零点，如果12点之后，base是明天零点==
      base::Time now = base::Time::Now();

      used_trading_day_.push_back(base::Time::)
    }
  } else if (timing_interval_ == "5m") {
    //用于测试，计算now开始的下个5分钟，存入used_trading_day_
  } else {
    CHECK(false);
  }
#endif
}

void GuardView::OnMiddleClick() {}

void GuardView::OnKeyDown(ui::KeyboardCode virtual_keycode) {}

void GuardView::OnTableViewDelete(TableView* table_view) {}

void GuardView::OnTableView2Delete(TableView2* table_view) {}

void GuardView::ButtonPressed(Button* sender, const ui::Event& event) {

}



void GuardView::Observe(int type,
  const content::NotificationSource& source,
  const content::NotificationDetails& details) {
}

bool GuardView::OnMessageReceived(IPC::Message const & msg) {


  CommonThread::ID thread;
  
  if (!CommonThread::GetCurrentThreadIdentifier(&thread) &&
    OverrideThreadForMessage(msg, &thread)) {

      CommonThread::PostTask(
        thread, FROM_HERE,
        base::Bind(base::IgnoreResult(&GuardView::OnMessageReceived),
        base::Unretained(this), msg));

      return true;
  }


  if (msg.routing_id() == MSG_ROUTING_CONTROL) {
    // Dispatch control messages.
    bool msg_is_ok = false;
/*
    IPC_BEGIN_MESSAGE_MAP_EX(GuardView, msg, msg_is_ok)
      IPC_MESSAGE_HANDLER(CTP_MQLevel2UI, OnMQ)
      IPC_MESSAGE_HANDLER(CTP_MQLevelParam, OnInitParam)
      IPC_MESSAGE_HANDLER(CTP_MQ_Login_Success_2UI, OnLoginSuccess)
      IPC_MESSAGE_HANDLER(CTP_MQ_Logout_2UI, OnDisConnect)
      IPC_END_MESSAGE_MAP_EX()*/

      if (msg_is_ok) {
        return true;
      }
  }

  // Dispatch incoming messages to the appropriate RenderView/WidgetHost.
  //RenderWidgetHost* rwh = render_widget_hosts_.Lookup(msg.routing_id());

  return false; 

}


void GuardView::OnLoginSuccess() {

}


void GuardView::OnDisConnect() {

}


//TaskManagerTableModel
void GuardView::EnsureVisible() {
  
}

void GuardView::OnMQ(int32 const type, int32 const id, base::Time const & time, PointerWrapper<phone_module::LevelQuotationInfoBig> const & p) {
    // table_->OnModelChanged();
    // EnsureVisible();
}

void GuardView::OnInitParam(std::string const & s) {

}


bool GuardView::OverrideThreadForMessage(IPC::Message const& message,
  CommonThread::ID* thread) {
    if (message.type() == CTP_CONSOLE_Command::ID || 
      message.type() == CTP_CONSOLE_CommandProtoBuf::ID) {
        *thread = CommonThread::UI;
        return true;
    }

    return false;
}

void GuardView::Selected() {

}


void GuardView::ShowContextMenuForView(views::View* source,
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

void GuardView::ExecuteCommand(int id) {
  //tab_table_->SetColumnVisibility(id, !tab_table_->IsColumnVisible(id));
}


void GuardView::DispatchProtobufMessage(std::string const & name, codec::MessagePtr const & p, base::Time const&) {
  if (p->GetTypeName() == "ctp.guard.ReportPid") {
    ctp::guard::ReportPid const* report = static_cast<ctp::guard::ReportPid const*>(p.get());
    for(auto it = process_vector_->begin(); it != process_vector_->end(); ++it) {
      if (name == it->socket_name) {
        it->process_id = report->process_id();
        if (table_) {
          table_->OnModelChanged();
        }
        return;
      }
    }
  } else if (p->GetTypeName() == "ctp.guard.CommonCmd") {
    ctp::guard::CommonCmd const* cmd = static_cast<ctp::guard::CommonCmd const*>(p.get());
    if (cmd->str1() == "mergemq") {
      LOG(INFO)<<"mergemq begin=======================";
      LaunchWrapper(switches::kCommunicatePyMQCheckAlive);
    }
  }
  

}

void GuardView::DispatchTLVMessage(std::string const & name, codec::TLVMessagePtr const & p, base::Time const&) {
 
}

void GuardView::CheckAlive() {
  
}


}  // namespace examples
}  // namespace views
