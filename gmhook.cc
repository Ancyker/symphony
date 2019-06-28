#include "gmhook.h"
#include <windows.h>
#include <madCHook.h>
#include <boost/bind.hpp>

GmHook gm_hook;

void GmHook::Init()
{
	is_name_yellow_pointer_ = reinterpret_cast<bool (__cdecl *)(unsigned int)>(0x52EA50);
	is_gravity_aid_pointer_ = reinterpret_cast<bool (__cdecl *)(unsigned int)>(0x52EA80);

	HookCode(is_name_yellow_pointer_, IsNameYellowHook, (PVOID*)&is_name_yellow_pointer_);
	HookCode(is_gravity_aid_pointer_, IsGravityAidHook, (PVOID*)&is_gravity_aid_pointer_);

	connection_hook.SetPacketLength(0xA005, 6);
	connection_hook.ConnectRecvPacketIdSlot(0xA005, boost::bind(&GmHook::GmPacketSlot, this, _1, _2));
}

void GmHook::Dispose()
{
	UnhookCode((PVOID*)&is_name_yellow_pointer_);
	UnhookCode((PVOID*)&is_gravity_aid_pointer_);
}

bool GmHook::GmPacketSlot(CRagConnection *connection, uint8_t *data)
{
	unsigned int aid = *reinterpret_cast<unsigned int*>(data+2);

	if (!IsGravityAidHook(aid))
	{
		gm_ids_.insert(gm_ids_.begin(), aid);
	}

	return true;
}

bool GmHook::IsNameYellowHook(unsigned int aid)
{
	return IsGravityAidHook(aid);
}

bool GmHook::IsGravityAidHook(unsigned int aid)
{
	for (std::vector<unsigned int>::const_iterator i = gm_hook.gm_ids_.begin(); i != gm_hook.gm_ids_.end(); ++i)
	{
		if (aid == *i)
		{
			return true;
		}
	}

	return false;
}