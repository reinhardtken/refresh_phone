#pragma once

#include "base/basictypes.h"
#include "base/threading/non_thread_safe.h"
#include "base/time.h"
#include <memory>

#include "../include/ctp_define.h"


#define PERFORMANCE_MACOR common_util::ScopedPerformanceCheck scoped_performance_check(__FUNCTION__, __LINE__, NULL);
#define PERFORMANCE_MACOR_EX(x) common_util::ScopedPerformanceCheck scoped_performance_check(__FUNCTION__, __LINE__, (x));

class PrefService;
namespace common_util {

  struct PerformanceQueueItem;

  class PHONE_COMMON_EXPORT PerformanceThreadCheck : public base::NonThreadSafe{
  public:
    static PerformanceThreadCheck* current();
    static PerformanceThreadCheck* Create();

    void Add(std::shared_ptr<PerformanceQueueItem const> item);
    std::string Dump();
    void Clear();
    base::TimeDelta CalculateCosts();

    ~PerformanceThreadCheck();
    
  private:
    PerformanceThreadCheck();

  private:
    typedef std::vector<std::shared_ptr<PerformanceQueueItem const>> PerformanceVector;
    PerformanceVector queue_;

    DISALLOW_COPY_AND_ASSIGN(PerformanceThreadCheck);
  };


  struct PerformanceQueueItem {
    char const* func_name;
    int line;
    base::TimeDelta cost;
    char const* extra_info;
  };


  class PHONE_COMMON_EXPORT ScopedPerformanceCheck {
  public:
    ScopedPerformanceCheck(char const* func, int const l, char const* extra)
      :start_(base::Time::Now()) {
        item_.reset(new PerformanceQueueItem);
        item_->func_name = func;
        item_->line = l;
        item_->extra_info = extra;
      }
    ~ScopedPerformanceCheck();


  private:
    std::shared_ptr<PerformanceQueueItem> item_;
    base::Time start_;
  };




  //std::string PHONE_COMMON_EXPORT SSMap2String(phone_module::NotifyMessage const &m);
  std::string PHONE_COMMON_EXPORT SSMap2String(phone_module::SSMap const &m);

  void PHONE_COMMON_EXPORT NotifyMessageFunction(phone_module::MessageType2UI const type, uint32 const id, std::string const &m);

  std::string PHONE_COMMON_EXPORT GetAccout(std::string const & s);



  PHONE_COMMON_EXPORT extern PrefService * g_pref_service;

  std::string PHONE_COMMON_EXPORT ProcessType2ServerName(std::string const & s);
  std::string PHONE_COMMON_EXPORT SocketName2ProcessType(std::string const & s);
  std::string PHONE_COMMON_EXPORT ProcessType2ClientName(std::string const & s);

  std::string PHONE_COMMON_EXPORT MsgType2String(uint32 const type);

  std::wstring PHONE_COMMON_EXPORT ToString(std::vector<std::string> const & v);
  std::wstring PHONE_COMMON_EXPORT ToString(std::map<std::string, double> const & v);
  std::wstring PHONE_COMMON_EXPORT ToString(std::map<std::string, uint32> const & v);
  std::wstring PHONE_COMMON_EXPORT ToString(std::map<std::string, std::string> const & v);


  void PHONE_COMMON_EXPORT Encode(unsigned char* key, std::vector<char> & buf, std::vector<char> & out);
  void PHONE_COMMON_EXPORT Decode(unsigned char* key, std::vector<char> & buf, std::vector<char> & out);

  std::string PHONE_COMMON_EXPORT Time2String(base::Time const & time, uint32 const type);
  std::wstring PHONE_COMMON_EXPORT Time2StringW(base::Time const & time, uint32 const type);

base::Time PHONE_COMMON_EXPORT Time2StrictTime(base::Time const & time, uint32 const type);
}

