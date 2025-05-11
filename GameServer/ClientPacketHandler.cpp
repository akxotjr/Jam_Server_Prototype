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

bool Handle_C_CREATE_ROOM(SessionRef& session, Protocol::C_CREATE_ROOM& pkt)
{
	std::cout << "[TCP] Recv : C_CREATE_ROOM\n";

	auto room = RoomManager::Instance().CreateRoom();
	if (!room) return false;

	PlayerRef player = MakeShared<Player>();
	player->SetUserId(session->GetId());

	room->Enter(player);

	RoomManager::Instance().AddRoom(room);

	{
		Protocol::S_CREATE_ROOM createRoomPkt;
		createRoomPkt.set_success(true);
		createRoomPkt.set_roomid(room->GetRoomId());
		auto sendBuffer = ClientPacketHandler::MakeSendBufferTcp(createRoomPkt);
		session->Send(sendBuffer);

		std::cout << "[TCP] Send : S_CREATE_ROOM\n";
	}

	{
		Protocol::S_SYNC_ROOMLIST roomListPkt;
		roomListPkt.set_allocated_roomlist(RoomManager::Instance().GetRoomList());
		auto sendBuffer = ClientPacketHandler::MakeSendBufferTcp(roomListPkt);
		SessionManager::Instance().Multicast(ProtocolType::PROTOCOL_TCP, sendBuffer);

		std::cout << "[TCP] Multicast : S_SYNC_ROOMLIST\n";
	}

	return true;
}

bool Handle_C_ENTER_ROOM(SessionRef& session, Protocol::C_ENTER_ROOM& pkt)
{
	std::cout << "[TCP] Recv : C_ENTER_ROOM\n";

	uint32 roomId = pkt.roomid();
	auto room = RoomManager::Instance().GetRoomByRoomId(roomId);
	if (!room)
	{
		std::cout << "[ERROR] can't find room\n";
		return false;
	}

	PlayerRef player = MakeShared<Player>();
	player->SetUserId(session->GetId());

	room->Enter(player);

	{
		Protocol::S_ENTER_ROOM enterRoomPkt;
		enterRoomPkt.set_success(true);
		enterRoomPkt.set_roomid(roomId);
		auto sendBuffer = ClientPacketHandler::MakeSendBufferTcp(enterRoomPkt);
		session->Send(sendBuffer);

		std::cout << "[TCP] Send : S_ENTER_ROOM\n";
	}

	{
		Protocol::S_SYNC_ROOMLIST roomListPkt;
		roomListPkt.set_allocated_roomlist(RoomManager::Instance().GetRoomList());
		auto sendBuffer = ClientPacketHandler::MakeSendBufferTcp(roomListPkt);
		SessionManager::Instance().Multicast(ProtocolType::PROTOCOL_TCP, sendBuffer);

		std::cout << "[TCP] Multicast : S_SYNC_ROOMLIST\n";
	}

	return true;
}

bool Handle_C_SYNC_ROOMLIST(SessionRef& session, Protocol::C_SYNC_ROOMLIST& pkt)
{
	return true;
}


bool Handle_C_ENTER_GAME(SessionRef& session, Protocol::C_ENTER_GAME& pkt)
{
	std::cout << "[TCP] Recv : C_ENTER_GAME\n";

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
		auto room = RoomManager::Instance().GetRoomByUserId(session->GetId());
		if (!room)
		{
			std::cout << "[ERROR] can't find room\n";
			return false;
		}

		room->Multicast(ProtocolType::PROTOCOL_TCP, sendBuffer);

		std::cout << "[TCP] Multicast : S_ENTER_GAME\n";
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
	//std::cout << "[TCP] Recv : C_TIMESYNC\n";

	double timestamp = TimeManager::Instance().GetServerTime();

	Protocol::S_SYNC_TIME timePkt;
	timePkt.set_timestamp(timestamp);
	SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBufferTcp(timePkt);

	session->Send(sendBuffer);

	//std::cout << "[TCP] Send : C_TIMESYNC\n";

	return true;
}

bool Handle_C_SPAWN_ACTOR(SessionRef& session, Protocol::C_SPAWN_ACTOR& pkt)
{
	std::cout << "[UDP] Recv : C_SPAWN_ACTOR\n";

	uint32 userId = session->GetId();

	auto room = RoomManager::Instance().GetRoomByUserId(userId);	//temp
	room->DoAsync(&Room::MulticastSpawnActor);

	room->SetIsReady(true);

	return true;
}

bool Handle_C_SYNC_ACTOR(SessionRef& session, Protocol::C_SYNC_ACTOR& pkt)
{
	return true;
}

bool Handle_C_PLAYER_INPUT(SessionRef& session, Protocol::C_PLAYER_INPUT& pkt)
{
	//std::cout << "[UDP] Recv : C_PLAYER_INPUT\n";

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
			std::cout << "[DEBUG] UserID = " << player->GetUserId() << "PlayerActorID = " << player->GetActorId() << "\n";
			return true;
		}
	}

	return false;
}
