#pragma once

#include <vector>
#include <set>
#include <map>
#include <utility>


#include "base/memory/scoped_ptr.h"

#include "adb_define.h"


namespace channel {
  class ChannelHost;
}


namespace phone_module {

  class CTPModule;
  //=============================================================
  class PythonAdbInterface {
  public:
    PythonAdbInterface(CTPModule*);
    virtual ~PythonAdbInterface();

    void ScanDevices();
    void ScanDevicesNow();

    void StartScan();
    void StopScan();
    

  private:
    scoped_ptr<channel::ChannelHost> adb_server_;
    CTPModule* core_;
    int scan_times_;
  };


}