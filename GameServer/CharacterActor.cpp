#include "pch.h"
#include "CharacterActor.h"
#include "PhysicsManager.h"
#include "Room.h"
#include "TransformCompressor.h"

CharacterActor::CharacterActor()
{
}

CharacterActor::~CharacterActor()
{
	if (_controller) _controller->release();
}

void CharacterActor::Init(RoomRef room)
{
	Super::Init(room);

	GetOwnerRoom()->_physicsQueue->Push(job::Job([this]()
		{
			physx::PxCapsuleControllerDesc desc = {};
			desc.position = physx::PxExtendedVec3(_position.x, _position.y, _position.z);
			desc.radius = 0.1f;
			desc.height = 0.8f;
			desc.upDirection = physx::PxVec3(0, 1, 0);
			desc.slopeLimit = cosf(physx::PxPi * 0.25f); // 45 degree
			desc.material = PhysicsManager::Instance().CreateMaterial(0.5f, 0.5f, 0.1f);
			_controller = static_cast<physx::PxCapsuleController*>(GetOwnerRoom()->_pxControllerManager->createController(desc));
			ASSERT_CRASH(_controller != nullptr)
		}));
}

void CharacterActor::Update()
{
}

Protocol::Transform CharacterActor::ToTransform() const
{
	Protocol::Transform transform;
	transform.set_position(TransformCompressor::PackPosition(_position.x, _position.y, _position.z));
	transform.set_velocity_speed(TransformCompressor::PackVelocityAndSpeed(_horizontalVelocity.x, _verticalVelocity, _horizontalVelocity.z, _moveSpeed));
	transform.set_rotation(TransformCompressor::PackRotation(_rotation.x, _rotation.y, _rotation.z));
	return transform;
}


