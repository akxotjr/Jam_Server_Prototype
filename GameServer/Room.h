#pragma once
#include "JobQueue.h"
#include "Actor.h"

struct SnapshotEntity
{
	uint32 actorId;
	physx::PxVec3 position;
	physx::PxQuat rotation;
	ColliderInfo collider;
};

using Snapshot = std::vector<SnapshotEntity>;

class Room : public job::JobQueue
{
	friend class StaticActor;
	friend class DynamicActor;
	friend class CharacterActor;
	friend class Player;

public:
	Room();
	virtual ~Room();

	void					Init();
	void					Update();

	void					AddActor(ActorRef actor);
	void					RemoveActor(ActorRef actor);

	bool					Enter(PlayerRef player);
	void					Leave(PlayerRef player);

	void					Multicast(ProtocolType type, network::SendBufferRef sendBuffer, bool reliable = false, double timestamp = 0.0);
	void					MulticastSpawnActor();
	void					MulticastSyncActor();

	uint32&					GetRoomId() { return _roomId; }
	RoomRef					GetRoomRef() { return static_pointer_cast<Room>(shared_from_this()); }
	
	PlayerRef				GetPlayerByUserId(uint32 userId);

	physx::PxScene*			GetPxScene() const { return _pxScene; }

	Vector<uint32>			GetPlayerList();	// TODO : change to user's name

	void					SetIsReady(bool ready) { _isReady = ready; }

	void					CaptureSnapshot();
	void					AddSnapshot(double timestamp, Snapshot& snapshot);
	Snapshot*				FindSnapshot(double timestamp);

	physx::PxScene*			BuildRewindScene(Snapshot& snapshot);
	void					ClearRewindScene();

private:
	USE_LOCK

	uint32									_roomId = 0;

	bool									_isReady = false;

	unordered_map<uint32, PlayerRef>		_players;	/// key = userId, value = PlayerRef 
	unordered_map<uint32, ActorRef>			_actors;	/// key = actorId, value = ActorRef

	physx::PxScene*							_pxScene = nullptr;
	physx::PxScene*							_pxRewindScene = nullptr;
	physx::PxControllerManager*				_pxControllerManager = nullptr;

	shared_ptr<core::thread::LockQueue<job::Job>>		_physicsQueue;

	Vector<std::pair<double, Snapshot>>		_snapshotBuffer;	/// first - timestamp, second - Snapshot
};

