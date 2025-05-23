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

    /** Actor impl **/
	virtual void                    Init(RoomRef room) override;
	virtual void                    Update() override;
    virtual	Protocol::Transform*	GetTransform() override;

    /** Processing Input **/
public:
	void                            ProcessInput(uint32 keyField, float cameraYaw, float cameraPitch, uint32 sequence, double timestamp);
private:
    void                            ProcessMovement(uint32 keyFiel);
    void                            ProcessJump(uint32 keyField);
    void                            ProcessFire(uint32 keyField, double timestamp, float yaw, float pitch);

    /** Getter & Setter **/
public:
	uint32                          GetLastSequence() const { return _lastProcessSequence; }

    void                            SetUserId(uint32 id) { _userId = id; }
    uint32                          GetUserId() const { return _userId; }

    void                            SetYawPitch(float yaw, float pitch);

    void                            SetPlayerIndex(int32 index) { _playerIndex = index; }
    int32                           GetPlayerIndex() { return _playerIndex; }

private:
    USE_LOCK

    uint32                          _userId;
    int32                           _playerIndex = 0;


	uint32	                        _lastProcessSequence;

    bool                            _isGrounded = true;
    float                           _jumpSpeed = 6.0f;

    float                           _cameraDist = 5.f;
    physx::PxVec3                   _cameraOffset = { -0.2f, 0.4f, -1.5f };
    physx::PxVec3                   _muzzleOffset = { 0.0f, 0.0f, 2.0f };
    const float                     _rayMaxDist = 10000.f;

    bool                            _isFireInProgress = false;
};