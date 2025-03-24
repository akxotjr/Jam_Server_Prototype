#pragma once
#include "JobQueue.h"

class Player;

class Room : public JobQueue
{
public:
	void		Update();

	void		Enter(PlayerRef player);
	void		Leave(PlayerRef player);	// todo : add another version(by id)
	void		Broadcast(SendBufferRef sendBuffer);

	void		SetId(uint32 id) { _id = id; }
	uint32&		GetId() { return _id; }

	RoomRef		GetRoomRef() { return static_pointer_cast<Room>(shared_from_this()); }
	
	PlayerRef	GetPlayerById(int32 id) { return _players[id]; }

	// temp
	void		AddBot(BotRef bot);
	void		RemoveBot(BotRef bot);

private:
	uint32					_id;

	map<uint64, PlayerRef>	_players;
	map<uint64, BotRef>		_bots;

	float _sumTime = 0.f;
};

