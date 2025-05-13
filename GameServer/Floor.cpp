#include "pch.h"
#include "Floor.h"
#include "PhysicsManager.h"
#include "IdManager.h"

Floor::Floor()
{
	_actorId = IdManager::Instance().Generate(IdType::Actor, ActorTypePrefix::Floor);
}

void Floor::Init(RoomRef room)
{
	_ownerRoom = room;

	_pxActor = PhysicsManager::Instance().CreatePlane(0.0f, 1.f, 0.0f, 0.0f);
	ASSERT_CRASH(_pxActor != nullptr)
}

void Floor::Update()
{
}

Protocol::Transform* Floor::GetTransform()
{
	return Super::GetTransform();
}
