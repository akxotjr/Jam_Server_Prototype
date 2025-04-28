#pragma once
#include "Actor.h"

class StaticActor : public Actor
{
	using Super = Actor;

public:
	StaticActor();
	virtual ~StaticActor() override;

	virtual void Init();
	virtual void Update();
};

