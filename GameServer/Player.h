#pragma once
#include "Character.h"

class Player : public Character
{
	using Super = Character;

public:
	Player(GameSessionRef gameSession);
	virtual ~Player();

	virtual void Init() override;
	virtual void Update() override;

public:
	Protocol::PlayerProto _playerProto;
};