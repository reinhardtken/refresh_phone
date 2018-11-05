#pragma  once


//Action后面的数字，第一个表示当前状态，第二个表示收到的事件
#define DECLARE_ACTION_FUNC(x, y) void Action##x##y(Event##y const &);

#define DECLARE_ACTION_FUNC_VIRTUAL(x, y) virtual void Action##x##y(Event##y const &) {}

//#define MOCK__ACTION_FUNC(c, x, y) void c##::Action##x##y(Event##y const &) {}


#define MOCK_ACTION_FUNC(c, x, y) void c##::Action##x##y(Event##y const &) {\
	NOTIFYMESSAGEMACRO3(phone_module::NORMAL, phone_module::NOTIFICATION_CTP_TORMSM_STATE_CHANGE_NODEAL, \
	"-", #c,\
	"state", base::UintToString(GetState()),\
	"event", #y);\
	}\

#define MACRO_STRING(x) #x
/**
#define CALLBACK_MACRO1(x) Delegate::CallbackParam d(x);\
  delegate()->Callback(d);

#define CALLBACK_MACRO2(x, y) Delegate::CallbackParam d(x, y);\
  delegate()->Callback(d);

#define CALLBACK_MACRO4(x, y, z, w) Delegate::CallbackParam d(x, y, z, w);\
  delegate()->Callback(d);

#define CALLBACK_MACRO5(x, y, z, w, w2) Delegate::CallbackParam d(x, y, z, w, w2);\
  delegate()->Callback(d);
*/