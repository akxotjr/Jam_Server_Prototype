#pragma once
#include "Protocol.pb.h"

using namespace core::network;
using PacketHandlerFunc = std::function<bool(SessionRef&, BYTE*, int32)>;
extern PacketHandlerFunc GPacketHandler_Tcp[UINT16_MAX];
extern PacketHandlerFunc GPacketHandler_Udp[UINT16_MAX];

enum : uint16
{
	PKT_C_LOGIN = 1000,
	PKT_S_LOGIN = 1001,
	PKT_C_CREATE_ROOM = 1002,
	PKT_S_CREATE_ROOM = 1003,
	PKT_C_ENTER_ROOM = 1004,
	PKT_S_ENTER_ROOM = 1005,
	PKT_C_SYNC_ROOMLIST = 1006,
	PKT_S_SYNC_ROOMLIST = 1007,
	PKT_C_ENTER_GAME = 1008,
	PKT_S_ENTER_GAME = 1009,
	PKT_C_ACK = 1010,
	PKT_S_ACK = 1011,
	PKT_C_HANDSHAKE = 1012,
	PKT_S_HANDSHAKE = 1013,
	PKT_C_CHAT = 1014,
	PKT_S_CHAT = 1015,
	PKT_C_SYNC_TIME = 1016,
	PKT_S_SYNC_TIME = 1017,
	PKT_C_SPAWN_ACTOR = 1018,
	PKT_S_SPAWN_ACTOR = 1019,
	PKT_C_SYNC_ACTOR = 1020,
	PKT_S_SYNC_ACTOR = 1021,
	PKT_C_PLAYER_INPUT = 1022,
	PKT_S_PLAYER_INPUT = 1023,
	PKT_C_HIT_RESULT = 1024,
	PKT_S_HIT_RESULT = 1025,
};

// Custom Handlers
bool Handle_INVALID(SessionRef& session, BYTE* buffer, int32 len);
bool Handle_C_LOGIN(SessionRef& session, Protocol::C_LOGIN& pkt);
bool Handle_C_CREATE_ROOM(SessionRef& session, Protocol::C_CREATE_ROOM& pkt);
bool Handle_C_ENTER_ROOM(SessionRef& session, Protocol::C_ENTER_ROOM& pkt);
bool Handle_C_SYNC_ROOMLIST(SessionRef& session, Protocol::C_SYNC_ROOMLIST& pkt);
bool Handle_C_ENTER_GAME(SessionRef& session, Protocol::C_ENTER_GAME& pkt);
bool Handle_C_ACK(SessionRef& session, Protocol::C_ACK& pkt);
bool Handle_C_HANDSHAKE(SessionRef& session, Protocol::C_HANDSHAKE& pkt);
bool Handle_C_CHAT(SessionRef& session, Protocol::C_CHAT& pkt);
bool Handle_C_SYNC_TIME(SessionRef& session, Protocol::C_SYNC_TIME& pkt);
bool Handle_C_SPAWN_ACTOR(SessionRef& session, Protocol::C_SPAWN_ACTOR& pkt);
bool Handle_C_SYNC_ACTOR(SessionRef& session, Protocol::C_SYNC_ACTOR& pkt);
bool Handle_C_PLAYER_INPUT(SessionRef& session, Protocol::C_PLAYER_INPUT& pkt);
bool Handle_C_HIT_RESULT(SessionRef& session, Protocol::C_HIT_RESULT& pkt);

class ClientPacketHandler
{
public:
	static void Init()
	{
		for (int32 i = 0; i < UINT16_MAX; i++)
		{
			GPacketHandler_Tcp[i] = Handle_INVALID;
			GPacketHandler_Udp[i] = Handle_INVALID;
		}
		GPacketHandler_Tcp[PKT_C_LOGIN] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<TcpPacketHeader, Protocol::C_LOGIN> (Handle_C_LOGIN, session, buffer, len); };
		GPacketHandler_Udp[PKT_C_LOGIN] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<UdpPacketHeader, Protocol::C_LOGIN> (Handle_C_LOGIN, session, buffer, len); };
		GPacketHandler_Tcp[PKT_C_CREATE_ROOM] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<TcpPacketHeader, Protocol::C_CREATE_ROOM> (Handle_C_CREATE_ROOM, session, buffer, len); };
		GPacketHandler_Udp[PKT_C_CREATE_ROOM] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<UdpPacketHeader, Protocol::C_CREATE_ROOM> (Handle_C_CREATE_ROOM, session, buffer, len); };
		GPacketHandler_Tcp[PKT_C_ENTER_ROOM] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<TcpPacketHeader, Protocol::C_ENTER_ROOM> (Handle_C_ENTER_ROOM, session, buffer, len); };
		GPacketHandler_Udp[PKT_C_ENTER_ROOM] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<UdpPacketHeader, Protocol::C_ENTER_ROOM> (Handle_C_ENTER_ROOM, session, buffer, len); };
		GPacketHandler_Tcp[PKT_C_SYNC_ROOMLIST] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<TcpPacketHeader, Protocol::C_SYNC_ROOMLIST> (Handle_C_SYNC_ROOMLIST, session, buffer, len); };
		GPacketHandler_Udp[PKT_C_SYNC_ROOMLIST] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<UdpPacketHeader, Protocol::C_SYNC_ROOMLIST> (Handle_C_SYNC_ROOMLIST, session, buffer, len); };
		GPacketHandler_Tcp[PKT_C_ENTER_GAME] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<TcpPacketHeader, Protocol::C_ENTER_GAME> (Handle_C_ENTER_GAME, session, buffer, len); };
		GPacketHandler_Udp[PKT_C_ENTER_GAME] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<UdpPacketHeader, Protocol::C_ENTER_GAME> (Handle_C_ENTER_GAME, session, buffer, len); };
		GPacketHandler_Tcp[PKT_C_ACK] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<TcpPacketHeader, Protocol::C_ACK> (Handle_C_ACK, session, buffer, len); };
		GPacketHandler_Udp[PKT_C_ACK] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<UdpPacketHeader, Protocol::C_ACK> (Handle_C_ACK, session, buffer, len); };
		GPacketHandler_Tcp[PKT_C_HANDSHAKE] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<TcpPacketHeader, Protocol::C_HANDSHAKE> (Handle_C_HANDSHAKE, session, buffer, len); };
		GPacketHandler_Udp[PKT_C_HANDSHAKE] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<UdpPacketHeader, Protocol::C_HANDSHAKE> (Handle_C_HANDSHAKE, session, buffer, len); };
		GPacketHandler_Tcp[PKT_C_CHAT] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<TcpPacketHeader, Protocol::C_CHAT> (Handle_C_CHAT, session, buffer, len); };
		GPacketHandler_Udp[PKT_C_CHAT] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<UdpPacketHeader, Protocol::C_CHAT> (Handle_C_CHAT, session, buffer, len); };
		GPacketHandler_Tcp[PKT_C_SYNC_TIME] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<TcpPacketHeader, Protocol::C_SYNC_TIME> (Handle_C_SYNC_TIME, session, buffer, len); };
		GPacketHandler_Udp[PKT_C_SYNC_TIME] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<UdpPacketHeader, Protocol::C_SYNC_TIME> (Handle_C_SYNC_TIME, session, buffer, len); };
		GPacketHandler_Tcp[PKT_C_SPAWN_ACTOR] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<TcpPacketHeader, Protocol::C_SPAWN_ACTOR> (Handle_C_SPAWN_ACTOR, session, buffer, len); };
		GPacketHandler_Udp[PKT_C_SPAWN_ACTOR] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<UdpPacketHeader, Protocol::C_SPAWN_ACTOR> (Handle_C_SPAWN_ACTOR, session, buffer, len); };
		GPacketHandler_Tcp[PKT_C_SYNC_ACTOR] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<TcpPacketHeader, Protocol::C_SYNC_ACTOR> (Handle_C_SYNC_ACTOR, session, buffer, len); };
		GPacketHandler_Udp[PKT_C_SYNC_ACTOR] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<UdpPacketHeader, Protocol::C_SYNC_ACTOR> (Handle_C_SYNC_ACTOR, session, buffer, len); };
		GPacketHandler_Tcp[PKT_C_PLAYER_INPUT] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<TcpPacketHeader, Protocol::C_PLAYER_INPUT> (Handle_C_PLAYER_INPUT, session, buffer, len); };
		GPacketHandler_Udp[PKT_C_PLAYER_INPUT] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<UdpPacketHeader, Protocol::C_PLAYER_INPUT> (Handle_C_PLAYER_INPUT, session, buffer, len); };
		GPacketHandler_Tcp[PKT_C_HIT_RESULT] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<TcpPacketHeader, Protocol::C_HIT_RESULT> (Handle_C_HIT_RESULT, session, buffer, len); };
		GPacketHandler_Udp[PKT_C_HIT_RESULT] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<UdpPacketHeader, Protocol::C_HIT_RESULT> (Handle_C_HIT_RESULT, session, buffer, len); };
	}

	template<typename T>
	struct always_false : std::false_type {};

	template<typename HeaderType>
	static bool HandlePacket(SessionRef& session, BYTE* buffer, int32 len)
	{
		HeaderType* header = reinterpret_cast<HeaderType*>(buffer);

		if constexpr (std::is_same_v<HeaderType, TcpPacketHeader>)
		{
			return GPacketHandler_Tcp[header->id](session, buffer, len);
		}
		else if constexpr (std::is_same_v<HeaderType, UdpPacketHeader>)
		{
			uint16 seq = header->sequence;
			if (seq > 0)
			{
				UdpSessionRef udpSession = static_pointer_cast<UdpSession>(session);
				if (udpSession && udpSession->CheckAndRecordReceiveHistory(seq))
				{
					Protocol::S_ACK ackPkt;
					ackPkt.set_latestsequence(header->sequence);
					ackPkt.set_bitfield(udpSession->GenerateAckBitfield(seq));

					SendBufferRef sendBuffer = MakeSendBufferUdp(ackPkt);
					udpSession->Send(sendBuffer);
				}
			}

			return GPacketHandler_Udp[header->id](session, buffer, len);
		}
		else
		{
			ASSERT_CRASH(always_false<HeaderType>::value);
			return false;
		}
	}
	static SendBufferRef MakeSendBufferTcp(Protocol::S_LOGIN& pkt) { return MakeSendBufferImpl<TcpPacketHeader>(pkt, PKT_S_LOGIN); }
	static SendBufferRef MakeSendBufferUdp(Protocol::S_LOGIN& pkt) { return MakeSendBufferImpl<UdpPacketHeader>(pkt, PKT_S_LOGIN); }
	static SendBufferRef MakeSendBufferTcp(Protocol::S_CREATE_ROOM& pkt) { return MakeSendBufferImpl<TcpPacketHeader>(pkt, PKT_S_CREATE_ROOM); }
	static SendBufferRef MakeSendBufferUdp(Protocol::S_CREATE_ROOM& pkt) { return MakeSendBufferImpl<UdpPacketHeader>(pkt, PKT_S_CREATE_ROOM); }
	static SendBufferRef MakeSendBufferTcp(Protocol::S_ENTER_ROOM& pkt) { return MakeSendBufferImpl<TcpPacketHeader>(pkt, PKT_S_ENTER_ROOM); }
	static SendBufferRef MakeSendBufferUdp(Protocol::S_ENTER_ROOM& pkt) { return MakeSendBufferImpl<UdpPacketHeader>(pkt, PKT_S_ENTER_ROOM); }
	static SendBufferRef MakeSendBufferTcp(Protocol::S_SYNC_ROOMLIST& pkt) { return MakeSendBufferImpl<TcpPacketHeader>(pkt, PKT_S_SYNC_ROOMLIST); }
	static SendBufferRef MakeSendBufferUdp(Protocol::S_SYNC_ROOMLIST& pkt) { return MakeSendBufferImpl<UdpPacketHeader>(pkt, PKT_S_SYNC_ROOMLIST); }
	static SendBufferRef MakeSendBufferTcp(Protocol::S_ENTER_GAME& pkt) { return MakeSendBufferImpl<TcpPacketHeader>(pkt, PKT_S_ENTER_GAME); }
	static SendBufferRef MakeSendBufferUdp(Protocol::S_ENTER_GAME& pkt) { return MakeSendBufferImpl<UdpPacketHeader>(pkt, PKT_S_ENTER_GAME); }
	static SendBufferRef MakeSendBufferTcp(Protocol::S_ACK& pkt) { return MakeSendBufferImpl<TcpPacketHeader>(pkt, PKT_S_ACK); }
	static SendBufferRef MakeSendBufferUdp(Protocol::S_ACK& pkt) { return MakeSendBufferImpl<UdpPacketHeader>(pkt, PKT_S_ACK); }
	static SendBufferRef MakeSendBufferTcp(Protocol::S_HANDSHAKE& pkt) { return MakeSendBufferImpl<TcpPacketHeader>(pkt, PKT_S_HANDSHAKE); }
	static SendBufferRef MakeSendBufferUdp(Protocol::S_HANDSHAKE& pkt) { return MakeSendBufferImpl<UdpPacketHeader>(pkt, PKT_S_HANDSHAKE); }
	static SendBufferRef MakeSendBufferTcp(Protocol::S_CHAT& pkt) { return MakeSendBufferImpl<TcpPacketHeader>(pkt, PKT_S_CHAT); }
	static SendBufferRef MakeSendBufferUdp(Protocol::S_CHAT& pkt) { return MakeSendBufferImpl<UdpPacketHeader>(pkt, PKT_S_CHAT); }
	static SendBufferRef MakeSendBufferTcp(Protocol::S_SYNC_TIME& pkt) { return MakeSendBufferImpl<TcpPacketHeader>(pkt, PKT_S_SYNC_TIME); }
	static SendBufferRef MakeSendBufferUdp(Protocol::S_SYNC_TIME& pkt) { return MakeSendBufferImpl<UdpPacketHeader>(pkt, PKT_S_SYNC_TIME); }
	static SendBufferRef MakeSendBufferTcp(Protocol::S_SPAWN_ACTOR& pkt) { return MakeSendBufferImpl<TcpPacketHeader>(pkt, PKT_S_SPAWN_ACTOR); }
	static SendBufferRef MakeSendBufferUdp(Protocol::S_SPAWN_ACTOR& pkt) { return MakeSendBufferImpl<UdpPacketHeader>(pkt, PKT_S_SPAWN_ACTOR); }
	static SendBufferRef MakeSendBufferTcp(Protocol::S_SYNC_ACTOR& pkt) { return MakeSendBufferImpl<TcpPacketHeader>(pkt, PKT_S_SYNC_ACTOR); }
	static SendBufferRef MakeSendBufferUdp(Protocol::S_SYNC_ACTOR& pkt) { return MakeSendBufferImpl<UdpPacketHeader>(pkt, PKT_S_SYNC_ACTOR); }
	static SendBufferRef MakeSendBufferTcp(Protocol::S_PLAYER_INPUT& pkt) { return MakeSendBufferImpl<TcpPacketHeader>(pkt, PKT_S_PLAYER_INPUT); }
	static SendBufferRef MakeSendBufferUdp(Protocol::S_PLAYER_INPUT& pkt) { return MakeSendBufferImpl<UdpPacketHeader>(pkt, PKT_S_PLAYER_INPUT); }
	static SendBufferRef MakeSendBufferTcp(Protocol::S_HIT_RESULT& pkt) { return MakeSendBufferImpl<TcpPacketHeader>(pkt, PKT_S_HIT_RESULT); }
	static SendBufferRef MakeSendBufferUdp(Protocol::S_HIT_RESULT& pkt) { return MakeSendBufferImpl<UdpPacketHeader>(pkt, PKT_S_HIT_RESULT); }

private:
	template<typename HeaderType, typename PacketType, typename ProcessFunc>
	static bool HandlePacket(ProcessFunc func, SessionRef& session, BYTE* buffer, int32 len)
	{
		PacketType pkt;
		if (pkt.ParseFromArray(buffer + sizeof(HeaderType), len - sizeof(HeaderType)) == false)
			return false;

		return func(session, pkt);
	}

	template<typename HeaderType, typename T>
	static SendBufferRef MakeSendBufferImpl(T& pkt, uint16 pktId)
	{
		const uint16 dataSize = static_cast<uint16>(pkt.ByteSizeLong());
		const uint16 packetSize = dataSize + sizeof(HeaderType);

		SendBufferRef sendBuffer = SendBufferManager::Instance().Open(packetSize);
		HeaderType* header = reinterpret_cast<HeaderType*>(sendBuffer->Buffer());
		header->size = packetSize;
		header->id = pktId;

		BYTE* payload = reinterpret_cast<BYTE*>(header) + sizeof(HeaderType);
		ASSERT_CRASH(pkt.SerializeToArray(payload, dataSize));
		sendBuffer->Close(packetSize);

		return sendBuffer;
	}
};