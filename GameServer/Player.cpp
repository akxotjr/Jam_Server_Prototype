#include "pch.h"
#include "Player.h"
#include "TimeManager.h"
#include "IdManager.h"

Player::Player()
{
	_id = IdManager::Instance().Generate(IdType::Actor, ActorTypePrefix::Player);
}

Player::~Player()
{
}

void Player::Init(RoomRef room)
{
	Super::Init(room);

	//_info->set_name("player" + to_string(_info->id()));
	//_info->set_type(Protocol::ActorType::ACTOR_TYPE_PLAYER);
}

void Player::Update()
{
	float deltaTime = static_cast<float>(TimeManager::Instance().GetDeltaTime());

	if (!_isOnGround)
		_verticalVelocity += -9.8f * deltaTime;	// TODO

	physx::PxVec3 finalVelocity(_horizontalVelocity.x, _verticalVelocity, _horizontalVelocity.z);

	physx::PxControllerFilters filters;
	physx::PxControllerCollisionFlags collisionFlags = _controller->move(finalVelocity * deltaTime, 0.001f, deltaTime, filters);


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
	_position = physx::PxVec3(static_cast<float>(pos.x), static_cast<float>(pos.y), static_cast<float>(pos.z));
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

	_horizontalVelocity = moveDir * _moveSpeed;

	// TODO : JUMP, Fire, Skill etc.
	// isJump
	// isFire
}
