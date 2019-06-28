#include <boost/bind.hpp>
#include "acthasher.h"
#include "sessionhook.h"
#include "connectionhook.h"
#include "modemgrhook.h"

#define CRYPTOPP_DEFAULT_NO_DLL
#include <crc.h>

ActHasher act_hasher;

void ActHasher::Init()
{
	file_hook.ConnectOpenedSlot(boost::bind(&ActHasher::FileOpenedSlot, this, _1));
	connection_hook.SetPacketLength(0xA004, 9);
}

void ActHasher::Dispose()
{
}

void ActHasher::FileOpenedSlot(CFile *file)
{
	if (mode_mgr_hook.GetMode() != ModeMgrHook::GAME_MODE)
	{
		return;
	}

	char buffer[128];

	if (!strstr(file->file_name, session_hook.GetJobActName(session_hook.GetJob(), session_hook.GetSex(), buffer)))
	{
		return;
	}

	CryptoPP::CRC32().CalculateDigest(reinterpret_cast<byte*>(buffer+5), reinterpret_cast<byte*>(file->buffer), file->size);

	*reinterpret_cast<uint16_t*>(buffer) = 0xA004;
	*reinterpret_cast<uint16_t*>(buffer+2) = session_hook.GetJob();
	*reinterpret_cast<uint8_t*>(buffer+4) = session_hook.GetSex();

	connection_hook.SendPacket(reinterpret_cast<uint8_t*>(buffer));
}