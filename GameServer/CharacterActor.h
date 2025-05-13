#pragma once
#include "Actor.h"

class CharacterActor : public Actor
{
	using Super = Actor;

public:
	CharacterActor();
	virtual ~CharacterActor() override;

	virtual void					Init(RoomRef room) override;
	virtual void					Update() override;

	virtual	Protocol::Transform*	GetTransform() override;

protected:
	physx::PxCapsuleController*		_controller;

	physx::PxVec3					_horizontalVelocity = {0.0f, 0.0f, 0.0f};
	float							_verticalVelocity = 0.0f;

	float							_moveSpeed = 10.f;
	float							_yawSpeed = 0.0f;
};

