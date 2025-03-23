#include "pch.h"
#include "RoomManager.h"
#include "Room.h"

RoomManager GRoomManager;

uint32 RoomManager::CreateRoom()
{
	RoomRef room = MakeShared<Room>();
	room->SetId(_idGenerator);

	_rooms.insert({ room->GetId(), room});

	return _idGenerator.fetch_add(1);
}

void RoomManager::AddRoom(RoomRef room)
{
	uint32 id = room->GetId();

	_rooms.insert({ id, room });
}

void RoomManager::RemoveRoom(RoomRef room)
{
	uint32 id = room->GetId();

	_rooms.erase(id);
}