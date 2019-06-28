#ifndef SYMPHONY4_LOGHOOK_H
#define SYMPHONY4_LOGHOOK_H

#include <boost/signal.hpp>
#include "connectionhook.h"

struct UIWindowMgr;

class LogHook
{
public:
	typedef boost::signal<bool (const char*,int,int)> write_signal_t;
	void Init();
	void Dispose();

	void Write(const char *message, int color, int type = 0);

	boost::signals::connection ConnectWriteSlot(write_signal_t::slot_function_type slot, int priority = 0) { return write_signal_.connect(priority, slot); };

protected:
	UIWindowMgr *window_mgr_;
	write_signal_t write_signal_;

	bool LogPacketSlot(CRagConnection *connection, uint8_t *buffer);

	static int __thiscall SendMsgHook(UIWindowMgr *window_mgr, int a2, const char* a3, int a4, int a5, int a6);
	int (__thiscall *send_msg_pointer_)(UIWindowMgr *window_mgr, int a2, const char* a3, int a4, int a5, int a6);
};

extern LogHook log_hook;

#endif