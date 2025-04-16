#include "pch.h"
#include "ClientPacketHandler.h"
#include "TimeManager.h"
#include "GameTcpSession.h"
#include "GameUdpSession.h"
#include "GameUdpReceiver.h"
#include "Room.h"
#include "RoomManager.h"
#include "IdManager.h"
#include "Player.h"
#include "Bot.h"
#include "Service.h"

PacketHandlerFunc GPacketHandler_Tcp[UINT16_MAX];
PacketHandlerFunc GPacketHandler_Udp[UINT16_MAX];

bool Handle_INVALID(SessionRef& session, BYTE* buffer, int32 len)
{
	return true;
}

bool Handle_C_LOGIN(SessionRef& session, Protocol::C_LOGIN& pkt)
{
	GameTcpSessionRef gameTcpSession = static_pointer_cast<GameTcpSession>(session);

	{
		if (gameTcpSession->_currentPlayer)
		{
			return false;
		}

		PlayerRef player = MakeShared<Player>();
		player->SetOwnerSession(gameTcpSession);
		player->Init();

		gameTcpSession->_currentPlayer = player;
	}


	Protocol::S_LOGIN loginPkt;
	loginPkt.set_success(true);

	SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBufferTcp(loginPkt);
	session->Send(sendBuffer);

	return true;
}

bool Handle_C_ENTER_GAME(SessionRef& session, Protocol::C_ENTER_GAME& pkt)
{
	GameTcpSessionRef gameSession = static_pointer_cast<GameTcpSession>(session);

	auto& player = gameSession->_currentPlayer;

	if (player == nullptr)
	{
		return false;
	}

	gameSession->_room = GRoomManager.GetRoomById(1);	// temp
	gameSession->_room.lock()->DoAsync(&Room::Enter, gameSession->_currentPlayer);


	auto service = gameSession->GetService();
	if (service == nullptr) return false;

	//service->SetAndStartUdpReceiver(MakeShared<GameUdpReceiver>());

	NetAddress udpAddr =  service->GetUdpNetAddress();
	if (udpAddr.IsValid() == false) return false;

	wstring a = udpAddr.GetIpAddress();
	uint32 b = udpAddr.GetPort();

	string ip;
	ip.assign(a.begin(), a.end());
	uint32 port = udpAddr.GetPort();

	{
		Protocol::S_ENTER_GAME enterGamePkt;
		enterGamePkt.set_success(true);
		enterGamePkt.set_ip(ip);
		enterGamePkt.set_port(port);

		SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBufferTcp(enterGamePkt);
		session->Send(sendBuffer);
	}
	return true;
}

bool Handle_C_ACK(SessionRef& session, Protocol::C_ACK& pkt)
{
	// TODO
	return true;
}

bool Handle_C_HANDSHAKE(SessionRef& session, Protocol::C_HANDSHAKE& pkt)
{
	//auto udpSession = dynamic_pointer_cast<ReliableUdpSessionRef>(session);
	//if (udpSession == nullptr) return false;

	{
		Protocol::S_HANDSHAKE handshakePkt;
		handshakePkt.set_success(true);

		auto sendBuffer = ClientPacketHandler::MakeSendBufferUdp(handshakePkt);
		session->Send(sendBuffer);
	}

	return true;
}

bool Handle_C_CHAT(SessionRef& session, Protocol::C_CHAT& pkt)
{
	auto msg = pkt.msg();

	cout << msg << endl;

	return true;
}

bool Handle_C_TIMESYNC(SessionRef& session, Protocol::C_TIMESYNC& pkt)
{
	// temp
	std::cout << "Handle C_TIMESYNK" << std::endl;

	float serverTime = GTimeManager.GetServerTime();

	Protocol::S_TIMESYNC timesyncPkt;
	timesyncPkt.set_timestamp(serverTime);
	SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBufferTcp(timesyncPkt);

	session->Send(sendBuffer);

	return true;
}

bool Handle_C_SPAWN_ACTOR(SessionRef& session, Protocol::C_SPAWN_ACTOR& pkt)
{
	GameTcpSessionRef gameSession = static_pointer_cast<GameTcpSession>(session);

	//auto& player = gameSession->_currentPlayer;

	//if (player == nullptr)
	//{
	//	return false;
	//}

	//auto& characters = gameSession->_room.lock()->GetCharacters();

	//Protocol::S_SPAWN_ACTOR spawnActorPkt;

	//for (auto& [id, character] : characters)
	//{
	//	Protocol::CharacterInfo* info = spawnActorPkt.add_characterinfo();
	//	info->CopyFrom(*character->GetInfo());
	//}

	//auto sendBuffer = ClientPacketHandler::MakeSendBuffer(spawnActorPkt);
	//session->Send(sendBuffer);

	gameSession->_room = GRoomManager.GetRoomById(1);	// temp
	gameSession->_room.lock()->BroadcastSpawnActor();
	

	return true;
}

bool Handle_C_CHARACTER_SYNC(SessionRef& session, Protocol::C_CHARACTER_SYNC& pkt)
{
	float timestamp = pkt.timestamp();
	Protocol::CharacterInfo info = pkt.characterinfo();

	uint32 id = info.id();


	
	return true;
}

bool Handle_C_PLAYER_INPUT(SessionRef& session, Protocol::C_PLAYER_INPUT& pkt)
{
	GameTcpSessionRef gameSession = static_pointer_cast<GameTcpSession>(session);

	float timestamp = pkt.timestamp();
	uint32 sequenceNumber = pkt.sequencenumber();
	Protocol::KeyType key = pkt.keytype();
	float deltaTime = pkt.deltatime();

	float mousePosX = -1.f;
	float mousePosY = -1.f;

	if (pkt.has_mouseposx())
		mousePosX = pkt.mouseposx();

	if (pkt.has_mouseposy())
		mousePosY = pkt.mouseposy();

	auto& player = gameSession->_currentPlayer;

	if (player == nullptr)
		return false; 

	player->ProcessPlayerInpuf(timestamp, sequenceNumber, key, deltaTime, Vec2(mousePosX, mousePosY));
	
	{
		Protocol::S_PLAYER_INPUT inputPkt;
		inputPkt.set_sequencenumber(player->GetLastSequence());
		auto characterInfo = inputPkt.mutable_characterinfo();
		characterInfo->CopyFrom(*player->GetInfo());

		auto sendBuffer = ClientPacketHandler::MakeSendBufferTcp(inputPkt);

		session->Send(sendBuffer);
	}

	return false;
}
