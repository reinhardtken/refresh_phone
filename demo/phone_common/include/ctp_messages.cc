//自己定义的ipc message，都要写个cpp包含，并且先定义IPC_MESSAGE_IMPL

#define IPC_MESSAGE_IMPL
#include "ctp_messages.h"



char const* MessageType2String(uint32 const type) {
  switch(type) {
    case CTP_CreateTradeUnit::ID: {
      return "CTP_CreateTradeUnit";
    }
    case CTP_TradeUnitCreated::ID: {
      return "CTP_TradeUnitCreated";
    }
    case CTP_MessageBroadcast::ID: {
      return "CTP_MessageBroadcast";
    }
    case CTP_TU_Login::ID: {
      return "CTP_TU_Login";
    }
    case CTP_TU_Logout::ID: {
      return "CTP_TU_Logout";
    }
    case CTP_DC_QryPositionList::ID: {
      return "CTP_DC_QryPositionList";
    }
    case CTP_TU_Open::ID: {
      return "CTP_TU_Open";
    }
    case CTP_TU_Close::ID: {
      return "CTP_TU_Close";
    }
    default: {
      return "Unknonw ...";
    }
  }
}
