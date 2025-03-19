#pragma once

class Room;

class RoomManager
{
public:
	RoomManager() {};
	~RoomManager() {};

	void Add(RoomRef room);
	void Remove(RoomRef room);
	void Broadcast(SendBufferRef sendBuffer);

private:
	unordered_map<int32, RoomRef> _rooms;
};

extern RoomManager GRoomManager;
