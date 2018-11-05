/**
* trader_unit.cpp
* ctp tsm
*
* @version 1.0 (06 ,20, 2013)
* @author reinhardt ken
*
*/



#include "tlv_codec.h"



#include "base/rand_util.h"
#include "base/logging.h"
#include "base/string_tokenizer.h"
#include "base/string_number_conversions.h"

#include "base/string_util.h"


#include "tlv_define.h"




namespace codec {


void TLVCodec::FillEmptyBuffer(const TLVObjectVector& message, TransferBuffer *buffer) {
  TLVObject::WriteTLVList(*buffer, message);
}






const std::string TLVCodec::ErrorCodeToString(int32 errorCode) {
  return std::string();
}

void TLVCodec::DefaultErrorCallback(channel::TCPSocket &, base::Time const&, int32 const errorCode) {
  LOG(ERROR) << "TLVCodec::defaultErrorCallback - " << ErrorCodeToString(errorCode);
  DCHECK(false);
}


void TLVCodec::OnMessage(std::string const & name, base::Time const& receiveTime) {

  while(true) {
     if (HasHead(last_received_data_.size())) {
  
      uint32 len = *(uint32*)&last_received_data_[0];
      if (kHeaderLen + len <= last_received_data_.size()) {
        //数据收齐了=

        uint32 number = *(uint32*)(&last_received_data_[kHeaderLen]);
        TLVMessagePtr ptr(new TLVObjectVector(number));
        TransferBuffer buffer(&last_received_data_[2*kHeaderLen], len - kHeaderLen);

        for (uint32 i = 0; i < number; ++i) {
          int tag = buffer.ReadInt();
          TLVObject tlv = TLVObject::ReadTLVObject(buffer, tag);
          ptr->operator[](i) = tlv;
        }

        //调用分发函数=
        auto it = message_callback_.find(name);
        if (it != message_callback_.end()) {
          it->second.Run(name, ptr, receiveTime);
        }
        //把余下的数据copy到头部=
        if (codec::HasMoreData(len, last_received_data_)) {
          continue;
        } else {
          return;
        }

      } else {
        return;
      }
   }
  }
}
  






}


