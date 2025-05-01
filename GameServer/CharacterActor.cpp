#include "pch.h"
#include "CharacterActor.h"
#include "Room.h"
#include "TransformCompressor.h"

CharacterActor::CharacterActor()
{
}

CharacterActor::~CharacterActor()
{
	_controller->release();
	_controller = nullptr;
}

void CharacterActor::Init(RoomRef room)
{
	Super::Init(room);

	physx::PxCapsuleControllerDesc desc = {};
	desc.position = physx::PxExtendedVec3(0, 1, 0);
	desc.radius = 0.5f;
	desc.height = 1.8f;
	desc.slopeLimit = cosf(physx::PxPi * 0.25f); // 45µµ
	_controller = dynamic_cast<physx::PxCapsuleController*>(GetOwnerRoom()->_controllerManager->createController(desc));
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


