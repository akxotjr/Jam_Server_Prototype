#pragma once

class Room;

class RoomManager
{
	DECLARE_SINGLETON(RoomManager)

public:
	void								Init(physx::PxFoundation* foundation);
	void								Update();

	RoomRef								CreateRoom();

	void								AddRoom(RoomRef room);
	void								RemoveRoom(RoomRef room);

	RoomRef								GetRoomByRoomId(uint32 roomId) { return _rooms[roomId]; }
	RoomRef								GetRoomByUserId(uint32 userId);

	physx::PxPhysics*					GetPxPhysics() const { return _physics; }

private:
	unordered_map<uint32, RoomRef>		_rooms;	// key : roomId, value : room

	physx::PxPhysics*					_physics;
};
