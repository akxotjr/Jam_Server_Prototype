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

	void ProcessPlayerInpuf(float timestamp, uint32 sequenceNumber, Protocol::KeyType key, float deltaTime, Vec2 targetPos);
	uint32 GetLastSequence() { return _lastProcessSequence; }

private:
	uint32 _lastProcessSequence;
	Vec2 _targetPos;
};