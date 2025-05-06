#include "pch.h"
#include "Actor.h"
#include "TransformCompressor.h"

Actor::~Actor()
{
	if (_pxActor)
	{
		_pxActor->release();
		_pxActor = nullptr;
	}
}

void Actor::Init(RoomRef room)
{
	_ownerRoom = room;
}

Protocol::Transform Actor::ToTransform() const
{
	Protocol::Transform transform;
	transform.set_position(TransformCompressor::PackPosition(_position.x, _position.y, _position.z));
	transform.set_velocity_speed(0);
	transform.set_rotation(TransformCompressor::PackRotation(_rotation.x, _rotation.y, _rotation.z));
	return transform;
}
