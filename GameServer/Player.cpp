#include "pch.h"
#include "Player.h"

#include "ClientPacketHandler.h"
#include "IdManager.h"
#include "PhysicsManager.h"
#include "Values.h"
#include "Room.h"
#include "SessionManager.h"
#include "TimeManager.h"
#include "TransformCompressor.h"

Player::Player()
{
	_actorId = IdManager::Instance().Generate(IdType::Actor, ActorTypePrefix::Player);
	_position = { 0.0f, 5.0f, 0.0f };
	_colliderInfo = ColliderInfo::MakeCapsule(0.1f, 0.4f);
}

void Player::Init(RoomRef room)
{
	_ownerRoom = room;

	GetOwnerRoom()->_physicsQueue->Push(job::Job([this]()
		{
			physx::PxCapsuleControllerDesc desc = {};
			desc.position = physx::PxExtendedVec3(_position.x, _position.y, _position.z);
			desc.radius = _colliderInfo.capsule.radius;
			desc.height = _colliderInfo.capsule.halfHeight * 2;
			desc.upDirection = physx::PxVec3(0, 1, 0);
			desc.slopeLimit = cosf(physx::PxPi * 0.25f); // 45 degree
			desc.material = PhysicsManager::Instance().GetDefaultMaterial();
			_controller = static_cast<physx::PxCapsuleController*>(GetOwnerRoom()->_pxControllerManager->createController(desc));
			ASSERT_CRASH(_controller != nullptr)
		}));
}

void Player::Update()
{
	float deltaTime = static_cast<float>(TICK_INTERVAL_S);

	if (!_isGrounded)
		_verticalVelocity -= GRAVITY * deltaTime;

	physx::PxVec3 finalVelocity(_horizontalVelocity.x, _verticalVelocity, _horizontalVelocity.z);

	physx::PxControllerFilters filters;
	physx::PxControllerCollisionFlags collisionFlags = _controller->move(finalVelocity * deltaTime, 0.01f, deltaTime, filters);

	if (collisionFlags & physx::PxControllerCollisionFlag::eCOLLISION_DOWN)
	{
		_verticalVelocity = 0.0f;
		_isGrounded = true;
	}
	else
	{
		_isGrounded = false;
	}

	physx::PxExtendedVec3 pos = _controller->getPosition();
	_position = physx::PxVec3(static_cast<float>(pos.x), static_cast<float>(pos.y), static_cast<float>(pos.z));
}

void Player::ProcessInput(uint32 keyField, float yaw, float pitch, uint32 sequence, double timestamp)
{
	_lastProcessSequence = sequence;

	_yaw = yaw;
	_pitch = pitch;

	ProcessMovement(keyField);
	ProcessJump(keyField);
	ProcessFire(keyField, timestamp, yaw, pitch);
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

void Player::ProcessMovement(uint32 keyField)
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

	physx::PxVec3 rotatedDir = {
		rotatedDir.x = dir.x * cosf(-_yaw) - dir.z * sinf(-_yaw),
		0.0f,
		rotatedDir.z = dir.x * sinf(-_yaw) + dir.z * cosf(-_yaw)
	};

	WRITE_LOCK
	_horizontalVelocity = rotatedDir * _moveSpeed;
}

void Player::ProcessJump(uint32 keyField)
{
	if (keyField & (1 << static_cast<int32>(EInputKey::Jump)))
	{
		if (_isGrounded)
		{
			WRITE_LOCK
			_verticalVelocity = _jumpSpeed;
		}
	}
}

void Player::ProcessFire(uint32 keyField, double timestamp, float yaw, float pitch)
{
	if (keyField & (1 << static_cast<int32>(EInputKey::Fire)))
	{
		auto room = GetOwnerRoom();
		Snapshot* snapshot = room->FindSnapshot(timestamp);
		if (snapshot == nullptr)
		{
			cout << "snapshot is nullptr\n";
			return;
		}

		cout << "Snapshot size : " << snapshot->size() << "\n";

		SnapshotEntity* shooter = nullptr;

		for (SnapshotEntity& entity : *snapshot)
		{
			if (entity.actorId == _actorId)
			{
				shooter = &entity;
				break;
			}
		}

		if (!shooter)
		{
			cout << "shooter is nullptr\n";
			return;
		}

		physx::PxVec3 cameraPos = shooter->position + PhysicsUtils::GetRotatedVecFromYawPitch(yaw, pitch, _cameraOffset * _cameraDist);
		physx::PxVec3 rayDir = PhysicsUtils::GetFowardVecFromYawPitch(yaw, pitch);

		physx::PxScene* rewindScene = room->BuildRewindScene(_playerIndex, *snapshot);

		if (!rewindScene)
		{
			cout << "rewindScene is nullptr\n";
			return;
		}

		physx::PxRaycastHit hit;
		bool hasHit = PhysicsManager::Instance().RaycastInScene(rewindScene, cameraPos, rayDir, _rayMaxDist, hit);
		if (!hasHit)
		{
			cout << "No Target\n";
			return;
		}

		physx::PxVec3 targetPos = hasHit ? hit.position : (cameraPos + rayDir * _rayMaxDist);


		physx::PxVec3 muzzlePos = shooter->position + PhysicsUtils::GetRotatedVecFromYawPitch(yaw, pitch, _muzzleOffset);
		physx::PxVec3 fireDir = (targetPos - muzzlePos).getNormalized();

		physx::PxRaycastHit finalHit;
		bool finalHasHit = PhysicsManager::Instance().RaycastInScene(rewindScene, muzzlePos, fireDir, _rayMaxDist, finalHit);

		room->ClearRewindScene(_playerIndex);

		if (finalHasHit)
		{
			double t = TimeManager::Instance().GetServerTime();
			Protocol::S_HIT_RESULT hitPkt;
			hitPkt.set_actorid(_actorId);
			hitPkt.set_position(TransformCompressor::PackPosition(finalHit.position.x, finalHit.position.y, finalHit.position.z));

			auto sendBuffer = ClientPacketHandler::MakeSendBufferUdp(hitPkt);
			auto session = static_pointer_cast<ReliableUdpSession>(SessionManager::Instance().GetSessionByUserId(ProtocolType::PROTOCOL_UDP, _userId));
			session->SendReliable(sendBuffer, t);

			// finalHit.actor->
		}
	}
}