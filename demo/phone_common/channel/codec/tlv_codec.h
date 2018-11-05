/**
* trader_unit.h
* ctp tsm
*
* @version 1.0 (06 ,20, 2013)
* @author reinhardt ken
*
*/
#pragma  once

#include "base/logging.h"
// #include "base/memory/scoped_ptr.h"
// #include "base/string_number_conversions.h"
// #include "content/public/browser/notification_observer.h"
// #include "content/public/browser/notification_registrar.h"




#include "../../include/ctp_define.h"
#include "codec_util.h"



namespace codec {

  class PHONE_COMMON_EXPORT TLVCodec {
   public:
    

    TLVCodec(std::vector<char> &last_received_data)
      :last_received_data_(last_received_data) {
    }


    void RegisterHandler(std::string const & name, const TLVMessageCallback& messageCb) {
      message_callback_[name] = messageCb;
      errorCallback_ = base::Bind(&TLVCodec::DefaultErrorCallback);
    }

    void RegisterHandler(std::string const & name, const TLVMessageCallback& messageCb, const ErrorCallback& errorCb) {
      message_callback_[name] = messageCb;
      errorCallback_ = errorCb;
    }

    void OnMessage(std::string const & name, base::Time const& receiveTime);

    static const std::string ErrorCodeToString(int32 errorCode);
    static void FillEmptyBuffer(const TLVObjectVector& message, TransferBuffer *buffer);
    
    

   private:
    static void DefaultErrorCallback(channel::TCPSocket &, base::Time const&, int32 const);

    std::map<std::string, TLVMessageCallback> message_callback_;
    ErrorCallback errorCallback_;

    std::vector<char> &last_received_data_;


    DISALLOW_COPY_AND_ASSIGN(TLVCodec);
  };



}
