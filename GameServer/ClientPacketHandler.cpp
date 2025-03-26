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
		PlayerRef player = MakeShared<Player>();
		player->SetOwnerSession(gameSession);
		player->Init();

		gameSession->_currentPlayer = player;
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

	gameSession->_room = GRoomManager.GetRoomById(1);	// temp
	gameSession->_room.lock()->DoAsync(&Room::Enter, gameSession->_currentPlayer);

	Protocol::S_ENTER_GAME enterGamePkt;
	enterGamePkt.set_success(true);
	enterGamePkt.set_actortype(Protocol::ACTOR_TYPE_PLAYER);

	auto& player = gameSession->_currentPlayer;

	//enterGamePkt.set_allocated_characterinfo(player->GetInfo());
	enterGamePkt.mutable_characterinfo()->CopyFrom(*player->GetInfo());

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

bool Handle_C_PLAYER_INPUT(PacketSessionRef& session, Protocol::C_PLAYER_INPUT& pkt)
{
	return false;
}
