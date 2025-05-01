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

	virtual	Protocol::Transform		ToTransform() const override;

protected:
	physx::PxCapsuleController*		_controller;

	physx::PxVec3					_horizontalVelocity = {};
	float							_verticalVelocity = 0.0f;

	float							_moveSpeed = 3.f;
	float							_jumpSpeed = 3.f;
};

