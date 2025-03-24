#pragma once
#include "Character.h"

class Player : public Character
{
	using Super = Character;

public:
	Player();
	virtual ~Player();

	virtual void Init() override;
	virtual void Update() override;

	virtual void UpdateMovement() override;

	void SetPlayerProto(Protocol::PlayerProto playerProto) { _playerProto = playerProto; }
	Protocol::PlayerProto& GetPlayerProto() { return _playerProto; }


public:
	Protocol::PlayerProto _playerProto;
};