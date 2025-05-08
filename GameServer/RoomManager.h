#pragma once

class Room;

class RoomManager
{
	DECLARE_SINGLETON(RoomManager)

public:
	void								Init();
	void								Update();

	RoomRef								CreateRoom();

	void								AddRoom(RoomRef room);
	void								RemoveRoom(RoomRef room);

	RoomRef								GetRoomByRoomId(uint32 roomId) { return _rooms[roomId]; }
	RoomRef								GetRoomByUserId(uint32 userId);

private:
	unordered_map<uint32, RoomRef>		_rooms;	// key : roomId, value : room
};
