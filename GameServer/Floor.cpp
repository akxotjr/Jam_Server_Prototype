#include "pch.h"
#include "Floor.h"
#include "PhysicsManager.h"
#include "IdManager.h"

Floor::Floor()
{
	_actorId = IdManager::Instance().Generate(IdType::Actor, ActorTypePrefix::Floor);
	_colliderInfo = ColliderInfo::MakePlane();
	_color = { 0.7f, 0.7f, 0.7f };
}

void Floor::Init(RoomRef room)
{
	_ownerRoom = room;

	_pxActor = PhysicsManager::Instance().CreatePlane(0.0f, 1.f, 0.0f, 0.0f);
	ASSERT_CRASH(_pxActor != nullptr)

	_pxActor->userData = this;
}

void Floor::Update()
{
}

Protocol::Transform* Floor::GetTransform()
{
	return Super::GetTransform();
}
