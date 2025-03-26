#include "pch.h"
#include "GameSession.h"
#include "Room.h"
#include "Player.h"
#include "Bot.h"
#include "TimeManager.h"


void Room::Update()
{
	for (auto& [id, player] : _players)
	{
		player->Update();
	}

	for (auto& [id, bot] : _bots)
	{
		bot->Update();
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
	_players[player->GetPlayerProto().id()] = player;
}

void Room::Leave(PlayerRef player)
{
	_players.erase(player->GetPlayerProto().id());
}

void Room::Broadcast(SendBufferRef sendBuffer)
{
	for (auto& p : _players)
	{
		p.second->GetOwnerSession()->Send(sendBuffer);
	}
}

void Room::AddBot(BotRef bot)
{
	_bots[bot->GetBotProto().id()] = bot;
}

void Room::RemoveBot(BotRef bot)
{
	_bots.erase(bot->GetBotProto().id());
}


