#include "pch.h"
#include "ClientPacketHandler.h"
#include "TimeManager.h"
#include "GameUdpSession.h"
#include "SessionManager.h"
#include "Room.h"
#include "RoomManager.h"
#include "Player.h"
#include "Service.h"
#include "UserManager.h"

PacketHandlerFunc GPacketHandler_Tcp[UINT16_MAX];
PacketHandlerFunc GPacketHandler_Udp[UINT16_MAX];

bool Handle_INVALID(SessionRef& session, BYTE* buffer, int32 len)
{
	return true;
}

bool Handle_C_LOGIN(SessionRef& session, Protocol::C_LOGIN& pkt)
{
	std::cout << "[TCP] Recv : C_LOGIN\n";

	const string id = pkt.id();
	const string pw = pkt.pw();

	// userId == 0 -> failed to login
	// userId != 0 -> success to login
	uint32 userId = UserManager::Instance().Login(id, pw);	

	if (userId)
	{
		session->SetId(userId);
		SessionManager::Instance().Add(session);
	}

	Protocol::S_LOGIN loginPkt;
	loginPkt.set_userid(userId);
	auto sendBuffer = ClientPacketHandler::MakeSendBufferTcp(loginPkt);
	session->Send(sendBuffer);

	std::cout << "[TCP] Send : S_LOGIN\n";

	return true;
}

bool Handle_C_ENTER_GAME(SessionRef& session, Protocol::C_ENTER_GAME& pkt)
{
	std::cout << "[TCP] Recv : C_ENTER_GAME\n";

	PlayerRef player = MakeShared<Player>();
	player->SetUserId(session->GetId());

	auto room = RoomManager::Instance().GetRoomByRoomId(player->GetUserId());
	if (!room)
		return false;

	room->Enter(player);

	NetAddress udpAddress = session->GetService()->GetUdpNetAddress();
	wstring wip = udpAddress.GetIpAddress();
	string ip;
	ip.assign(wip.begin(), wip.end());
	uint32 port = udpAddress.GetPort();

	{
		Protocol::S_ENTER_GAME enterGamePkt;
		enterGamePkt.set_success(true);
		enterGamePkt.set_ip(ip);
		enterGamePkt.set_port(port);

		SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBufferTcp(enterGamePkt);
		session->Send(sendBuffer);

		std::cout << "[TCP] Send : S_ENTER_GAME\n";
	}
	return true;
}

bool Handle_C_ACK(SessionRef& session, Protocol::C_ACK& pkt)
{
	//std::cout << "[UDP] Recv : C_ACK\n";
	auto udpSession = static_pointer_cast<GameUdpSession>(session);
	if (udpSession == nullptr)
		return false;

	uint32 latestSeq = pkt.latestsequence();
	uint32 bitfield = pkt.bitfield();

	udpSession->HandleAck(latestSeq, bitfield);
	return true;
}

bool Handle_C_HANDSHAKE(SessionRef& session, Protocol::C_HANDSHAKE& pkt)
{
	std::cout << "[UDP] Recv : C_HANDSHAKE\n";

	uint32 userId = pkt.userid();
	session->SetId(userId);
	SessionManager::Instance().Add(session);

	auto udpSession = static_pointer_cast<GameUdpSession>(session);
	if (udpSession == nullptr) 
		return false;


	{
		double timestamp = TimeManager::Instance().GetServerTime();

		Protocol::S_HANDSHAKE handshakePkt;
		handshakePkt.set_success(true);

		auto sendBuffer = ClientPacketHandler::MakeSendBufferUdp(handshakePkt);
		udpSession->SendReliable(sendBuffer, timestamp);

		std::cout << "[UDP] Send : S_HANDSHAKE\n";
	}

	return true;
}

bool Handle_C_CHAT(SessionRef& session, Protocol::C_CHAT& pkt)
{
	std::cout << "[TCP] Recv : C_CHAT\n";
	auto msg = pkt.msg();

	cout << msg << endl;

	return true;
}

bool Handle_C_SYNC_TIME(SessionRef& session, Protocol::C_SYNC_TIME& pkt)
{
	std::cout << "[TCP] Recv : C_TIMESYNC\n";

	double timestamp = TimeManager::Instance().GetServerTime();

	Protocol::S_SYNC_TIME timePkt;
	timePkt.set_timestamp(timestamp);
	SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBufferTcp(timePkt);

	session->Send(sendBuffer);

	std::cout << "[TCP] Send : C_TIMESYNC\n";

	return true;
}

bool Handle_C_SPAWN_ACTOR(SessionRef& session, Protocol::C_SPAWN_ACTOR& pkt)
{
	std::cout << "[UDP] Recv : C_SPAWN_ACTOR\n";

	uint32 userId = session->GetId();

	auto room = RoomManager::Instance().GetRoomByUserId(userId);	//temp
	room->DoAsync(&Room::MulticastSpawnActor);

	return true;
}

bool Handle_C_SYNC_ACTOR(SessionRef& session, Protocol::C_SYNC_ACTOR& pkt)
{
	std::cout << "[UDP] Recv : C_CHARACTER_SYNC\n";
	return true;
}

bool Handle_C_PLAYER_INPUT(SessionRef& session, Protocol::C_PLAYER_INPUT& pkt)
{
	std::cout << "[UDP] Recv : C_PLAYER_INPUT\n";

	uint32 seq = pkt.sequence();
	uint32 keyField = pkt.keyfield();

	float mouseX = 0.f;
	float mouseY = 0.f;

	if (pkt.has_mouseposx() && pkt.has_mouseposy())
	{
		mouseX = pkt.mouseposx();
		mouseY = pkt.mouseposy();
	}

	uint32 userId = session->GetId();

	if (auto room = RoomManager::Instance().GetRoomByUserId(userId))
	{
		if (auto player = room->GetPlayerByUserId(userId))
		{
			player->ProcessInput(keyField, mouseX, mouseY, seq);

			return true;
		}
	}

	return false;
}
