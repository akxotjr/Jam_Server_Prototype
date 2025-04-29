#include "pch.h"
#include "Actor.h"
#include "IdManager.h"

Actor::Actor()
{
	_id = GIdManager.Generate(IdType::Actor);
}

Actor::~Actor()
{
	_pxActor->release();
	_pxActor = nullptr;
}

void Actor::Init(RoomRef room)
{
	_ownerRoom = room;
}
