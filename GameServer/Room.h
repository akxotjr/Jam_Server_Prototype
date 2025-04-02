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
	void		AddCharacter(CharacterRef character);
	void		RemoveCharacter(CharacterRef character);

	void		BroadCastCharacterSync();
	void		BroadcastSpawnActor();

	unordered_map<uint32, CharacterRef>& GetCharacters() { return _characters; }

private:
	USE_LOCK
	uint32									_id;

	unordered_map<uint32, PlayerRef>		_players;
	unordered_map<uint32, CharacterRef>		_characters; // character doesn't have asession

	Atomic<float> _sumTime = 0.f;
};

