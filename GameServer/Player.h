#pragma once
#include "CharacterActor.h"


enum class KeyType : uint32
{
	KEY_W = 0,
	KEY_S,
	KEY_D,
	KEY_A
};


class Player : public CharacterActor
{
	using Super = CharacterActor;

public:
	Player();
	virtual ~Player() override;

	virtual void Init() override;
	virtual void Update() override;

	void ProcessInput(uint32 keyField, float cameraYaw, float cameraPitch, uint32 sequence);


	uint32 GetLastSequence() const { return _lastProcessSequence; }

private:
	void ProcessKeyField(uint32& keyField);

private:
	uint32	_lastProcessSequence;

	physx::PxCapsuleController* _controller;

	bool _isOnGround = false;
	bool _isJumping = false;
};