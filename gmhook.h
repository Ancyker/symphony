#ifndef SYMPHONY4_GMHOOK_H
#define SYMPHONY4_GMHOOK_H

#include "connectionhook.h"
#include <vector>

class GmHook
{
public:
	void Init();
	void Dispose();

protected:
	bool GmPacketSlot(CRagConnection *connection, uint8_t *data);

	static bool __cdecl IsNameYellowHook(unsigned int aid);
	static bool __cdecl IsGravityAidHook(unsigned int aid);

	bool (__cdecl *is_name_yellow_pointer_)(unsigned int aid);
	bool (__cdecl *is_gravity_aid_pointer_)(unsigned int aid);

	std::vector<unsigned int> gm_ids_;
};

extern GmHook gm_hook;

#endif