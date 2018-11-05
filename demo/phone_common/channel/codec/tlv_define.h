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
//#include "codec_util.h"



namespace codec {

  class PHONE_COMMON_EXPORT TransferBuffer {
  public:
    TransferBuffer(std::vector<char> const&v)
      :buffer_(v),
      index_(0) {}

    TransferBuffer(char const* p, uint32 size)
      :buffer_(p, p + size),
      index_(0) {}

    TransferBuffer() {}

    char ReadByte() const {
      DCHECK(index_ < buffer_.size());
      return buffer_[index_++];
    }

    short ReadShort() const {
      DCHECK(index_ + 1 < buffer_.size());
      short s = *reinterpret_cast<short const*>(&buffer_[index_]);
      index_ += 2;
      return s;
    }

    int ReadInt() const {
      DCHECK(index_ + 3 < buffer_.size());
      int s = *reinterpret_cast<int const*>(&buffer_[index_]);
      index_ += 4;
      return s;
    }

    int64 ReadInt64() const {
      DCHECK(index_ + 7 < buffer_.size());
      //r不支持64位长整
      double s = *reinterpret_cast<double const*>(&buffer_[index_]);
      index_ += 8;
      return (int64)s;
    }

    double ReadDouble() const {
      DCHECK(index_ + 7 < buffer_.size());
      double s = *reinterpret_cast<double const*>(&buffer_[index_]);
      index_ += 8;
      return s;
    }

    std::string ReadString(unsigned int size) const {
      //这个长度不包括结束符
      DCHECK(index_ + size + 1 < buffer_.size());
      char const*s = reinterpret_cast<char const*>(&buffer_[index_]);
      index_ += size + 1;
      std::string out = s;
      DCHECK(out.size() == size);
      return out;
    }

    void WriteByte(char const v) {
      buffer_.push_back(v);
    }

    void WriteShort(short const v) {
      index_ = buffer_.size();
      buffer_.resize(buffer_.size() + 2);
      short *s = reinterpret_cast<short*>(&buffer_[index_]);
      *s = v;
    }

    void WriteInt(int const v) {
      index_ = buffer_.size();
      buffer_.resize(buffer_.size() + 4);
      int *s = reinterpret_cast<int*>(&buffer_[index_]);
      *s = v;
    }

    void WriteInt64(int64 const v) {
      //因为r超过32位的都用浮点，所以。。。
      double temp = (double)v;
      WriteDouble(temp);
      return;
      /**
      index_ = buffer_.size();
      buffer_.resize(buffer_.size() + 8);
      int64 *s = reinterpret_cast<int64*>(&buffer_[index_]);
      *s = v;*/
    }

    void WriteDouble(double const v) {
      index_ = buffer_.size();
      buffer_.resize(buffer_.size() + 8);
      double *s = reinterpret_cast<double*>(&buffer_[index_]);
      *s = v;
    }

    void WriteString(std::string const &v) {
      index_ = buffer_.size();
      buffer_.resize(buffer_.size() + v.size() + 1);//结束符
      char *s = reinterpret_cast<char*>(&buffer_[index_]);
      strncpy_s(s, v.size() + 1, v.c_str(), _TRUNCATE);
      DCHECK(buffer_.back() == 0);
    }

    std::vector<char> const & buffer() { return buffer_; }
  private:
    std::vector<char> buffer_;
    unsigned int mutable index_;

  };



//========================================================================


  struct PHONE_COMMON_EXPORT TLVObject {
    TLVObject():t(TAG_UNKNOWN) {}


    int t;
    int l;
    int v_int;
    int64 v_int64;
    double v_double;
    std::string v_string;


    static int const TAG_UNKNOWN;
    static int const TAG_INT;
    static int const TAG_INT64; 
    static int const TAG_DOUBLE;
    static int const TAG_STRING;
  



    static TLVObject CreateTLVIntObject(int const v);

    static TLVObject CreateTLVInt64Object(int64 const v);

    static TLVObject CreateTLVDoubleObject(double const v);

    static TLVObject CreateTLVStringObject(std::string const &v);

    static TLVObject ReadTLVIntObject(TransferBuffer const& buf);

    static TLVObject ReadTLVInt64Object(TransferBuffer const& buf);

    static TLVObject ReadTLVDoubleObject(TransferBuffer const& buf);

    static TLVObject ReadTLVStringObject(TransferBuffer const& buf);

    static TLVObject ReadTLVObject(TransferBuffer const& buf, int const tag);

    static int WriteTLVObject(TransferBuffer & buf, TLVObject const &tlv);

    static int CalculateTLVObjectSize(TLVObject const &tlv);

    static int CalculateTLVListtSize(std::vector<TLVObject> const &tlv_list);

    static int WriteTLVList(TransferBuffer & buf, std::vector<TLVObject> const &tlv_list);

  };


}
