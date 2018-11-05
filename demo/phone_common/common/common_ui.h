#pragma once

#include "base/basictypes.h"
#include "common_main.h"

#include "../include/ctp_define.h"

#include "base/memory/scoped_ptr.h"


class CommandLine;



class PHONE_COMMON_EXPORT CommonUI {
public:
  CommonUI() {}
  ~CommonUI() {}

public:
  bool CommonUI::Start(CommonMainParams & params) {
    return params.delegate->ShowUI(params.command_line);
  }

private:
  DISALLOW_COPY_AND_ASSIGN(CommonUI);
};


namespace common_util {
  
}

