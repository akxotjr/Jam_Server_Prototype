#pragma once
#include "CharacterActor.h"

enum class EInputKey : uint32
{
    MoveForward = 0,    // W
    MoveBackward = 1,   // S
    MoveLeft = 2,       // A
    MoveRight = 3,      // D
    Jump = 4,           // Space
    Fire = 5,           // Left mouse
    AltFire = 6,        // Right mouse
    Skill1 = 7,         // 1
    Skill2 = 8,         // 2
    Skill3 = 9,         // 3
    Skill4 = 10,        // 4
    QSkill = 11,        // Q
    ESkill = 12,        // E
    TabOpen = 13,       // Tab
};


class Player : public CharacterActor
{
	using Super = CharacterActor;

public:
	Player();
	virtual ~Player() override = default;

    /* Actor impl */
	virtual void                    Init(RoomRef room) override;
	virtual void                    Update() override;
    virtual	Protocol::Transform		GetTransform() const override;

	void                            ProcessInput(uint32 keyField, float cameraYaw, float cameraPitch, uint32 sequence);

	uint32                          GetLastSequence() const { return _lastProcessSequence; }

    void                            SetUserId(uint32 id) { _userId = id; }
    uint32                          GetUserId() const { return _userId; }

    void SetYawPitch(float yaw, float pitch);

private:
	void                            ProcessKeyField(uint32& keyFiel);

private:
    uint32                          _userId;

	uint32	                        _lastProcessSequence;
};