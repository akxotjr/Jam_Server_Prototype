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

Protocol::RoomList* RoomManager::GetRoomList()
{
	Protocol::RoomList* roomList = new Protocol::RoomList();

	for (auto& [roomId, room] : _rooms)
	{
		Protocol::RoomInfo* roomInfo = roomList->add_roominfo();
		roomInfo->set_roomid(roomId);

		const Vector<uint32>& playerList = room->GetPlayerList();

		for (auto& userId : playerList)
		{
			roomInfo->add_playerlist(userId);
		}
	}

	return roomList;
}
