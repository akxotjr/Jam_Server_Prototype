#pragma once
#include "Protocol.pb.h"

using namespace core::network;
using PacketHandlerFunc = std::function<bool(SessionRef&, BYTE*, int32)>;
extern PacketHandlerFunc GPacketHandler_Tcp[UINT16_MAX];
extern PacketHandlerFunc GPacketHandler_Udp[UINT16_MAX];

enum : uint16
{
{%- for pkt in parser.total_pkt %}
	PKT_{{pkt.name}} = {{pkt.id}},
{%- endfor %}
};

// Custom Handlers
bool Handle_INVALID(SessionRef& session, BYTE* buffer, int32 len);

{%- for pkt in parser.recv_pkt %}
bool Handle_{{pkt.name}}(SessionRef& session, Protocol::{{pkt.name}}& pkt);
{%- endfor %}

class {{output}}
{
public:
	static void Init()
	{
		for (int32 i = 0; i < UINT16_MAX; i++)
		{
			GPacketHandler_Tcp[i] = Handle_INVALID;
			GPacketHandler_Udp[i] = Handle_INVALID;
		}

		{%- for pkt in parser.recv_pkt %}
		GPacketHandler_Tcp[PKT_{{pkt.name}}] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<TcpPacketHeader, Protocol::{{pkt.name}}> (Handle_{{pkt.name}}, session, buffer, len); };
		GPacketHandler_Udp[PKT_{{pkt.name}}] = [](SessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<UdpPacketHeader, Protocol::{{pkt.name}}> (Handle_{{pkt.name}}, session, buffer, len); };
		{%- endfor %}
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
				ReliableUdpSessionRef udpSession = static_pointer_cast<ReliableUdpSession>(session);
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

	{%- for pkt in parser.send_pkt %}
	static SendBufferRef MakeSendBufferTcp(Protocol::{{pkt.name}}& pkt) { return MakeSendBufferImpl<TcpPacketHeader>(pkt, PKT_{{pkt.name}}); }
	static SendBufferRef MakeSendBufferUdp(Protocol::{{pkt.name}}& pkt) { return MakeSendBufferImpl<UdpPacketHeader>(pkt, PKT_{{pkt.name}}); }
	{%- endfor %}

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
