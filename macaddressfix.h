#ifndef SYMPHONY4_MACADDRESSFIX_H
#define SYMPHONY4_MACADDRESSFIX_H

#include "connectionhook.h"

class MacAddressFix
{
public:
	void Init();
	void Dispose();

protected:
	bool LoginPacketSlot(CRagConnection* connection, int length, uint8_t *data);
	bool ExtendedLoginPacketSlot(CRagConnection* connection, int length, uint8_t *data);
};

extern MacAddressFix mac_address_fix;

#endif