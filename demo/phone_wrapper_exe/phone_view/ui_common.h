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

//#include "log_daemon.h"
#include "../../phone_common/common/common_thread.h"
//#include "profiler_data.h"
#include "../../phone_common/include/ctp_define.h"
#include "../../phone_common/common/common_main.h"
#include "../../phone_common/common/thread_message_filter.h"
#include "../../phone_common/include/ctp_messages.h"

#include "../../phone_spi/phone_module/common.h"


namespace ui {
  //below code copy from common.cpp,make sure always the same ...
  std::string MakeupKey1(std::string const & type, phone_module::BuySellEnum const direction);

  std::string MakeupKey(std::string const & type, TThostFtdcPosiDirectionType const direction);

  extern char * const kSperatorTable;

}