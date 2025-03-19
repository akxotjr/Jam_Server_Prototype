#pragma once
#include "JobQueue.h"

class Player;

class Room : public JobQueue
{
public:
	void Enter(PlayerRef player);
	void Leave(PlayerRef player);
	void Broadcast(SendBufferRef sendBuffer);

	void SetId(int32 id) { _id = id; }
	int32 GetId() { return _id; }

	RoomRef GetRoom() { return static_pointer_cast<Room>(shared_from_this()); }
	
	PlayerRef GetPlayerById(int32 id) { return _players[id]; }


private:
	int32					_id;
	map<uint64, PlayerRef>	_players;
};

