//#include "StdAfx.h"
#include <TlHelp32.h>
#include <wtypes.h>
#include <winnt.h>
#include "adb_py_interface.h"
#include <fstream>

#include "base/bind.h"
#include "base/file_util.h"
#include "base/path_service.h"
#include "base/logging.h"
#include "base/string_util.h"

#include "phone_common/common/pref_service.h"
#include "phone_common/common/common_common.h"
#include "phone_common/channel/channel_host.h"

#include "../../protobuf/out/apk_protomsg.pb.h"


#include "phone_spi/phone_module/phone_module.h"


namespace phone_module {

  PythonAdbInterface::PythonAdbInterface(CTPModule* p)
    :core_(p),
    keep_scan_(false) {
    adb_server_.reset(new channel::ChannelHost());
    //channel::ServerResult result = adb_server_->InitializeServer(::prefs::kAdbServer, false);
    //DCHECK(result.first == true && result.second == 5037);
  }

  PythonAdbInterface::~PythonAdbInterface() {

  }

  void PythonAdbInterface::ScanDevices() {
    ScanDevicesNow();

    if (keep_scan_) {
      CommonThread::PostDelayedTask(CommonThread::CTP,
        FROM_HERE,
        base::Bind(&PythonAdbInterface::ScanDevices, base::Unretained(this)),
        10 * 1000);
    }
  }

  void PythonAdbInterface::ScanDevicesNow() {
    apk::Command * cmd = new apk::Command;
    cmd->set_cmd(command::kPyAdbScanDevices);
    cmd->set_cmd_no(core_->cmd_no());
    codec::MessagePtr ptr(cmd);
    core_->channel_host_->SendProtobufMsg(switches::kCommunicatePyUpdateApk, ptr);
  }

  void PythonAdbInterface::StartScan() {
    keep_scan_ = true;
    ScanDevices();
  }

  void PythonAdbInterface::StopScan() {
    keep_scan_ = false;
  }
}