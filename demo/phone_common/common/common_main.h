#pragma once

#include "base/command_line.h"
#include "../include/ctp_define.h"


struct CommonMainDelegate {
  virtual bool ShowUI(CommandLine const &) = 0;
};
struct CommonMainParams{
  CommonMainParams(CommandLine const &c, CommonMainDelegate * d)
    :command_line(c),
    delegate(d) {}
  CommandLine const& command_line;
  CommonMainDelegate * delegate;
};



int PHONE_COMMON_EXPORT CommonMain(CommonMainParams params);