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
	float deltaTime = static_cast<float>(TICK_INTERVAL_S);

	physx::PxVec3 finalVelocity(_horizontalVelocity.x, _verticalVelocity, _horizontalVelocity.z);

	physx::PxControllerFilters filters;
	physx::PxControllerCollisionFlags collisionFlags = _controller->move(finalVelocity * deltaTime, 0.01f, deltaTime, filters);

	physx::PxExtendedVec3 pos = _controller->getPosition();
	_position = physx::PxVec3(static_cast<float>(pos.x), static_cast<float>(pos.y), static_cast<float>(pos.z));
}

void Player::ProcessInput(uint32 keyField, float yaw, float pitch, uint32 sequence)
{
	_lastProcessSequence = sequence;

	//SetYawPitch(yaw, pitch);
	_yaw = yaw;
	_pitch = pitch;

	ProcessKeyField(keyField);
}

Protocol::Transform* Player::GetTransform()
{
	return Super::GetTransform();
}

void Player::SetYawPitch(float yaw, float pitch)
{
	physx::PxQuat qYaw(yaw, physx::PxVec3(0, 1, 0));
	physx::PxQuat qPitch(pitch, physx::PxVec3(1, 0, 0));

	_rotation = qYaw * qPitch;
}

void Player::ProcessKeyField(uint32& keyField)
{
	float dx = 0.f, dz = 0.f;
	if (keyField & (1 << static_cast<int32>(EInputKey::MoveForward)))	// W
		dz = 1.f;
	if (keyField & (1 << static_cast<int32>(EInputKey::MoveBackward)))	// S
		dz = -1.f;
	if (keyField & (1 << static_cast<int32>(EInputKey::MoveLeft)))	// A
		dx = 1.f;
	if (keyField & (1 << static_cast<int32>(EInputKey::MoveRight)))	// D
		dx = -1.f;

	physx::PxVec3 dir(dx, 0.f, dz);

	if (dir.magnitudeSquared() > 0.f)
	{
		dir = dir.getNormalized();
	}
	else
	{
		WRITE_LOCK
		_horizontalVelocity = dir;
		return;
	}

	//float actualYaw = GetYawFromPxQuat();


	//physx::PxVec3 rotatedDir(0,0,0);
	//rotatedDir.x = dir.x * cosf(actualYaw) - dir.z * sinf(actualYaw);
	//rotatedDir.z = dir.x * sinf(actualYaw) + dir.z * cosf(actualYaw);

	physx::PxVec3 rotatedDir(0, 0, 0);
	rotatedDir.x = dir.x * cosf(-_yaw) - dir.z * sinf(-_yaw);
	rotatedDir.z = dir.x * sinf(-_yaw) + dir.z * cosf(-_yaw);

	WRITE_LOCK
	_horizontalVelocity = rotatedDir * _moveSpeed;
}
