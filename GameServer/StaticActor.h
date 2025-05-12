#pragma once
#include "Actor.h"

class StaticActor : public Actor
{
	using Super = Actor;

public:
	StaticActor() = default;
	virtual ~StaticActor() override = default;

	virtual void					Init(RoomRef room) override;
	virtual void					Update() override;
	virtual Protocol::Transform*	GetTransform() override;
};

