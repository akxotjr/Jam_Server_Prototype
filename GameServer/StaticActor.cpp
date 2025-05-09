#include "pch.h"
#include "StaticActor.h"
#include "PhysicsManager.h"
#include "Room.h"

StaticActor::StaticActor()
{
}

StaticActor::~StaticActor()
{
}

void StaticActor::Init(RoomRef room)
{
	Super::Init(room);

	GetOwnerRoom()->_physicsQueue->Push(job::Job([this]()
		{
			_pxActor = PhysicsManager::Instance().CreateRigidStatic(_position, _rotation);
		}));
}

void StaticActor::Update()
{
}
