#include "pch.h"
#include "RoomManager.h"
#include "Room.h"

void RoomManager::Init()
{
}

void RoomManager::Update()
{
	for (auto& room : _rooms | views::values)
	{
		room->Update();
	}
}

RoomRef RoomManager::CreateRoom()
{
	RoomRef room = MakeShared<Room>();
	room->Init();

	return room;
}

void RoomManager::AddRoom(RoomRef room)
{
	uint32 id = room->GetRoomId();

	_rooms.insert({ id, room });
}

void RoomManager::RemoveRoom(RoomRef room)
{
	uint32 id = room->GetRoomId();

	_rooms.erase(id);
}

RoomRef RoomManager::GetRoomByUserId(uint32 userId)
{
	for (auto& room : _rooms | views::values)
	{
		if (room->GetPlayerByUserId(userId))
			return room;
	}
	return nullptr;
}
