#include "pch.h"
#include "DynamicActor.h"
#include "PhysicsManager.h"
#include "Room.h"
#include "TransformCompressor.h"


void DynamicActor::Init(RoomRef room)
{
	Super::Init(room);

	GetOwnerRoom()->_physicsQueue->Push(job::Job([this]()
		{
			_pxActor = PhysicsManager::Instance().CreateRigidStatic(_position, _rotation);

			
		}));

	//auto* physics = RoomManager::Instance().GetPxPhysics();
	//auto* pxScene = GetOwnerRoom()->GetPxScene();
	//if (physics == nullptr || pxScene == nullptr)
	//	return;

	//// Material
	//static physx::PxMaterial* defaultMaterial = physics->createMaterial(0.5f, 0.5f, 0.6f); // staticFriction, dynamicFriction, restitution

	//// Actor
	//_pxActor = physics->createRigidDynamic(physx::PxTransform(_position, _rotation));
	//if (_pxActor == nullptr)
	//	return;

	//// Shape
	//physx::PxShape* shape = physics->createShape(physx::PxBoxGeometry(0.5f, 0.5f, 0.5f), *defaultMaterial);
	//auto* rigid = static_cast<physx::PxRigidDynamic*>(_pxActor);
	//rigid->attachShape(*shape);

	//shape->release();

	//physx::PxRigidBodyExt::updateMassAndInertia(*static_cast<physx::PxRigidDynamic*>(_pxActor), 1.0f);
}

void DynamicActor::Update()
{
}

Protocol::Transform DynamicActor::GetTransform() const
{
	Protocol::Transform transform;
	transform.set_position(TransformCompressor::PackPosition(_position.x, _position.y, _position.z));
	transform.set_velocity_speed(TransformCompressor::PackVelocityAndSpeed(_horizontalVelocity.x, _verticalVelocity, _horizontalVelocity.z, _moveSpeed));
	transform.set_rotation(TransformCompressor::PackRotation(GetYawFromPxQuat(), _yawSpeed));
	return transform;
}
