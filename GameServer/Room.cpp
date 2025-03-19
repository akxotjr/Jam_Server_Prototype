#include "pch.h"
#include "GameSession.h"
#include "Room.h"
#include "Player.h"


void Room::Enter(PlayerRef player)
{
	_players[player->_playerId] = player;
}

void Room::Leave(PlayerRef player)
{
	_players.erase(player->_playerId);
}

void Room::Broadcast(SendBufferRef sendBuffer)
{
	for (auto& p : _players)
	{
		p.second->_gameSession->Send(sendBuffer);
	}
}