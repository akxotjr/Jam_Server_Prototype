#pragma once
#include "Actor.h"

class DynamicActor : public Actor
{
	using Super = Actor;

public:
	DynamicActor();
	virtual ~DynamicActor() override;

	void Init();
	void Update();

protected:
	physx::PxVec3 _velocity = {};
	physx::PxQuat _rotation = {};

	//temp
	float _speed = 3.f;
};

