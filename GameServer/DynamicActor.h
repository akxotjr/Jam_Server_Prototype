#pragma once
#include "Actor.h"

class DynamicActor : public Actor
{
	using Super = Actor;

public:
	DynamicActor() = default;
	virtual ~DynamicActor() override = default;

	virtual void					Init(RoomRef room) override;
	virtual void					Update() override;
	virtual	Protocol::Transform		GetTransform() const override;

protected:
	physx::PxVec3					_horizontalVelocity = { 0.0f, 0.0f, 0.0f };
	float							_verticalVelocity = 0.0f;

	float							_moveSpeed = 10.f;
	float							_yawSpeed = 0.0f;
};

