#ifndef SYMPHONY4_ConnectionHook_H
#define SYMPHONY4_ConnectionHook_H

#include <map>
#include <utility>
#include <queue>
#include <boost/signal.hpp>
#include <boost/cstdint.hpp>
#include <boost/ptr_container/ptr_map.hpp>
#include "callchain.h"
#include "sessionhook.h"

using boost::uint8_t;
using boost::int8_t;
using boost::uint16_t;
using boost::int16_t;
using boost::uint32_t;
using boost::int32_t;
using boost::uint64_t;
using boost::int64_t;

struct CRagConnection;

class ConnectionHook
{
public:
	typedef boost::signal<void (CRagConnection*,const char*)> connected_signal_t;
	typedef boost::signal<bool (CRagConnection*,int,uint8_t*), CallChain> send_packet_signal_t;
	typedef boost::signal<bool (CRagConnection*,uint8_t*), CallChain> recv_packet_signal_t;

	void Init();
	void Dispose();

	int GetPacketLength(int id);
	void SetPacketLength(int id, int length);

	bool SendPacket(uint8_t *data);

	boost::signals::connection ConnectConnectedSlot(connected_signal_t::slot_function_type slot, int priority = 0) { return connected_signal_.connect(priority, slot); }
	boost::signals::connection ConnectSendPacketSlot(send_packet_signal_t::slot_function_type slot, int priority = 0) { return send_packet_signal_.connect(priority, slot); };
	boost::signals::connection ConnectRecvPacketSlot(recv_packet_signal_t::slot_function_type slot, int priority = 0) { return recv_packet_signal_.connect(priority, slot); };
	boost::signals::connection ConnectSendPacketIdSlot(uint16_t id, send_packet_signal_t::slot_function_type slot, int priority = 0) { return send_packet_id_signals_[id].connect(priority, slot); };
	boost::signals::connection ConnectRecvPacketIdSlot(uint16_t id, recv_packet_signal_t::slot_function_type slot, int priority = 0) { return recv_packet_id_signals_[id].connect(priority, slot); };

protected:
	void SessionCreatedSlot(CSession* session);
	bool SendPacketSlot(CRagConnection* connection, int length, uint8_t *data);
	bool SendPacketIdSlot(CRagConnection* connection, int length, uint8_t *data);
	bool RecvPacketIdSlot(CRagConnection* connection, uint8_t *buffer);

	int* (__thiscall *set_packet_size_pointer_)(void *packet_len_map, int* id);

	static bool __thiscall ConnectHook(CRagConnection* connection, char *address);
	bool (__thiscall *connect_pointer_)(CRagConnection* connection, char *address);
	static bool __thiscall SendConnectionHook(CRagConnection* connection, int length, uint8_t *data);
	bool (__thiscall *send_packet_pointer_)(CRagConnection* connection, int length, uint8_t *data);
	static bool __thiscall RecvConnectionHook(CRagConnection* connection, uint8_t *buffer);
	bool (__thiscall *recv_packet_pointer_)(CRagConnection* connection, uint8_t *buffer);

	void *packet_len_map_;
	bool packet_len_map_initialized_;
	CRagConnection *connection_;


	connected_signal_t connected_signal_;
	send_packet_signal_t send_packet_signal_;
	recv_packet_signal_t recv_packet_signal_;

	boost::ptr_map<uint16_t, send_packet_signal_t> send_packet_id_signals_;
	boost::ptr_map<uint16_t, recv_packet_signal_t> recv_packet_id_signals_;

	std::queue<std::pair<int,int>> queued_packet_lengths_;
};

extern ConnectionHook connection_hook;

#endif