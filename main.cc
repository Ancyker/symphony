#include <windows.h>
#include "connectionhook.h"
#include "macaddressfix.h"
#include "filehook.h"
#include "loghook.h"
#include "acthasher.h"
#include "sessionhook.h"
#include "modemgrhook.h"
#include "xray.h"
#include "memory.h"
#include "gmhook.h"

BOOL __declspec(dllexport) WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		memory.Init();
		session_hook.Init();
		file_hook.Init();
		connection_hook.Init();
		log_hook.Init();
		mac_address_fix.Init();
		act_hasher.Init();
		mode_mgr_hook.Init();
		xray.Init();
		gm_hook.Init();
	}
	else if (fdwReason == DLL_PROCESS_ATTACH)
	{
		xray.Dispose();
		mode_mgr_hook.Dispose();
		connection_hook.Dispose();
		mac_address_fix.Dispose();
		file_hook.Dispose();
		log_hook.Dispose();
		act_hasher.Dispose();
		session_hook.Dispose();
		memory.Dispose();
		gm_hook.Dispose();
	}

	return TRUE;
}
