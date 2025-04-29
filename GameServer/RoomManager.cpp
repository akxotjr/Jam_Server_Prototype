#include "pch.h"
#include "RoomManager.h"
#include "Room.h"
#include "IdManager.h"

void RoomManager::Init(physx::PxFoundation* foundation)
{
	_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, physx::PxTolerancesScale(), true, nullptr);
	if (_physics == nullptr) 
		CRASH("_physics is nullptr.")
}

void RoomManager::Update()
{
	for (auto& room : _rooms | views::values)
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