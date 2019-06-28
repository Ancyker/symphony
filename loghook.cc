#include "loghook.h"
#include "signature.h"
#include <windows.h>
#include <madCHook.h>
#include <boost/bind.hpp>

LogHook log_hook;

void LogHook::Init()
{
	window_mgr_ = *reinterpret_cast<UIWindowMgr**>(Signature("........506A01E8........8B0D........85C974..8B116A006A006A006A006A3C6A00").getAddress());
	send_msg_pointer_ = reinterpret_cast<int (__thiscall *)(UIWindowMgr *,int,const char *,int,int,int)>(Signature("558BEC6AFF68........64A100000000506489250000000083EC688B45085348").getAddress());

	HookCode(send_msg_pointer_, SendMsgHook, (PVOID*)&send_msg_pointer_);

	connection_hook.SetPacketLength(0xA002, -1);
	connection_hook.ConnectRecvPacketIdSlot(0xA002, boost::bind(&LogHook::LogPacketSlot, this, _1, _2));
}

void LogHook::Dispose()
{
	UnhookCode((PVOID*)&send_msg_pointer_);
}

void LogHook::Write(const char *message, int color, int type)
{
	send_msg_pointer_(window_mgr_, 1, message, color, type, 0);
}

bool LogHook::LogPacketSlot(CRagConnection *connection, uint8_t *buffer)
{
	uint8_t color[4];
	color[0] = buffer[6];
	color[1] = buffer[5];
	color[2] = buffer[4];
	color[3] = 0;

	Write(reinterpret_cast<char*>(buffer+9), *reinterpret_cast<int*>(color), buffer[8]);

	return true;
}

int LogHook::SendMsgHook(UIWindowMgr* window_mgr, int a2, const char* a3, int a4, int a5, int a6)
{
	if (a2 == 1 && log_hook.write_signal_.num_slots())
	{
		if (log_hook.write_signal_(a3, a4, a5))
		{
			return 0;
		}
	}

	return log_hook.send_msg_pointer_(window_mgr, a2, a3, a4, a5, a6);
}