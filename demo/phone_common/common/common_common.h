#pragma once

#include "base/basictypes.h"


#include "../include/ctp_define.h"



namespace switches {
  PHONE_COMMON_EXPORT extern const char kProcessTypeGuard[];
  PHONE_COMMON_EXPORT extern const char kProcessTypeMQ[];
  PHONE_COMMON_EXPORT extern const char kProcessTypeTU[];
  PHONE_COMMON_EXPORT extern const char kProcessTypeMQTU[];
  PHONE_COMMON_EXPORT extern const char kProcessTypePyCmd[];
  PHONE_COMMON_EXPORT extern const char kProcessTypePySQL[];    
  PHONE_COMMON_EXPORT extern const char kProcessTypeRlangMS[];    
  PHONE_COMMON_EXPORT extern const char kProcessTypeTest[];
  PHONE_COMMON_EXPORT extern const char kProcessTypeBacktesting[];
  PHONE_COMMON_EXPORT extern const char kProcessTypePyBacktesting[];

  PHONE_COMMON_EXPORT extern const char kProcessTypePhone[];
  


  PHONE_COMMON_EXPORT extern const char kCommunicateMQ[];
  PHONE_COMMON_EXPORT extern const char kCommunicateTU[];
  PHONE_COMMON_EXPORT extern const char kCommunicateMQAndTU[];
  PHONE_COMMON_EXPORT extern const char kCommunicateCmd[];
  PHONE_COMMON_EXPORT extern const char kCommunicateMQSQL[];    
  PHONE_COMMON_EXPORT extern const char kCommunicateCmdAndSQL[];
  PHONE_COMMON_EXPORT extern const char kCommunicateMS[];    
  PHONE_COMMON_EXPORT extern const char kCommunicateTest[];
  PHONE_COMMON_EXPORT extern const char kCommunicateBacktesting[]; 

  PHONE_COMMON_EXPORT extern const char kCommunicateMQAlive[];
  PHONE_COMMON_EXPORT extern const char kCommunicateTUAlive[];
  PHONE_COMMON_EXPORT extern const char kCommunicateMQAndTUAlive[];
  PHONE_COMMON_EXPORT extern const char kCommunicateCmdAlive[];
  PHONE_COMMON_EXPORT extern const char kCommunicateMQSQLAlive[];
  PHONE_COMMON_EXPORT extern const char kCommunicateCmdAndSQLAlive[];
  PHONE_COMMON_EXPORT extern const char kCommunicateMSAlive[];
  PHONE_COMMON_EXPORT extern const char kCommunicateTestAlive[];
  PHONE_COMMON_EXPORT extern const char kCommunicateBacktestingAlive[];
  PHONE_COMMON_EXPORT extern const char kCommunicatePyBacktestingAlive[];
  PHONE_COMMON_EXPORT extern const char kCommunicatePyMQCheckAlive[];
  PHONE_COMMON_EXPORT extern const char kCommunicatePyToolboxAlive[];

  PHONE_COMMON_EXPORT extern const char kCommunicatePyUpdateApk[];
  PHONE_COMMON_EXPORT extern const char kCommunicatePyUpdateApkAlive[];

  PHONE_COMMON_EXPORT extern const char kCmdParamsAutoLogin[];
}

namespace prefs {
  //pref
  PHONE_COMMON_EXPORT extern const char kGuardServer[];
  PHONE_COMMON_EXPORT extern const char kMQServer[];
  PHONE_COMMON_EXPORT extern const char kTUServer[];
  PHONE_COMMON_EXPORT extern const char kMQTUServer[];
  PHONE_COMMON_EXPORT extern const char kBacktestingServer[];

  //PHONE_COMMON_EXPORT extern const char kCheckUpdateApkListAlive[];
  PHONE_COMMON_EXPORT extern const char kCheckUpdateApkList[];

  PHONE_COMMON_EXPORT extern const char kTUPyPathAndCmd[];
  PHONE_COMMON_EXPORT extern const char kMQPyPathAndCmd[];
  PHONE_COMMON_EXPORT extern const char kBacktestingPyPathAndCmd[];
  PHONE_COMMON_EXPORT extern const char kMQCheckPyPathAndCmd[];
  PHONE_COMMON_EXPORT extern const char kPyToolboxPyPathAndCmd[];

  PHONE_COMMON_EXPORT extern const char kGuardAutoRun[];
  PHONE_COMMON_EXPORT extern const char kGuardTimingRun[];
  PHONE_COMMON_EXPORT extern const char kGuardTimingInterval[];
  PHONE_COMMON_EXPORT extern const char kBacktestingType[];
  PHONE_COMMON_EXPORT extern const char kBacktestingLevel[];
  PHONE_COMMON_EXPORT extern const char kBacktestingOneMode[];
  PHONE_COMMON_EXPORT extern const char kBacktestingLogLevelMQ[];
  PHONE_COMMON_EXPORT extern const char kBacktestingDefaultExpectTableName[];
  PHONE_COMMON_EXPORT extern const char kBacktestingPropExpectTableName[];
  PHONE_COMMON_EXPORT extern const char kBacktestingMQTableName[];
  PHONE_COMMON_EXPORT extern const char kBacktestingHighLevelMQNotify[];

  
}

namespace market_strategy_name {
  PHONE_COMMON_EXPORT extern const char kSuccessive[];
  PHONE_COMMON_EXPORT extern const char kSuccessiveStopWin[];
  PHONE_COMMON_EXPORT extern const char kManual[];
  PHONE_COMMON_EXPORT extern const char kStop[];
}

namespace command {
  PHONE_COMMON_EXPORT extern const char kRemoveLocalPackageList[];
  PHONE_COMMON_EXPORT extern const char kCheckNetPackageList[];
  PHONE_COMMON_EXPORT extern const char kGetLocalPackageList[];
  PHONE_COMMON_EXPORT extern const char kPYConfig[];
}