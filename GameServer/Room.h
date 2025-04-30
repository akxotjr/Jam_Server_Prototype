#pragma once
#include "JobQueue.h"

class Player;

class Room : public job::JobQueue
{
	friend class CharacterActor;
	friend class Player;

public:
	Room();
	virtual ~Room();

	void					Init();
	void					Update();

	void					AddActor(ActorRef actor);
	void					RemoveActor(ActorRef actor);

	void					Enter(PlayerRef player);
	void					Leave(PlayerRef player);
	void					Multicast(ProtocolType type, network::SendBufferRef sendBuffer, bool reliable = false);

	void					MulticastActorSync();

	uint32&		GetId() { return _id; }

	RoomRef		GetRoomRef() { return static_pointer_cast<Room>(shared_from_this()); }
	
	PlayerRef	GetPlayerById(int32 id) { return _players[id]; }

	void		BroadCastCharacterSync();
	void		BroadcastSpawnActor();


	physx::PxScene* GetPxScene() const { return _scene; }

private:
	USE_LOCK

	uint32									_id = 0;

	unordered_map<uint32, PlayerRef>		_players;
	unordered_map<uint32, ActorRef>			_actors;

	physx::PxScene*							_scene = nullptr;
	physx::PxControllerManager*				_controllerManager = nullptr;
};

