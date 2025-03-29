#pragma once

class Room;

class RoomManager
{
public:
	RoomManager() {};
	~RoomManager() {};

	void Update();

	uint32 CreateRoom();
	void AddRoom(RoomRef room);
	void RemoveRoom(RoomRef room);

	RoomRef GetRoomById(uint32 id) { return _rooms[id]; }

private:
	unordered_map<uint32, RoomRef> _rooms;
};

extern RoomManager GRoomManager;
