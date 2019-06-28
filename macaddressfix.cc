#include "macaddressfix.h"
#include <boost/bind.hpp>
#include <winsock2.h>
#include <windows.h>
#include <iphlpapi.h>
#include <stdio.h>
#pragma comment(lib, "IPHLPAPI.lib")

MacAddressFix mac_address_fix;

void MacAddressFix::Init()
{
	connection_hook.ConnectSendPacketIdSlot(0x0064, boost::bind(&MacAddressFix::LoginPacketSlot, this, _1, _2, _3));
	connection_hook.ConnectSendPacketIdSlot(0x0277, boost::bind(&MacAddressFix::ExtendedLoginPacketSlot, this, _1, _2, _3));
	connection_hook.ConnectSendPacketIdSlot(0x02B0, boost::bind(&MacAddressFix::ExtendedLoginPacketSlot, this, _1, _2, _3));
}

void MacAddressFix::Dispose()
{
}

bool MacAddressFix::LoginPacketSlot(CRagConnection* connection, int length, uint8_t *data)
{
	uint8_t buffer[85];

	*reinterpret_cast<uint16_t*>(buffer) = 0x0277;
	memcpy(buffer+2, data+2, 53);
	strcpy(reinterpret_cast<char*>(buffer+71), "0.0.0.0");
	strcpy(reinterpret_cast<char*>(buffer+71), "111111111111");

	connection_hook.SendPacket(buffer);

	return true;
}

bool MacAddressFix::ExtendedLoginPacketSlot(CRagConnection* connection, int length, uint8_t *data)
{
	if (strncmp(reinterpret_cast<char*>(data+71), "111111111111", 12) && 
		strncmp(reinterpret_cast<char*>(data+71), "000000000000", 12))
	{
		return false;
	}

	ULONG addresses_size = 15000;
	IP_ADAPTER_ADDRESSES *addresses = reinterpret_cast<IP_ADAPTER_ADDRESSES*>(new uint8_t[addresses_size]);

	if (GetAdaptersAddresses(AF_INET, 0, NULL, addresses, &addresses_size) != ERROR_SUCCESS)
	{
		return false;
	}

	char buffer[13];

	while (addresses)
	{
		if (addresses->PhysicalAddressLength < 6)
		{
			addresses = addresses->Next;
			continue;
		}

		if (!addresses->PhysicalAddress[0] && 
			!addresses->PhysicalAddress[1] && 
			!addresses->PhysicalAddress[2] && 
			!addresses->PhysicalAddress[3] && 
			!addresses->PhysicalAddress[4] && 
			!addresses->PhysicalAddress[5])
		{
			addresses = addresses->Next;
			continue;
		}

		sprintf(buffer, "%.2x%.2x%.2x%.2x%.2x%.2x", 
			addresses->PhysicalAddress[0],
			addresses->PhysicalAddress[1],
			addresses->PhysicalAddress[2],
			addresses->PhysicalAddress[3],
			addresses->PhysicalAddress[4],
			addresses->PhysicalAddress[5]);

		strncpy(reinterpret_cast<char*>(data+71), buffer, 12);

		break;
	}

	delete [] reinterpret_cast<uint8_t*>(addresses);

	return false;
}