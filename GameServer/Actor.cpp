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

Protocol::Transform* Actor::GetTransform()
{
	Protocol::Transform* transform = new Protocol::Transform;
	transform->set_position(TransformCompressor::PackPosition(_position.x, _position.y, _position.z));
	transform->set_velocity_speed(0);
	transform->set_rotation(TransformCompressor::PackRotation(_yaw, _pitch));
	return transform;
}

float Actor::GetYawFromPxQuat() const
{
	float siny = 2.0f * (_rotation.w * _rotation.y + _rotation.x * _rotation.z);
	float cosy = 1.0f - 2.0f * (_rotation.y * _rotation.y + _rotation.z * _rotation.z);
	return std::atan2(siny, cosy);
}

float Actor::GetPitchFromPxQuat() const
{
	float value = 2.0f * (_rotation.w * _rotation.x - _rotation.y * _rotation.z);
	value = std::clamp(value, -1.0f, 1.0f);
	return std::asin(value);
}
