#pragma once
#include "CharacterActor.h"


class Bot : public CharacterActor
{
	using Super = CharacterActor;

public:
	Bot();
	virtual ~Bot() override;

	virtual void					Init(RoomRef room) override;
	virtual void					Update() override;

	virtual	Protocol::Transform*	GetTransform() override;

	void							ToggleColor();

private:
	bool							_toggleColor = false;

	physx::PxVec3 _magenta = { 1.0f, 0.0f, 1.0f };
	physx::PxVec3 _cyan = { 0.0f, 1.0f, 1.0f };
};

