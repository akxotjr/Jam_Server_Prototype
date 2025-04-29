#include "pch.h"
#include "StaticActor.h"
#include "RoomManager.h"

StaticActor::StaticActor()
{
}

StaticActor::~StaticActor()
{
}

void StaticActor::Init(RoomRef room)
{
	Super::Init(room);

	physx::PxPhysics* physics = RoomManager::Instance().GetPxPhysics();
	if (physics == nullptr)
		return;

	_pxActor = physics->createRigidStatic(physx::PxTransform(_position, _rotation));
}

void StaticActor::Update()
{
}
