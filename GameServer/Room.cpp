#include "pch.h"
#include "GameTcpSession.h"
#include "Room.h"
#include "Character.h"
#include "Player.h"
#include "Bot.h"
#include "TimeManager.h"
#include "ClientPacketHandler.h"
#include "SendBuffer.h"

void Room::Update()
{
	for (auto& [id, character] : _characters)
	{
		if (character)
		{
			character->Update();
		}
	}

	float deltaTime = GTimeManager.GetDeltaTime();
	_sumTime += deltaTime;

	if (_sumTime >= 0.05f) 
	{
		BroadCastCharacterSync();
		_sumTime = 0.f;
	}
}

void Room::Enter(PlayerRef player)
{
	WRITE_LOCK
	_players[player->GetInfo()->id()] = player;
	_characters[player->GetInfo()->id()] = player;
}

void Room::Leave(PlayerRef player)
{
	WRITE_LOCK
	_players.erase(player->GetInfo()->id());
}

void Room::Broadcast(SendBufferRef sendBuffer)
{
	for (auto& p : _players)
	{
		p.second->GetOwnerSession()->Send(sendBuffer);
	}
}

void Room::AddCharacter(CharacterRef character)
{
	WRITE_LOCK
	_characters[character->GetInfo()->id()] = character;
}

void Room::RemoveCharacter(CharacterRef character)
{
	WRITE_LOCK
	_characters.erase(character->GetInfo()->id());
}

void Room::BroadCastCharacterSync()
{
	float timestamp = GTimeManager.GetServerTime();

	for (auto& [id, player] : _players)
	{
		auto session = player->GetOwnerSession();

		Protocol::S_CHARACTER_SYNC pkt;

		pkt.set_timestamp(timestamp);

		for (auto& [i, character] : _characters)
		{
			Protocol::CharacterInfo* info = pkt.add_characterinfo();
			info->CopyFrom(*character->GetInfo()); 
		}

		auto sendBuffer = ClientPacketHandler::MakeSendBufferTcp(pkt);
		session->Send(sendBuffer);
	}
}

void Room::BroadcastSpawnActor()
{
	for (auto& [id, player] : _players)
	{
		if (!player)
		{
			continue;
		}

		auto session = player->GetOwnerSession();

		Protocol::S_SPAWN_ACTOR pkt;

		uint32 id = player->GetInfo()->id();
		pkt.set_playerid(id);

		for (auto& [i, character] : _characters)
		{
			Protocol::CharacterInfo* info = pkt.add_characterinfo();
			info->CopyFrom(*character->GetInfo());
		}

		auto sendBuffer = ClientPacketHandler::MakeSendBufferTcp(pkt);
		session->Send(sendBuffer);
	}
}



