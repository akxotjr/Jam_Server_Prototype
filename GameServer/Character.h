#pragma once
#include "Actor.h"

class Character : public Actor
{
	using Super = Actor;

public:
	Character();
	virtual ~Character();

	virtual void Init();
	virtual void Update();

	void SetInfo(Protocol::CharacterInfo* info);
	Protocol::CharacterInfo* GetInfo() { return _info; }

protected:
	Vec2		_direction = {0.f, 0.f};
	Vec2		_velocity = {};
	float		_speed = 5.f;

	Protocol::CharacterInfo* _info;
};

