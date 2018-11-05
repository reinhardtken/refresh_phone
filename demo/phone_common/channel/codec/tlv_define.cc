/**
* trader_unit.cpp
* ctp tsm
*
* @version 1.0 (06 ,20, 2013)
* @author reinhardt ken
*
*/



#include "tlv_define.h"



#include "base/rand_util.h"
#include "base/logging.h"
#include "base/string_tokenizer.h"
#include "base/string_number_conversions.h"

#include "base/string_util.h"







namespace codec {

  int const TLVObject::TAG_UNKNOWN = 0;
  int const TLVObject::TAG_INT = 1;
  int const TLVObject::TAG_INT64 = 2; 
  int const TLVObject::TAG_DOUBLE = 3;
  int const TLVObject::TAG_STRING = 4;
  

  TLVObject TLVObject::CreateTLVIntObject(int const v) {
    TLVObject tlv;
    tlv.t = TAG_INT;
    tlv.l = 4;
    tlv.v_int = v;
    return tlv;
  }

  TLVObject TLVObject::CreateTLVInt64Object(int64 const v) {
    TLVObject tlv;
    tlv.t = TAG_INT64;
    tlv.l = 8;
    tlv.v_int64 = v;
    return tlv;
  }

  TLVObject TLVObject::CreateTLVDoubleObject(double const v) {
    TLVObject tlv;
    tlv.t = TAG_DOUBLE;
    tlv.l = 8;
    tlv.v_double = v;
    return tlv;
  }

  TLVObject TLVObject::CreateTLVStringObject(std::string const &v) {
    TLVObject tlv;
    tlv.t = TAG_STRING;
    tlv.l = v.size();
    tlv.v_string = v;
    return tlv;
  }


  TLVObject TLVObject::ReadTLVIntObject(TransferBuffer const& buf) {
    int size = buf.ReadInt();
    int value = buf.ReadInt();
    return CreateTLVIntObject(value);
  }

  TLVObject TLVObject::ReadTLVInt64Object(TransferBuffer const& buf) {
    int size = buf.ReadInt();
    int64 value = buf.ReadInt64();
    return CreateTLVInt64Object(value);
  }

  TLVObject TLVObject::ReadTLVDoubleObject(TransferBuffer const& buf) {
    int size = buf.ReadInt();
    double value = buf.ReadDouble();
    return CreateTLVDoubleObject(value);
  }

  TLVObject TLVObject::ReadTLVStringObject(TransferBuffer const& buf) {
    int size = buf.ReadInt();
    std::string value = buf.ReadString(size);
    return CreateTLVStringObject(value);
  }

  TLVObject TLVObject::ReadTLVObject(TransferBuffer const& buf, int const tag) {
    if (tag == TAG_INT) {
        return ReadTLVIntObject(buf);
    } else if (tag == TAG_INT64) {
        return ReadTLVInt64Object(buf);
    } else if (tag == TAG_DOUBLE) {
        return ReadTLVDoubleObject(buf);
      } else if (tag == TAG_STRING) {
          return ReadTLVStringObject(buf);
      } else {
        LOG(ERROR)<<"error TLV data!!!!!!!!!!";
        CHECK(false);
        return TLVObject();
      }
  }

  int TLVObject::WriteTLVObject(TransferBuffer & buf, TLVObject const &tlv) {
    if (tlv.t == TAG_INT) {
      buf.WriteInt(TAG_INT);
      buf.WriteInt(4);
      buf.WriteInt(tlv.v_int);
      return 12;
    } else if (tlv.t == TAG_INT64) {
      buf.WriteInt(TAG_INT64);
      buf.WriteInt(8);
      buf.WriteInt64(tlv.v_int64);
      return 16;
    } else if (tlv.t == TAG_DOUBLE) {
      buf.WriteInt(TAG_DOUBLE);
      buf.WriteInt(8);
      buf.WriteDouble(tlv.v_double);
      return 16;
      } else if (tlv.t == TAG_STRING) {
        buf.WriteInt(TAG_STRING);
        buf.WriteInt(tlv.v_string.size());
        buf.WriteString(tlv.v_string);
        return 9 + tlv.v_string.size();
      } else {
        LOG(ERROR)<<"error TLV data!!!!!!!!!!";
        CHECK(false);
        return (0);
      }
  }

  int TLVObject::CalculateTLVObjectSize(TLVObject const &tlv) {
      if (tlv.t == TAG_INT) {
        return (12);
      } else if (tlv.t == TAG_INT64) {
        return (16); 
      } else if (tlv.t == TAG_DOUBLE) {
        return (16); 
      } else if (tlv.t == TAG_STRING) {
        return (8 + 1 + tlv.v_string.size());
      } else {
        LOG(ERROR)<<"error TLV data!!!!!!!!!!";
        CHECK(false);
        return (0);
      }
  }

  int TLVObject::CalculateTLVListtSize(std::vector<TLVObject> const &tlv_list) {
      int total = 4;
      for (unsigned int i = 0; i < tlv_list.size(); ++i) {
        total = total + CalculateTLVObjectSize(tlv_list[i]);
      }
      return (total);
  }

  int TLVObject::WriteTLVList(TransferBuffer & buf, std::vector<TLVObject> const &tlv_list) {
      int total = CalculateTLVListtSize(tlv_list);
      buf.WriteInt(total + 4);
      buf.WriteInt(tlv_list.size());
      int cal = 0;
      for (unsigned int i = 0; i < tlv_list.size(); ++i) {
        cal += WriteTLVObject(buf, tlv_list[i]);
      }
      DCHECK(cal + 4 == total);
      return total + 4;
  }

}


