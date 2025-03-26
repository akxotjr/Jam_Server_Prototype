#include "pch.h"
#include "GameSession.h"
#include "Room.h"
#include "Character.h"
#include "Player.h"
#include "Bot.h"
#include "TimeManager.h"


void Room::Update()
{
	for (auto& [id, player] : _players)
	{
		player->Update();
	}

	for (auto& [id, character] : _characters)
	{
		character->Update();
	}

	float deltaTime = GTimeManager.GetDeltaTime();
	_sumTime += deltaTime;

	if (_sumTime > 0.5f)
	{
		// TODO
	}
}

void Room::Enter(PlayerRef player)
{
	_players[player->GetInfo().id()] = player;
}

void Room::Leave(PlayerRef player)
{
	_players.erase(player->GetInfo().id());
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
	_characters[character->GetInfo().id()] = character;
}

void Room::RemoveCharacter(CharacterRef character)
{
	_characters.erase(character->GetInfo().id());
}


