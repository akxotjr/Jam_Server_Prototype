#pragma once
#include "Character.h"

class Player : public Character
{
	using Super = Character;

public:
	Player();
	virtual ~Player();

	virtual void Init();
	virtual void Update();
};