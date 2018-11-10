﻿/**
* CTPModule.cpp
* this file implement the im module
*
* @version 1.0 (06 ,04, 2008)
* @author reinhardt ken
*
*/



#include "phone_module.h"

#include <iostream>
#include "time.h"
#include <xstring>
// build on win10, solve build error [1/24/2016 liuqingping]
//#include <playsoundapi.h>
#include <MMSystem.h>

#include "base/command_line.h"
#include "base/logging.h"
#include "base/process_util.h"
#include "base/string_number_conversions.h"
#include "base/string_util.h"
#include "base/string_split.h"
#include "base/path_service.h"
#include "base/string_tokenizer.h"
#include "base/utf_string_conversions.h"
#include "base/sys_string_conversions.h"

#include "content/public/browser/notification_details.h"
#include "content/public/browser/notification_source.h"
#include "content/browser/notification_service_impl.h"
#include "content/public/common/content_switches.h"



#include "common.h"
#include "paths.h"

#include "phone_common/include/ctp_define.h"
#include "phone_common/include/imodulemanager.h"
#include "phone_common/include/ctp_notification_types.h"
#include "phone_common/common/pref_service.h"
#include "phone_common/common/common_common.h"
#include "phone_common/common/common_util.h"



#include "../sqlite/persistent_store_base_ex.h"
#include "../sqlite/tableset_order_footprints.h"


#include "../../protobuf/out/apk_protomsg.pb.h"

#include "phone_common/channel/channel_host.h"

#include "phone_common/channel/codec/protobuf_codec.h"
#include "phone_common/channel/codec/tlv_codec.h"
#include "phone_common/channel/codec/tlv_define.h"

#include "../adbtool/adb_interface.h"
#include "../adbtool/adb_py_interface.h"

namespace {
  template<typename T>
  bool ReadValue(std::string const &, T*) {
    DCHECK_RLOG(false);
    return false;
  }

  template<>
  bool ReadValue(std::string const &vstring, double* v) {
    return base::StringToDouble(vstring, v);
  }

  template<>
  bool ReadValue(std::string const &vstring, int* v) {
    return base::StringToInt(vstring, v);
  }

  template<>
  bool ReadValue(std::string const &vstring, uint32* v) {
    return base::StringToUint(vstring, v);
  }

  template<>
  bool ReadValue(std::string const &vstring, std::string* v) {
    *v = vstring;
    return true;
  }

  template<typename T>
  bool ReadJsonList(base::ListValue const* p, std::map<std::string, T> &result) {
    if (p) {
      std::size_t size_list = p->GetSize();
      for (std::size_t i = 0; i < size_list; ++i) {
        std::string out;
        if (p->GetString(i, &out)) {
          std::size_t index = out.find(":");
          if (index != std::string::npos) {
            std::string k = out.substr(0, index);
            std::string vstring = out.substr(index + 1, out.size());
            T v;
            if (ReadValue<T>(vstring, &v)) {
              result[k] = v;
            } else {
              DCHECK_RLOG(false);
              return false;
            }
          } else {
            DCHECK_RLOG(false);
            return false;
          }
        } else {
          DCHECK_RLOG(false);
          return false;
        }
      }
      return true;
    }
    return false;
  }

}



namespace phone_module {
 
  void RegisterLocalState(PrefService* local_state) {
    
    
  }


  /////////////////////////////////////////////////////////////////////////////////
  wchar_t const * const g_module_name=L"IM_MODULE";
  //CTPModule * g_ctp_module;



  
  CTPModule::CTPModule()
    :ThreadMessageFilter(true),
    python_connected_(false),
    routing_id_seed_(0) {

    LOG(INFO)<<"CTPModule::CTPModule begin";
    LOG(INFO)<<"===========(VERSION="<<version::kVersion<<")================";

    LOG(INFO)<<WideToUTF8(base::SysNativeMBToWide("log test!!!")).c_str();
    LOG(INFO)<<WideToUTF8(base::SysNativeMBToWide("日志测试！！！")).c_str();

    //  [7/14/2016 liuqingping]
    // test message id
 
    base::TimeDelta delta = base::TimeDelta::FromSeconds(1);
    LOG(INFO)<<delta.ToInternalValue();
    LOG(INFO)<<"CTP_START message id  "<<CTP_START::ID;
    LOG(INFO)<<"CTP_CONSOLE_RoutedCommandRsp message id  "<<CTP_CONSOLE_RoutedCommandRsp::ID;
    LOG(INFO)<<"CTP_LAST message id  "<<CTP_LAST::ID;

    CommandLine const& command_line = *CommandLine::ForCurrentProcess();
    std::string const process_type = command_line.GetSwitchValueASCII(switches::kProcessType);

    //PathService
    RegisterPathProvider();
    //RegisterTradeUnitPathProvider();


    local_state_ = common_util::g_pref_service;
    RegisterLocalState(local_state());
    g_pref_service = local_state();


    if (PathService::Get(base::DIR_EXE, &exe_dir_)) {
      apk_dir_ = exe_dir_.value();
      apk_dir_.append(L"\\ctp_data\\apk\\");
    } else {
      DCHECK(false);
    }
    
    //下面这两个东西很多时候互斥，目前策略二选一
    //pyadb
    py_adb_.reset(new PythonAdbInterface(this));
    //native adb
    //phone begin here
    /*CAdbInterface::StaticInit();
    adb_.reset(new CAdbInterface);
    ADB = adb_.get();*/
    
    //开启连接服务器================================
    channel_host_.reset(new channel::ChannelHost());
    
	//业务
    channel_host_->RegisterProtobufHandler(switches::kCommunicatePyUpdateApk, base::Bind(&CTPModule::DispatchProtobufMessage, base::Unretained(this)));
	//报活
	//channel_host_->RegisterProtobufHandler(switches::kCommunicatePyUpdateApkAlive, base::Bind(&CTPModule::DispatchProtobufMessage, base::Unretained(this)));
    server_port_ = -1;
    channel::ServerResult result = channel_host_->InitializeServer(::prefs::kCheckUpdateApkList);
    if (result.first == true) {
      server_port_ = result.second;
    }
    //================
    
    channel_host_->RegisterConnectStatusHandler(switches::kCommunicatePyUpdateApk,
      [&, process_type](std::string const &name, bool connected) ->void {
        if (true == connected) {
          //发送config信息到py端
          apk::Command * cmd = new apk::Command;
          cmd->set_cmd(command::kPYConfig);
          cmd->set_cmd_no(cmd_no());
          cmd->add_param(WideToUTF8(exe_dir_.value()));
            
          codec::MessagePtr ptr(cmd);
          current_cmd_no_set_.insert(cmd->cmd_no());
          channel_host_->SendProtobufMsg(switches::kCommunicatePyUpdateApk, ptr);
			    //通知建立连接
          DeviceData * date = new DeviceData();
          date->info = L"包管理模块-已连接";
          PointerWrapper<DeviceData> tmp(date);
          ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::UI,
            new L2U_ModuleUpdate(tmp));


          if (py_adb_.get()) {
            py_adb_->StartScan();
          }

		} else {
			//通知断开连接
          DeviceData * date = new DeviceData();
          date->info = L"包管理模块-未连接";
          PointerWrapper<DeviceData> tmp(date);
          ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::UI,
            new L2U_ModuleUpdate(tmp));

          if (py_adb_.get()) {
            py_adb_->StopScan();
            //当使用pyadb的时候，如果连接断开，即不能工作，等同于没有设备连接
            DevicesList * data = new DevicesList;
            PointerWrapper<DevicesList> tmp(data);
            ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::UI,
              new L2U_DevicesList(tmp));
          }
		  }
    }
    );
    //======================
    RlangHeartbeat();


	

   
    //周知各个模块，初始化已经完成===
    content::NotificationService::current()->Notify(
      phone_module::NOTIFICATION_CTP_INIT_FINISHED,
      content::NotificationService::AllSources(),
      content::NotificationService::NoDetails());
    

    LOG(INFO)<<"CTPModule::CTPModule over";
  }

  CTPModule::~CTPModule() {
    CloseRlangHeartbeat();

    
    g_pref_service = NULL;
    //销毁所有的tu

  }


  void CTPModule::Load(IModuleManager *const p) {
    module_manager_ = p;


    //Stat().StateStep(Statistics::STATE_STEP_LOAD);

    //Notify(IM_NOTIFY_LOAD);
  }

  void CTPModule::Unload() {  
    //Notify(IM_NOTIFY_UNLOAD);// nothing do now...

  }

  ModuleID CTPModule::GetModuleID() {
    return MODULE_ID_CTP;
  }

  wchar_t const * const CTPModule::GetModuleName() {
    return g_module_name;
  }




  bool CTPModule::OnMessageReceived(IPC::Message const & msg) {
    if (msg.routing_id() == MSG_ROUTING_CONTROL) {
      // Dispatch control messages.
      bool msg_is_ok = false;
      IPC_BEGIN_MESSAGE_MAP_EX(CTPModule, msg, msg_is_ok)


        IPC_MESSAGE_HANDLER(CTP_QUIT_R, OnQuitR)
        IPC_MESSAGE_HANDLER(CTP_PlaySound, OnPlaySound)
        IPC_MESSAGE_HANDLER(U2L_ScanDevices, ScanDevices)
		    IPC_MESSAGE_HANDLER(U2L_Refresh, OnRefresh)
		    IPC_MESSAGE_HANDLER(U2L_DeviceChange, OnDeviceChange)
		    IPC_MESSAGE_HANDLER(U2L_GetLocalInstallApkList, OnGetLocalInstallApkList)
        IPC_MESSAGE_HANDLER(U2L_GetNetInstallApkList, OnGetNetInstallApkList)
		    IPC_MESSAGE_HANDLER(U2L_ApkInstallCmd, OnInstallApkList)
		    IPC_MESSAGE_HANDLER(U2L_CheckUpdateApkList, OnCheckUpdateApkList)
        IPC_MESSAGE_HANDLER(U2L_RemoveLocalInstallApkList, OnRemoveLocalInstallApkList)
        IPC_MESSAGE_HANDLER(U2L_LaunchPY, OnLaunchPY)
        
		  
		  
        IPC_MESSAGE_UNHANDLED(msg_is_ok = false)
        IPC_END_MESSAGE_MAP_EX()

        if (msg_is_ok) {
          return true;
        } 
    } else {
      //if (MQ()->routing_id() == msg.routing_id()) {
      //  return MQ()->OnMessageReceived(msg);
      //} else {
      //  // Dispatch incoming messages to the appropriate RenderView/WidgetHost.
      //  TradeUnit* tu = GetTradeUnit(msg.routing_id());
      //  if (tu) {
      //    return tu->OnMessageReceived(msg);
      //  } else {
      //    DCHECK_RLOG(false);
      //  }
      //}
    }
 
    return false; 
  
  }

  


  void CTPModule::OnQuitR() {
    //把task丢到file线程执行
    //CommonThread::PostTask(CommonThread::FILE,
    //  FROM_HERE,
    //  base::Bind(base::IgnoreResult(&ctp_bridge::RBridgeWorkOnFileQuitR)));
  }


  void CTPModule::DispatchProtobufMessage(std::string const & name, codec::MessagePtr const & p, base::Time const&) {
    if (name == switches::kCommunicatePyUpdateApkAlive) {
      DCHECK(false);
    } else if (name == switches::kCommunicatePyUpdateApk) {
      if (p->GetTypeName() == "apk.CommandProgress") {
        apk::CommandProgress const* progress = static_cast<apk::CommandProgress const*>(p.get());
        if (std::string(command::kCheckNetPackageList) == progress->cmd() ||
          std::string(command::kRemoveLocalPackageList) == progress->cmd()) {
          ApkUpdateInfo * data = new ApkUpdateInfo(progress->code());
          //auto result = StringPrintf(L"成功：信息：%ls", UTF8ToWide(progress->info()).c_str());
          data->op = UTF8ToWide(progress->info(0));
          if (progress->info_size() > 1) {
            data->info = UTF8ToWide(progress->info(1));
          }
          if (progress->info_size() > 2) {
            data->package_name = UTF8ToWide(progress->info(2));
          }
          PointerWrapper<ApkUpdateInfo> tmp(data);
          ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::UI,
            new L2U_ApkUpdateInfo(tmp));
        } else if (std::string(command::kRemoveLocalPackageList) == progress->cmd()) {
        } else if (std::string(command::kPyAdbInstallApk) == progress->cmd()) {
          ApkIRStatus * data = new ApkIRStatus(UTF8ToWide(progress->info(0)));
          data->error_code = progress->code();
          data->stage = UTF8ToWide(progress->info(1));
          data->package_name = UTF8ToWide(progress->info(2));
          data->percent = UTF8ToWide(progress->info(3));
          data->op = L"全新/覆盖安装";
          if (data->error_code == ERROR_CODE_OK) {
            data->result = L"成功";
          } else {
            data->result = L"失败";
          }
          

          PointerWrapper<ApkIRStatus> tmp(data);
          ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::UI,
            new L2U_ApkIRStatus(tmp));

        }
      } else if (p->GetTypeName() == "apk.CommandResponse") {
        apk::CommandResponse const* response = static_cast<apk::CommandResponse const*>(p.get());
        
      } else if (p->GetTypeName() == "apk.ApkList") {
        apk::ApkList const* apk_list = static_cast<apk::ApkList const*>(p.get());
        if (std::string(command::kGetLocalPackageList) == apk_list->head().cmd()) {
          if (apk_list->head().code() == ERROR_CODE_OK) {
            std::vector< ApkInstallInfo> * data = new std::vector< ApkInstallInfo>;
            for (auto i = 0; i < apk_list->apk_list_size(); ++i) {
              ApkInstallInfo info;
              info.package_name = UTF8ToWide(apk_list->apk_list(i).apk_name());
              info.md5 = UTF8ToWide(apk_list->apk_list(i).md5());
              info.name = UTF8ToWide(apk_list->apk_list(i).name());
              info.brief = UTF8ToWide(apk_list->apk_list(i).brief());
              info.price = apk_list->apk_list(i).price();
              info.type = (PACKAGE_TYPE)apk_list->apk_list(i).type();
              data->push_back(info);
            }
            PointerWrapper<std::vector< ApkInstallInfo>> tmp(data);
            ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::UI,
              new L2U_ApkInstallInfo(tmp));
          } else {
            DCHECK(false);
          }
        }
      } else if (p->GetTypeName() == "apk.DevicesList") {
        apk::DevicesList const* device_list = static_cast<apk::DevicesList const*>(p.get());
        if (std::string(command::kPyAdbScanDevices) == device_list->head().cmd()) {
          if (device_list->head().code() == ERROR_CODE_OK) {
            DevicesList * data = new DevicesList;
            for (auto i = 0; i < device_list->devices_list_size(); ++i) {
              AdbDevice info;
              info.serial_no = UTF8ToWide(device_list->devices_list(i).serial_no());
              info.device = UTF8ToWide(device_list->devices_list(i).device());
              info.product = UTF8ToWide(device_list->devices_list(i).product());
              info.model = UTF8ToWide(device_list->devices_list(i).model());
              data->push_back(info);
            }
            PointerWrapper<DevicesList> tmp(data);
            ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::UI,
              new L2U_DevicesList(tmp));
          } else {
            DCHECK(false);
          }
        }
      }

    } else {
        DCHECK_RLOG(false);
      }
    }


  void CTPModule::DispatchTLVMessage(std::string const & name, codec::TLVMessagePtr const & p, base::Time const&) {
    if (name == switches::kCommunicateMS) {
      content::NotificationService::current()->Notify(
          phone_module::NOTIFICATION_CTP_MS_ACTION_INFO_FROM_R,
          content::Source<CTPModule>(this),
          content::Details<codec::TLVObjectVector const>(p.get()));
    } else {
      DCHECK_RLOG(false);
    }
    
  }

  void CTPModule::SendTLVMsg(codec::TLVMessagePtr const & p) {
    channel_host_->SendTLVMsg(switches::kCommunicateMS, p);
  }

  void CTPModule::SendRlangHeartbeat() {
    codec::TLVObject hb = codec::TLVObject::CreateTLVStringObject("rlang-heartbeat");
    codec::TLVMessagePtr ptr(new codec::TLVObjectVector(1));
    ptr->operator[](0) = hb;
    //LOG(INFO)<<"CTPModule::SendRlangHeartbeat "<<ptr->size();
    channel_host_->SendTLVMsg(switches::kCommunicateMS, ptr);
  }

  void CTPModule::RlangHeartbeat() {
    SendRlangHeartbeat();

    keep_connect_rlang_.reset(
      new VoidVoidCallback(
      base::Bind(&CTPModule::RlangHeartbeat, base::Unretained(this))));
    CommonThread::PostDelayedTask(CommonThread::CTP,
      FROM_HERE,
      keep_connect_rlang_->callback(),
      10 * 1000);
  }


  void CTPModule::CloseRlangHeartbeat() {
    if (keep_connect_rlang_.get()) {
      keep_connect_rlang_->Cancel();
      keep_connect_rlang_.reset(NULL);
    }
  }

  void CTPModule::Observe(int type,
    const content::NotificationSource& source,
    const content::NotificationDetails& details) {

  }

  //void CTPModule::OnLoginAll() {
  //  TradeMapIt it = trade_map_.begin();
  //  for (; it != trade_map_.end(); ++it) {
  //    it->second->LoginFromLoginAll();
  //  }
  //}

  void CTPModule::OnPlaySound(uint32 const v) {
    FilePath dir_path;
    PathService::Get(base::DIR_EXE, &dir_path);

    if (v == PLAYSOUND_LOGIN) {
      dir_path = dir_path.Append(L"login.wav");
      PlaySound(dir_path.value().c_str(), NULL, SND_FILENAME | SND_ASYNC);
    } else if (v == PLAYSOUND_LOGOUT) {
      dir_path = dir_path.Append(L"logout.wav");
      PlaySound(dir_path.value().c_str(), NULL, SND_FILENAME | SND_ASYNC);
    } else if (v == PLAYSOUND_ORDER) {
      dir_path = dir_path.Append(L"order.wav");
      PlaySound(dir_path.value().c_str(), NULL, SND_FILENAME | SND_ASYNC);
    } else if (v == PLAYSOUND_WITHDRAW) {
      dir_path = dir_path.Append(L"withdraw.wav");
      PlaySound(dir_path.value().c_str(), NULL, SND_FILENAME | SND_ASYNC);
    } else if (v == PLAYSOUND_CLINCH) {
      dir_path = dir_path.Append(L"clinch.wav");
      PlaySound(dir_path.value().c_str(), NULL, SND_FILENAME | SND_ASYNC);
    } else if (v == PLAYSOUND_BT_FINISH) {
      dir_path = dir_path.Append(L"bt_finish.wav");
      PlaySound(dir_path.value().c_str(), NULL, SND_FILENAME | SND_ASYNC);
    }

  }


  //phone begin here
  void CTPModule::InitConnectedDevice() {
	  if (adb_.get()) {
		  adb_->SetPrintkLevel(KERN_ALL);
	  }
  }

  void CTPModule::ScanDevices() {
    if (py_adb_.get()) {
      py_adb_->ScanDevices();
    }
    //if (adb_.get()) {
    //  CString strTitle;
    //  adb_->Init();

    //  for (int i = 0; i < 3; i++)
    //  {
    //    adb_->GetDevices();
    //    if (adb_->IsMultiDevConnected()) {
    //      //多个设备，默认选第一个
    //      adb_->SelectDevice(0);
    //      //CDialogSelectDevice  sddlg(g_pToolDlg);
    //      //sddlg.DoModal();
    //      //Sleep(1000);
    //    }
    //    if (adb_->IsConnected())
    //    {
    //      InitConnectedDevice();
    //      break;
    //    }
    //    Sleep(100);
    //  }

    //  if (adb_->IsConnected())
    //  {
    //    strTitle = TEXT("Android Dev: ");
    //    strTitle += adb_->GetModel();
    //    strTitle.AppendFormat(TEXT("(%s)"), adb_->GetSerialNo());
    //    strTitle += TEXT(" Connected");
    //    if (adb_->IsRooted()) {
    //      strTitle += TEXT("(Root)");
    //    }
    //  } else
    //  {
    //    strTitle = TEXT("Disconnected, Please Connect Android Device!");
    //  }

    //  //通知ui=================================
    //  if (adb_->IsConnected()) {
    //    ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::UI,
    //      new L2U_AdbInfo("", std::wstring(strTitle.GetBuffer())));
    //    strTitle.ReleaseBuffer();

    //    //DeviceData * date = new DeviceData(adb_->GetSerialNo2());
    //    //PointerWrapper<DeviceData> tmp(date);
    //    //ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::UI,
    //    //  new L2U_DeviceUpdate(tmp));

    //    //做一些其余的动作，先放在这里做
    //    CommonThread::PostTask(CommonThread::CTP,
    //      FROM_HERE,
    //      base::Bind(&CTPModule::OnGetPackageList, base::Unretained(this), L""));
    //  } else {
    //    ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::UI,
    //      new L2U_AdbInfo("", std::wstring(L"No devices")));
    //  }
    //}
	  

  }

  void CTPModule::OnGetPackageList(std::wstring const &) {
    if (adb_.get()) {
      vector<ANDROID_PACKAGE_T> out;
      vector<ANDROID_PACKAGE_T> thirdpart_out;
      adb_->GetPackages(LPPARAM_FILE | LPPARAM_UID, out);
      adb_->GetPackages(LPPARAM_THIRDPARTY | LPPARAM_UID, thirdpart_out);

      vector<ANDROID_PACKAGE_T>::iterator ait = out.begin();
      vector<ANDROID_PACKAGE_T>::const_iterator subcit;
      for (; ait != out.end(); ait++)
      {
        subcit = thirdpart_out.begin();
        for (; subcit != thirdpart_out.end(); subcit++)
        {
          if (ait->package == subcit->package)
          {
            ait->type = PKGT_THIRD_PARTY;
            break;
          }
        }
      }
      PointerWrapper<vector<ANDROID_PACKAGE_T>> tmp(new vector<ANDROID_PACKAGE_T>(out));
      ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::UI,
        new L2U_PackageList(std::wstring(L""), tmp));
    }
	  

  }


  void CTPModule::OnRefresh(std::string const & id) {
  }

  void CTPModule::OnDeviceChange(int const) {
    ScanDevices();
  }


  void CTPModule::OnGetLocalInstallApkList() {
    apk::Command * cmd = new apk::Command;
    cmd->set_cmd(command::kGetLocalPackageList);
    cmd->set_cmd_no(cmd_no());
    codec::MessagePtr ptr(cmd);
    current_cmd_no_set_.insert(cmd->cmd_no());
    channel_host_->SendProtobufMsg(switches::kCommunicatePyUpdateApk, ptr);
  }


  void CTPModule::OnGetNetInstallApkList() {
    //触发python的网测更新和包下发操作
    apk::Command * cmd = new apk::Command;
    cmd->set_cmd(command::kCheckNetPackageList);
    cmd->set_cmd_no(cmd_no());
    codec::MessagePtr ptr(cmd);
    current_cmd_no_set_.insert(cmd->cmd_no());
    channel_host_->SendProtobufMsg(switches::kCommunicatePyUpdateApk, ptr);
  }
  

  //void CTPModule::OnInstallApkList(PointerWrapper<std::vector<phone_module::ApkInstallInfo>> const & p) {
  //  std::vector<phone_module::ApkInstallInfo> & info = *p.get();
  //  for (auto it = info.begin(); it != info.end(); ++it) {
  //    std::wstring dir = apk_dir_;
  //    std::wstring file = dir.append(it->package_name).append(L".apk");
  //    std::pair<bool, std::wstring> result = adb_->InstallApk(file.c_str());

  //    StatusInfo * data = new StatusInfo(adb_->GetSerialNo2());
  //    data->op = StringPrintf(L"装包：%ls", it->package_name.c_str());

  //    if (result.first == true) {
  //      data->result = L"成功";
  //    } else {
  //      data->result = StringPrintf(L"失败：%ls", result.second.c_str());
  //    }
  //    PointerWrapper<StatusInfo> tmp(data);
  //    ThreadMessageDispatcherImpl::DispatchHelper(CommonThread::UI,
  //      new L2U_StatusInfo(tmp));
  //  }
	 // //adb_->InstallApk(L"E:\\workspace\\chromium24\\src\\build\\Debug\\apks\\AutoUnlock.apk");
  //}


  void CTPModule::OnInstallApkList(std::wstring const & type, PointerWrapper<std::vector<phone_module::ApkInstallInfo>> const & p) {
    std::vector<phone_module::ApkInstallInfo> & info = *p.get();
    for (auto it = info.begin(); it != info.end(); ++it) {
      std::wstring dir = apk_dir_;
      std::wstring file = dir.append(it->package_name).append(L".apk");
      //std::pair<bool, std::wstring> result = adb_->InstallApk(file.c_str());

      apk::Command * cmd = new apk::Command;
      cmd->set_cmd(command::kPyAdbInstallApk);
      cmd->set_cmd_no(cmd_no());
      cmd->add_param(WideToUTF8(type));
      cmd->add_param(WideToUTF8(file));
      codec::MessagePtr ptr(cmd);
      current_cmd_no_set_.insert(cmd->cmd_no());
      channel_host_->SendProtobufMsg(switches::kCommunicatePyUpdateApk, ptr);
      
    }
  }


  void CTPModule::OnCheckUpdateApkList() {
	  "E:/workspace/python/Python27/python.exe E:/workspace/chromium24/src/phone/python/ctp_py/main.py check_update  E:/workspace/chromium24/src/phone/python/check_update_home_prop";
	  //ctp::cp::Command* cmd = new ctp::cp::Command;
	  //cmd->set_cmd("echo_answer");
	  //codec::MessagePtr ptr(cmd);
	  //channel_host_->SendProtobufMsg(switches::kCommunicatePyUpdateApk, ptr);
  }

  void CTPModule::OnRemoveLocalInstallApkList() {
    apk::Command * cmd = new apk::Command;
    cmd->set_cmd(command::kRemoveLocalPackageList);
    cmd->set_cmd_no(cmd_no());
    codec::MessagePtr ptr(cmd);
    current_cmd_no_set_.insert(cmd->cmd_no());
    channel_host_->SendProtobufMsg(switches::kCommunicatePyUpdateApk, ptr);
  }

  void CTPModule::OnLaunchPY() {
    FilePath cur;
    FilePath python_exe;
    FilePath python_code;
    std::wstring python_exe_string;
    std::wstring python_code_string;
    if (PathService::Get(base::DIR_EXE, &cur)) {
      python_exe = cur.Append(FILE_PATH_LITERAL("python"));
      python_code = cur.Append(FILE_PATH_LITERAL("python_code"));
      
      python_exe_string = python_exe.value();
      python_code_string = python_code.value();
      python_code_string.append(L"\\main.pyc ").append(base::IntToString16(server_port_));
      python_exe_string.append(L"\\python.exe").append(L" ").append(python_code_string);
    }
#if defined(_DEBUG)
    python_exe_string = L"C:\\workspace\\Python27\\python.exe C:\\workspace\\code\\chromium24\\src\\phone\\python\\ctp_py\\main.py ";
    python_exe_string.append(base::IntToString16(server_port_));
#endif

    HANDLE process_handler;
    base::LaunchProcess(python_exe_string, base::LaunchOptions(), &process_handler);
  }


}
