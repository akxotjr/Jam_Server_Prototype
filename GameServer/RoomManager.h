#pragma once

class Room;

class RoomManager
{
	DECLARE_SINGLETON(RoomManager)

public:
	void							Init(physx::PxFoundation* foundation);
	void							Update();

	uint32							CreateRoom();

	void							AddRoom(RoomRef room);
	void							RemoveRoom(RoomRef room);

	RoomRef							GetRoomById(uint32 id) { return _rooms[id]; }

	physx::PxPhysics*				GetPxPhysics() const { return _physics; }

private:
	unordered_map<uint32, RoomRef>	_rooms;	// key : id, value : room

	physx::PxPhysics*				_physics;
};
