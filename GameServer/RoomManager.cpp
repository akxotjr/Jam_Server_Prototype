#include "pch.h"
#include "RoomManager.h"
#include "Room.h"

RoomManager GRoomManager;

void RoomManager::Add(RoomRef room)
{
	int32 id = room->GetId();

	_rooms.insert({ id, room });
}

void RoomManager::Remove(RoomRef room)
{
	int32 id = room->GetId();

	_rooms.erase(id);
}