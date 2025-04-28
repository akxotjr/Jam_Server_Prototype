#pragma once
#include "Actor.h"

class CharacterActor : public Actor
{
	using Super = Actor;

public:
	CharacterActor();
	virtual ~CharacterActor() override;

	void Init();
	void Update();

protected:
	physx::PxVec3 _velocity = {};

	physx::PxVec3		_horizontalVelocity = {};	//TODO
	float				_verticalVelocity = 0.0f;
	physx::PxQuat		_rotation = {};

	//temp
	float _moveSpeed = 3.f;
};

