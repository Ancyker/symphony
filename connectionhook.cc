#include "connectionhook.h"
#include "signature.h"
#include <boost/bind.hpp>
#include <windows.h>
#include <madCHook.h>
#include <map>

ConnectionHook connection_hook;

void ConnectionHook::Init()
{
	packet_len_map_ = *reinterpret_cast<void**>(Signature("........E8........8B008D4DF0C74010060000008D45F450518D4DD8C745F081000000").getAddress());
	set_packet_size_pointer_ = reinterpret_cast<int *(__thiscall *)(void *,int *)>(0x587330);
	connect_pointer_ =  reinterpret_cast<bool (__thiscall *)(CRagConnection*,char *)>(Signature("558BEC5153568BF1578D7E..C646..008BCFE8........85C074..").getAddress());
	send_packet_pointer_ = reinterpret_cast<bool (__thiscall *)(CRagConnection *,int,uint8_t *)>(Signature("558BEC568BF1837E04FF74358A461884C074178B450C8B4D0850518D4E..E8........B0015E5DC20800").getAddress());
	recv_packet_pointer_ = reinterpret_cast<bool (__thiscall *)(CRagConnection *,uint8_t *)>(Signature("558BEC83EC30538D45FC8D5940506A028BCBE8........84C00F84F80000008B").getAddress());

	connection_ = 0;
	packet_len_map_initialized_ = false;

	session_hook.ConnectCreatedSlot(boost::bind(&ConnectionHook::SessionCreatedSlot, this, _1));
	send_packet_signal_.connect(50, boost::bind(&ConnectionHook::SendPacketSlot, this, _1, _2, _3));
	send_packet_signal_.connect(25, boost::bind(&ConnectionHook::SendPacketIdSlot, this, _1, _2, _3));
	recv_packet_signal_.connect(100, boost::bind(&ConnectionHook::RecvPacketIdSlot, this, _1, _2));

	HookCode(connect_pointer_, ConnectHook, (PVOID*)&connect_pointer_);
	HookCode(send_packet_pointer_, SendConnectionHook, (PVOID*)&send_packet_pointer_);
	HookCode(recv_packet_pointer_, RecvConnectionHook, (PVOID*)&recv_packet_pointer_);
}

void ConnectionHook::Dispose()
{
	UnhookCode((PVOID*)&connect_pointer_);
	UnhookCode((PVOID*)&send_packet_pointer_);
	UnhookCode((PVOID*)&recv_packet_pointer_);
}

int ConnectionHook::GetPacketLength(int id)
{
	return *set_packet_size_pointer_(packet_len_map_, &id);
}

void ConnectionHook::SetPacketLength(int id, int length)
{
	if (!packet_len_map_initialized_)
	{
		queued_packet_lengths_.push(std::pair<int,int>(id,length));
		return;
	}

	*set_packet_size_pointer_(packet_len_map_, &id) = length;
}

void ConnectionHook::SessionCreatedSlot(CSession *session)
{
	connection_hook.packet_len_map_initialized_ = true;

	while (!connection_hook.queued_packet_lengths_.empty())
	{
		connection_hook.SetPacketLength(connection_hook.queued_packet_lengths_.front().first, connection_hook.queued_packet_lengths_.front().second);
		connection_hook.queued_packet_lengths_.pop();
	}

}

bool ConnectionHook::ConnectHook(CRagConnection *connection, char *address)
{
	connection_hook.connection_ = connection;

	if (!connection_hook.connect_pointer_(connection, address))
	{
		return false;
	}

	if (connection_hook.connected_signal_.num_slots())
	{
		connection_hook.connected_signal_(connection, address);
	}

	return true;
}

bool ConnectionHook::SendPacket(uint8_t *data)
{
	if (!connection_)
	{
		return false;
	}

	uint16_t id = *reinterpret_cast<uint16_t*>(data);

	int packet_length = GetPacketLength(id);

	if (packet_length == -1)
	{
		packet_length = *reinterpret_cast<uint16_t*>(data+2);
	}

	return send_packet_signal_(connection_, packet_length, data);
}

bool ConnectionHook::SendPacketSlot(CRagConnection* connection, int length, uint8_t *data)
{
	return send_packet_pointer_(connection, length, data);
}

bool ConnectionHook::SendPacketIdSlot(CRagConnection* connection, int length, uint8_t *data)
{
	uint16_t id = *reinterpret_cast<uint16_t*>(data);

	if (send_packet_id_signals_.count(id))
	{
		return send_packet_id_signals_[id](connection, length, data);
	}

	return false;
}

bool ConnectionHook::SendConnectionHook(CRagConnection* connection, int length, uint8_t *data)
{
	if (!connection_hook.connection_)
	{
		connection_hook.connection_ = connection;
	}

	return connection_hook.send_packet_signal_(connection, length, data);
}

bool ConnectionHook::RecvPacketIdSlot(CRagConnection* connection, uint8_t *buffer)
{
	uint16_t id = *reinterpret_cast<uint16_t*>(buffer);

	if (recv_packet_id_signals_.count(id))
	{
		return recv_packet_id_signals_[id](connection, buffer);
	}

	return false;
}

bool ConnectionHook::RecvConnectionHook(CRagConnection* connection, uint8_t *buffer)
{
	if (!connection_hook.connection_)
	{
		connection_hook.connection_ = connection;
	}

	bool result = connection_hook.recv_packet_pointer_(connection, buffer);

	if (!result)
	{
		return result;
	}

	return !connection_hook.recv_packet_signal_(connection, buffer);
}