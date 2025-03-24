#include "pch.h"
#include "ClientPacketHandler.h"
#include "TimeManager.h"
#include "GameSession.h"
#include "Room.h"
#include "RoomManager.h"
#include "IdManager.h"
#include "Player.h"
#include "Bot.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	return true;
}

bool Handle_C_LOGIN(PacketSessionRef& session, Protocol::C_LOGIN& pkt)
{
	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);


	{
		Protocol::PlayerProto player;
		uint32 id = GIdManager.Generate(IdType::Actor);

		string name = "player_" + to_string(id);

		player.set_id(id);
		player.set_name(name);

		PlayerRef playerRef = MakeShared<Player>();
		playerRef->SetOwnerSession(gameSession);
		playerRef->SetPlayerProto(player);

		gameSession->_currentPlayer = playerRef;
	}

	Protocol::S_LOGIN loginPkt;
	loginPkt.set_success(true);

	SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(loginPkt);
	session->Send(sendBuffer);

	return true;
}

bool Handle_C_ENTER_GAME(PacketSessionRef& session, Protocol::C_ENTER_GAME& pkt)
{
	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);

	// todo
	gameSession->_room = GRoomManager.GetRoomById(1);	// temp
	gameSession->_room.lock()->DoAsync(&Room::Enter, gameSession->_currentPlayer);

	Protocol::S_ENTER_GAME enterGamePkt;
	enterGamePkt.set_success(true);
	enterGamePkt.set_startpositionx(400.f);
	enterGamePkt.set_startpositiony(300.f);

	SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(enterGamePkt);
	session->Send(sendBuffer);

	return true;
}

bool Handle_C_CHAT(PacketSessionRef& session, Protocol::C_CHAT& pkt)
{
	auto msg = pkt.msg();

	cout << msg << endl;

	return true;
}

bool Handle_C_TIMESYNC(PacketSessionRef& session, Protocol::C_TIMESYNC& pkt)
{
	// temp
	std::cout << "Handle C_TIMESYNK" << std::endl;

	float serverTime = GTimeManager.GetServerTime();

	Protocol::S_TIMESYNC timesyncPkt;
	timesyncPkt.set_timestamp(serverTime);
	SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(timesyncPkt);

	session->Send(sendBuffer);

	return true;
}

bool Handle_C_SPAWN_ACTOR(PacketSessionRef& session, Protocol::C_SPAWN_ACTOR& pkt)
{


	return true;
}

bool Handle_C_CHARACTER_SYNC(PacketSessionRef& session, Protocol::C_CHARACTER_SYNC& pkt)
{
	return true;
}