#include "common_common.h"



namespace switches {
  const char kProcessTypeGuard[] = "guard";
  const char kProcessTypeMQ[] = "mq";
  const char kProcessTypeTU[] = "tu";
  const char kProcessTypeMQTU[] = "mq-tu";
  const char kProcessTypePyCmd[] = "py-cmd";
  const char kProcessTypePySQL[] = "py-mysql";    
  const char kProcessTypeRlangMS[] = "rlang-ms";    
  const char kProcessTypeTest[] = "test";
  const char kProcessTypeBacktesting[] = "backtesting";
  const char kProcessTypePyBacktesting[] = "py-backtesting";

  const char kProcessTypePhone[] = "phone";

  //协议://语言/业务1+业务2+业务3
  const char kCommunicateMQ[] = "protobuf://cpp/mq";
  const char kCommunicateTU[] = "protobuf://cpp/tu";
  const char kCommunicateMQAndTU[] = "protobuf://cpp/mq+tu";
  const char kCommunicateCmd[] = "protobuf://python/cmd";
  const char kCommunicateMQSQL[] = "protobuf://python/mysql";    
  const char kCommunicateCmdAndSQL[] = "protobuf://python/cmd+mysql";
  const char kCommunicateMS[] = "tlv://rlang/ms";    
  const char kCommunicateTest[] = "protobuf://cpp/test";
  const char kCommunicateBacktesting[] = "protobuf://python/backtesting";

  //报活
  const char kCommunicateMQAlive[] = "protobuf://cpp/mq-alive";
  const char kCommunicateTUAlive[] = "protobuf://cpp/tu-alive";
  const char kCommunicateMQAndTUAlive[] = "protobuf://cpp/mq-tu-alive";
  const char kCommunicateCmdAlive[] = "protobuf://python/cmd-alive";
  const char kCommunicateMQSQLAlive[] = "protobuf://python/mysql-alive";    
  const char kCommunicateCmdAndSQLAlive[] = "protobuf://python/cmd-mysql-alive";
  const char kCommunicateMSAlive[] = "tlv://rlang/ms-alive";    
  const char kCommunicateTestAlive[] = "protobuf://cpp/test-alive";
  const char kCommunicateBacktestingAlive[] = "protobuf://cpp/backtesting-alive";
  const char kCommunicatePyBacktestingAlive[] = "protobuf://python/backtesting-alive";
  const char kCommunicatePyMQCheckAlive[] = "protobuf://python/mqcheck-alive";
  const char kCommunicatePyToolboxAlive[] = "protobuf://python/toolbox-alive";

  //业务
  const char kCommunicatePyUpdateApk[] = "protobuf://python/update-apk";
  //报活
  const char kCommunicatePyUpdateApkAlive[] = "protobuf://python/update-apk-alive";

  const char kCmdParamsAutoLogin[] = "auto-login";
}

namespace prefs {
  //pref
  const char kGuardServer[] = "tcp_server.guard_server";
  const char kMQServer[] = "tcp_server.mq_server";
  const char kTUServer[] = "tcp_server.tu_server";
  const char kMQTUServer[] = "tcp_server.mqtu_server";
  const char kBacktestingServer[] = "tcp_server.backtesting_server";

  //const char kCheckUpdateApkListAlive[] = "tcp_server.adbtool_alive_server";
  const char kCheckUpdateApkList[] = "tcp_server.adbtool_py_server";
  const char kAdbServer[] = "tcp_server.adb_server";

  const char kTUPyPathAndCmd[] = "TUPyPathAndCmd";
  const char kMQPyPathAndCmd[] = "MQPyPathAndCmd";
  const char kBacktestingPyPathAndCmd[] = "BacktestingPyPathAndCmd";
  const char kMQCheckPyPathAndCmd[] = "MQCheckPyPathAndCmd";
  const char kPyToolboxPyPathAndCmd[] = "PyToolboxPyPathAndCmd";


  const char kGuardAutoRun[] = "guard.auto_run";
  const char kGuardTimingRun[] = "guard.timing";
  const char kGuardTimingInterval[] = "guard.timing-interval";
  const char kBacktestingType[] = "Backtesting.type";
  const char kBacktestingLevel[] = "Backtesting.level";
  const char kBacktestingOneMode[] = "Backtesting.one_account_mode";
  const char kBacktestingLogLevelMQ[] = "Backtesting.log_levelmq";
  const char kBacktestingDefaultExpectTableName[] = "Backtesting.default_expect_data_table_name";
  const char kBacktestingPropExpectTableName[] = "Backtesting.prop_expect_data_table_name";
  const char kBacktestingMQTableName[] = "Backtesting.mq_data_table_name";
  const char kBacktestingHighLevelMQNotify[] = "Backtesting.high_level_mq_notify";

}


namespace market_strategy_name {
  const char kSuccessive[] = "ms.successive.";
  // 止盈命令，这个设计不是太好，先这么用 [7/9/2015 ken]
  const char kSuccessiveStopWin[] = "ms.successive.stopwin";
  const char kManual[] = "ms.manual";
  const char kStop[] = "ms.stop";
}

namespace command {
  const char kRemoveLocalPackageList[] = "remove_local_package_list";
  const char kCheckNetPackageList[] = "check_net_package_list";
  const char kGetLocalPackageList[] = "get_local_package_list";
  const char kPYConfig[] = "py_config";
  //const char kInstallApk[] = "install_apk";

  const char kPyAdbInstallApk[] = "pyadb_install_apk";
  const char kPyAdbGetPackageList[] = "pyadb_get_package_list";
  const char kPyAdbScanDevices[] = "pyadb_scan_devices";
  const char kPyAdbRemoveApk[] = "pyadb_remove_apk";
}