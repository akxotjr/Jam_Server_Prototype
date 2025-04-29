#pragma once
#include "Actor.h"

class DynamicActor : public Actor
{
	using Super = Actor;

public:
	DynamicActor();
	virtual ~DynamicActor() override;

	void		Init(RoomRef room) override;
	void		Update() override;

protected:
	
};

