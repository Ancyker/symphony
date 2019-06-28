#include "signature.h"
#include "sessionhook.h"
#include <windows.h>
#include <madCHook.h>

SessionHook session_hook;

void SessionHook::Init()
{
	create_pointer_ = reinterpret_cast<int (__thiscall *)(CSession*)>(Signature("568BF1FF15........50E8........83C4048BCEE8........8BCEE8........").getAddress());
	get_job_pointer_ = reinterpret_cast<int (__thiscall *)(CSession *)>(Signature("8B81........8B0D........83F80775108BD1C1EA05F6C2017406B80D000000").getAddress());
	get_sex_pointer_ = reinterpret_cast<int (__thiscall *)(CSession *)>(Signature("8B81....00008B91....00003BD074068981....00008B81....0000C3......").getAddress());
	get_job_act_name_pointer_ = reinterpret_cast<char *(__thiscall *)(CSession *,int,int,char *)>(Signature("558BEC8B550C8B4508568BB1....00003D6E0F00008B3496568BB1....00007E").getAddress());

	HookCode(create_pointer_, CreateHook, (PVOID*)&create_pointer_);
}

void SessionHook::Dispose()
{
	UnhookCode((PVOID*)&create_pointer_);
}

int __thiscall SessionHook::CreateHook(CSession *session)
{
	int result = session_hook.create_pointer_(session);

	session_hook.session_ = session;

	session_hook.created_signal_(session);

	return result;
}