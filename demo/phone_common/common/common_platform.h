#pragma once

#include "../include/ctp_define.h"

namespace common_platform {
  void PHONE_COMMON_EXPORT LowLevelInit(void* hinstance);
  void PHONE_COMMON_EXPORT LowLevelShutdown();
  void PHONE_COMMON_EXPORT CreateConsole();
}
