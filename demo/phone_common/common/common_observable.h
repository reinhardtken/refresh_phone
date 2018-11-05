#pragma once

#include <vector>


//#include "../include/ctp_define.h"

template<typename T>
class CommonObservable : public T{
public:
  void AddObserver(T * t) {
    observer_list_.push_back(t);
  }

  void RemoveObserver(T const* t) {
    for(size_t i = 0; i < observer_list_.size(); ++i) {
      if (observer_list_[i] == t) {
        observer_list_.erase(i);
        break;
      }
    }
  }

  template<typename Functor, typename Param1>
  void Notify(Param1 cosnt & p1) {
    for(size_t i = 0; i < observer_list_.size(); ++i) {
      //observer_list_.Functor(p1);
    }
  }

private:
  std::vector<T*> observer_list_;
};


template<typename T>
class CommonFilterable : public T{
public:
  void AddFilter(T * t) {
    filter_list_.push_back(t);
  }

  void RemoveFilter(T const* t) {
    for(size_t i = 0; i < filter_list_.size(); ++i) {
      if (filter_list_[i] == t) {
        filter_list_.erase(i);
        break;
      }
    }
  }

  template<typename Functor, typename Param1>
  void Notify(Param1 cosnt & p1) {
    for(size_t i = 0; i < filter_list_.size(); ++i) {
      //filter_list_.Functor(p1);
    }
  }

private:
  std::vector<T*> filter_list_;
};
