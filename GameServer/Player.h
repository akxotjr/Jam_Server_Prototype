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
    virtual	Protocol::Transform*	GetTransform() override;

	void                            ProcessInput(uint32 keyField, float cameraYaw, float cameraPitch, uint32 sequence, double timestamp);

	uint32                          GetLastSequence() const { return _lastProcessSequence; }

    void                            SetUserId(uint32 id) { _userId = id; }
    uint32                          GetUserId() const { return _userId; }

    void                            SetYawPitch(float yaw, float pitch);

private:
	void                            ProcessMovement(uint32 keyFiel);
    void                            ProcessJump(uint32 keyField);
    void                            ProcessFire(uint32 keyField, double timestamp);

    physx::PxVec3                   ComputeForwardFromYawPitch(float yaw, float pitch);
    bool RaycastInScene(physx::PxScene* scene, const physx::PxVec3& origin, const physx::PxVec3& dir, float maxDist, physx::PxRaycastHit& outHit);

private:
    USE_LOCK

    uint32                          _userId;

	uint32	                        _lastProcessSequence;

    bool                            _isGrounded = true;
    float                           _jumpSpeed = 6.0f;

    physx::PxVec3                   _cameraOffset = { -0.2f, 0.4f, -1.5f };
    physx::PxVec3                   _muzzleOffset = { 0.2f, 1.4f, 0.5f };   // TODO : °ª Á¶Á¤
    const float                     _rayMaxDist = 500.f;
};