#include "pch.h"
#include "RoomManager.h"
#include "Room.h"
#include "IdManager.h"

RoomManager GRoomManager;

void RoomManager::Update()
{
	for (auto& [id, room] : _rooms)
	{
		room->Update();
	}
}

uint32 RoomManager::CreateRoom()
{
	RoomRef room = MakeShared<Room>();
	uint32 id = GIdManager.Generate(IdType::Room);
	room->SetId(id);

	_rooms.insert({ room->GetId(), room});

	return id;
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