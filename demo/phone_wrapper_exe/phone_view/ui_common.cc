#include "ui_common.h"


namespace ui {
  //below code copy from common.cpp,make sure always the same ...
  std::string MakeupKey1(std::string const & type, phone_module::BuySellEnum const direction) {
    if (type.size() == 6 || type.size() == 5 || type == "CASH") {

      //G401, m1401
      if (type.size() == 5) {
        if (direction == phone_module::BuyOrder) {
          return type + "_" + "L";
        } else {
          return type + "_" + "S";
        }
      } else if (type == "CASH") {
        return "CASH+++";
      } else {
        if (direction == phone_module::BuyOrder) {
          return type + "L";
        } else {
          return type + "S";
        }
      }
    } else {
      CHECK(false);
    }

    return "";
  }

  std::string MakeupKey(std::string const & type, TThostFtdcPosiDirectionType const direction) {
    if (direction == THOST_FTDC_PD_Long) {
      return MakeupKey1(type, phone_module::BuyOrder);
    } else if (direction == THOST_FTDC_PD_Short) {
      return MakeupKey1(type, phone_module::SellOrder);
    } else {
      DCHECK(false);
    }

    return "";
  }


  //=======================================================
  char * const kSperatorTable = "sperator";



  //==============================================================

}