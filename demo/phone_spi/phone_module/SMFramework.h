/**
* SMFramework.h
* 状态机框架
*
* @version 1.0 (06 ,10, 2008)
* @author reinhardt ken
*
*/
#pragma  once


#include <boost/mpl/fold.hpp>
#include <boost/mpl/filter_view.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/mpl/vector.hpp>
//boost 1.35
#include <boost/mpl/vector/aux_/preprocessed/plain/vector30.hpp>
//boost 1.53
//#include <boost/mpl/vector/vector30.hpp>

#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/static_assert.hpp>
//#include <iostream>
namespace mpl = boost::mpl;
/*using namespace mpl::placeholders;*/

#include <cassert>

template<
  class Transition
    , class Next
>
struct event_dispatcher
{
  typedef typename Transition::fsm_t fsm_t;
  typedef typename Transition::event event;

  static int dispatch(
    fsm_t& fsm, int state, event const& e)
  {
    if (state == Transition::current_state)
    {
      Transition::execute(fsm, e);
      return Transition::next_state;
    }
    else // move on to the next node in the chain.
    {
      return Next::dispatch(fsm, state, e);
    }
  }
};



template <class Derived> class state_machine;

struct default_event_dispatcher
{
  template<class FSM, class Event>
  static int dispatch(
    state_machine<FSM>& m, int state, Event const& e)
  {
    return m.call_no_transition(state, e);
  }
};


template<class Table, class Event>
struct generate_dispatcher;

template<class Derived>
class state_machine
{
  // ...
protected:
  template<
    int CurrentState
    , class Event
    , int NextState
    , void (Derived::*action)(Event const&)
  >
  struct row
  {
    // for later use by our metaprogram
    static int const current_state = CurrentState;
    static int const next_state = NextState;
    typedef Event event;
    typedef Derived fsm_t;

    // do the transition action.
    static void execute(Derived& fsm, Event const& e)
    {
      (fsm.*action)(e);
    }
  };


  friend struct default_event_dispatcher;

  template <class Event>
  int call_no_transition(int state, Event const& e)
  {
    return static_cast<Derived*>(this) // CRTP downcast
      ->no_transition(state, e);
  }
  //  
public:

  template<class Event>
  int process_event(Event const& evt)
  {
    typedef typename generate_dispatcher<
      typename Derived::transition_table, Event
    >::type dispatcher;

    event_value(Event::Value);

    // dispatch the event.
    int old_state = this->state;
    this->state = dispatcher::dispatch(
      *static_cast<Derived*>(this) // CRTP downcast
      , this->state
      , evt
      );

    static_cast<Derived*>(this)->NotifyStateChanged(old_state, Event::Value, this->state);
    // return the new state
    return this->state;
  }

  int event_value() { return event_value_; }
  // ...
protected:
  //在处理一个event的callstack上，能检查当前的event是什么
  int event_value_;
  void event_value(int const v) { event_value_ = v; }

  state_machine()
    : state(Derived::initial_state)
  {
  }

protected:
  int state;
  // ...

  // ...
public:
  template <class Event>
  int no_transition(int state, Event const& e)
  {
    //assert(false);
    static_cast<Derived*>(this)->NoTransition(state, e);
    return state;
  }
};


template <class Transition>
struct transition_event
{
  typedef typename Transition::event type;
};

template<class Table, class Event>
struct generate_dispatcher
  : mpl::fold<
  mpl::filter_view< // select rows triggered by Event
  Table
  , boost::is_same<Event, transition_event<boost::mpl::_1> >
  >
  , default_event_dispatcher
  , event_dispatcher<boost::mpl::_2,boost::mpl::_1>  
  >
{};
