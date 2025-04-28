#include "pch.h"
#include "Player.h"
#include "Room.h"
#include "TimeManager.h"

Player::Player()
{
}

Player::~Player()
{
	_controller->release();
	_controller = nullptr;
}

void Player::Init()
{
	Super::Init();

	physx::PxCapsuleControllerDesc desc = {};
	desc.position = physx::PxExtendedVec3(0, 1, 0);
	desc.radius = 0.5f;
	desc.height = 1.8f;
	desc.slopeLimit = cosf(physx::PxPi * 0.25f); // 45µµ
	_controller = dynamic_cast<physx::PxCapsuleController*>(GetOwnerRoom()->_controllerManager->createController(desc));


	//_info->set_name("player" + to_string(_info->id()));
	//_info->set_type(Protocol::ActorType::ACTOR_TYPE_PLAYER);
}

void Player::Update()
{
	float deltaTime = static_cast<float>(TimeManager::Instance().GetDeltaTime());
	physx::PxVec3 displacement = _velocity * deltaTime;

	physx::PxControllerFilters filters;
	physx::PxControllerCollisionFlags collisionFlags = _controller->move(displacement, 0.001f, deltaTime, filters);

	// TODO : collision 
	if (collisionFlags & physx::PxControllerCollisionFlag::eCOLLISION_DOWN)
	{
		_isOnGround = true;
	}
	else
	{
		_isOnGround = false;
	}

	physx::PxExtendedVec3 pos = _controller->getPosition();
	_pos = physx::PxVec3(static_cast<float>(pos.x), static_cast<float>(pos.y), static_cast<float>(pos.z));
}

void Player::ProcessInput(uint32 keyField, float cameraYaw, float cameraPitch, uint32 sequence)
{
	_lastProcessSequence = sequence;

	ProcessKeyField(keyField);

	// if isFire then Fire(cameraYaw, cameraPitch)

}

void Player::ProcessKeyField(uint32& keyField)
{
	float dx = 0.f, dz = 0.f;
	if (keyField & (1 << 0))	// W
		dz = 1.f;
	if (keyField & (1 << 1))	// S
		dz = -1.f;
	if (keyField & (1 << 2))	// A
		dx = 1.f;
	if (keyField & (1 << 3))	// D
		dx = -1.f;

	physx::PxVec3 moveDir(dx, 0.f, dz);

	if (moveDir.magnitudeSquared() > 0.f)
		moveDir = moveDir.getNormalized();

	_velocity = moveDir * _moveSpeed;

	// TODO : JUMP, Fire, Skill etc.
	// isJump
	// isFire
}
