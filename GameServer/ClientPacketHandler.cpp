#include "pch.h"
#include "ClientPacketHandler.h"
#include "TimeManager.h"
#include "GameTcpSession.h"
#include "GameUdpSession.h"
#include "GameUdpReceiver.h"
#include "SessionManager.h"
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
	std::cout << "[TCP] Recv : C_LOGIN\n";

	GameTcpSessionRef gameTcpSession = static_pointer_cast<GameTcpSession>(session);

	// 일단은 그냥 Session Id 사용 나중에 db 붙여서 userId 로
	uint32 id = GIdManager.Generate(IdType::Session);
	gameTcpSession->SetId(id);
	GSessionManager.Add(session);

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
	loginPkt.set_userid(id);

	SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBufferTcp(loginPkt);
	session->Send(sendBuffer);

	std::cout << "[TCP] Send : S_LOGIN\n";

	return true;
}

bool Handle_C_ENTER_GAME(SessionRef& session, Protocol::C_ENTER_GAME& pkt)
{
	std::cout << "[TCP] Recv : C_ENTER_GAME\n";
	GameTcpSessionRef gameTcpSession = static_pointer_cast<GameTcpSession>(session);

	auto& player = gameTcpSession->_currentPlayer;

	if (player == nullptr)
	{
		return false;
	}

	//temp
	gameTcpSession->SetRoom(GRoomManager.GetRoomById(1));
	gameTcpSession->GetRoom()->DoAsync(&Room::Enter, gameTcpSession->_currentPlayer);


	auto service = gameTcpSession->GetService();
	if (service == nullptr) return false;


	NetAddress udpAddr =  service->GetUdpNetAddress();
	if (udpAddr.IsValid() == false) return false;

	wstring wip = udpAddr.GetIpAddress();
	string ip;
	ip.assign(wip.begin(), wip.end());
	uint32 port = udpAddr.GetPort();

	{
		Protocol::S_ENTER_GAME enterGamePkt;
		enterGamePkt.set_success(true);
		enterGamePkt.set_ip(ip);
		enterGamePkt.set_port(port);

		SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBufferTcp(enterGamePkt);
		gameTcpSession->Send(sendBuffer);

		std::cout << "[TCP] Send : S_ENTER_GAME\n";
	}
	return true;
}

bool Handle_C_ACK(SessionRef& session, Protocol::C_ACK& pkt)
{
	std::cout << "[UDP] Recv : C_ACK\n";
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

	uint32 id = pkt.userid();
	session->SetId(id);
	GSessionManager.Add(session);


	auto udpSession = static_pointer_cast<GameUdpSession>(session);
	if (udpSession == nullptr) 
		return false;



	{
		float timestamp = GTimeManager.GetServerTime();

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

bool Handle_C_TIMESYNC(SessionRef& session, Protocol::C_TIMESYNC& pkt)
{
	//std::cout << "[TCP] Recv : C_TIMESYNC\n";

	float serverTime = GTimeManager.GetServerTime();

	Protocol::S_TIMESYNC timesyncPkt;
	timesyncPkt.set_timestamp(serverTime);
	SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBufferTcp(timesyncPkt);

	session->Send(sendBuffer);

	//std::cout << "[TCP] Send : C_TIMESYNC\n";

	return true;
}

bool Handle_C_SPAWN_ACTOR(SessionRef& session, Protocol::C_SPAWN_ACTOR& pkt)
{
	std::cout << "[UDP] Recv : C_SPAWN_ACTOR\n";

	GameUdpSessionRef gameUdpSession = static_pointer_cast<GameUdpSession>(session);

	uint32 id = gameUdpSession->GetId();

	auto gameTcpSession = static_pointer_cast<GameTcpSession>(GSessionManager.GetSessionById(ProtocolType::PROTOCOL_TCP, id));
	gameTcpSession->GetRoom()->BroadcastSpawnActor();

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

	//gameSession->_room = GRoomManager.GetRoomById(1);	// temp
	//gameSession->_room.lock()->BroadcastSpawnActor();
	

	return true;
}

bool Handle_C_CHARACTER_SYNC(SessionRef& session, Protocol::C_CHARACTER_SYNC& pkt)
{
	std::cout << "[UDP] Recv : C_CHARACTER_SYNC\n";

	float timestamp = pkt.timestamp();
	Protocol::CharacterInfo info = pkt.characterinfo();

	uint32 id = info.id();


	
	return true;
}

bool Handle_C_PLAYER_INPUT(SessionRef& session, Protocol::C_PLAYER_INPUT& pkt)
{
	std::cout << "[UDP] Recv : C_PLAYER_INPUT\n";

	GameUdpSessionRef gameUdpSession = static_pointer_cast<GameUdpSession>(session);
	auto gameTcpSession = static_pointer_cast<GameTcpSession>(GSessionManager.GetSessionById(ProtocolType::PROTOCOL_TCP, gameUdpSession->GetId()));

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

	auto& player = gameTcpSession->_currentPlayer;

	if (player == nullptr)
		return false; 

	player->ProcessPlayerInpuf(timestamp, sequenceNumber, key, deltaTime, Vec2(mousePosX, mousePosY));
	
	{
		Protocol::S_PLAYER_INPUT inputPkt;
		inputPkt.set_sequencenumber(player->GetLastSequence());
		auto characterInfo = inputPkt.mutable_characterinfo();
		characterInfo->CopyFrom(*player->GetInfo());

		auto sendBuffer = ClientPacketHandler::MakeSendBufferUdp(inputPkt);

		float serverTime = GTimeManager.GetServerTime();

		gameUdpSession->SendReliable(sendBuffer, serverTime);
		std::cout << "[UDP] Send : S_PLAYER_INPUT\n";
	}

	return false;
}
