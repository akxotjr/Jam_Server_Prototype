#pragma once
#include "StaticActor.h"


class Floor : public StaticActor
{
	using Super = StaticActor;

public:
	Floor() = default;
	virtual ~Floor() override = default;

	virtual void					Init(RoomRef room) override;
	virtual void					Update() override;
	virtual Protocol::Transform*	GetTransform() override;
};

