#include "pch.h"
#include "Player.h"
#include "IdManager.h"
#include "Values.h"

Player::Player()
{
	_actorId = IdManager::Instance().Generate(IdType::Actor, ActorTypePrefix::Player);
}

void Player::Init(RoomRef room)
{
	Super::Init(room);
}

void Player::Update()
{
	//float deltaTime = static_cast<float>(TimeManager::Instance().GetDeltaTime());
	float deltaTime = static_cast<float>(TICK_INTERVAL_S);

	//if (!_isOnGround)
	//	_verticalVelocity += -9.8f * deltaTime;	// TODO

	physx::PxVec3 finalVelocity(_horizontalVelocity.x, _verticalVelocity, _horizontalVelocity.z);

	physx::PxControllerFilters filters;
	physx::PxControllerCollisionFlags collisionFlags = _controller->move(finalVelocity * deltaTime, 0.0f, deltaTime, filters);


	//// TODO : collision 
	//if (collisionFlags & physx::PxControllerCollisionFlag::eCOLLISION_DOWN)
	//{
	//	_isOnGround = true;
	//}
	//else
	//{
	//	_isOnGround = false;
	//}

	physx::PxExtendedVec3 pos = _controller->getPosition();
	_position = physx::PxVec3(static_cast<float>(pos.x), static_cast<float>(pos.y), static_cast<float>(pos.z));
}

void Player::ProcessInput(uint32 keyField, float cameraYaw, float cameraPitch, uint32 sequence)
{
	_lastProcessSequence = sequence;

	ProcessKeyField(keyField);
	Update();
	_horizontalVelocity = physx::PxVec3(0, 0, 0);
	// if isFire then Fire(cameraYaw, cameraPitch)

}

Protocol::Transform Player::ToTransform() const
{
	return Super::ToTransform();
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

	/*cout << "_horizontalVelocity (" << _horizontalVelocity.x << ", " << _horizontalVelocity.y << ", " << _horizontalVelocity.z << ")\n";*/

	// TODO : JUMP, Fire, Skill etc.
	// isJump
	// isFire
}
