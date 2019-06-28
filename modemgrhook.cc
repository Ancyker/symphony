#include "modemgrhook.h"
#include "signature.h"
#include <Windows.h>
#include <madCHook.h>

ModeMgrHook mode_mgr_hook;

void ModeMgrHook::Init()
{
	mode_ = -1;

	run_pointer_ = reinterpret_cast<int (__thiscall *)(CModeMgr *,int,char *)>(Signature("558BEC64A1000000006AFF68........508B450864892500000000538BD95657").getAddress());
	switch_pointer_ = reinterpret_cast<int (__thiscall *)(CModeMgr *,int,char *)>(Signature("558BEC8BD15356578B42048B7D0C83C9FF8D5A30C740140000000033C0F2AEF7").getAddress());

	HookCode(run_pointer_, RunHook, (PVOID*)&run_pointer_);
	HookCode(switch_pointer_, SwitchHook, (PVOID*)&switch_pointer_);
}

void ModeMgrHook::Dispose()
{
	UnhookCode((PVOID*)&run_pointer_);
	UnhookCode((PVOID*)&switch_pointer_);
}

int ModeMgrHook::RunHook(CModeMgr *mode_mgr, int mode, char *rsw)
{
	mode_mgr_hook.mode_ = mode;
	mode_mgr_hook.switched_signal_(mode_mgr, mode, rsw);

	return mode_mgr_hook.run_pointer_(mode_mgr, mode, rsw);
}

int ModeMgrHook::SwitchHook(CModeMgr *mode_mgr, int mode, char *rsw)
{
	int result = mode_mgr_hook.switch_pointer_(mode_mgr, mode, rsw);

	if (result)
	{
		mode_mgr_hook.mode_ = mode;

		mode_mgr_hook.switched_signal_(mode_mgr, mode, rsw);
	}

	return result;
}