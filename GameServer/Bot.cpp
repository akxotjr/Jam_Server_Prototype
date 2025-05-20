#include "pch.h"
#include "Bot.h"
#include "IdManager.h"
#include "Room.h"

Bot::Bot()
{
	_actorId = IdManager::Instance().Generate(IdType::Actor, ActorTypePrefix::Bot);
	_position = { 0.0f, 5.0f, 30.0f };
	_colliderInfo = ColliderInfo::MakeBox(1.f, 1.f, 1.f);

	_color = _magenta;

	_moveSpeed = 500.0f;
	_horizontalVelocity = { _moveSpeed, 0.0f, 0.0f };

}

Bot::~Bot()
{
	if (_controller)
		_controller->release();
}

void Bot::Init(RoomRef room)
{
	_ownerRoom = room;

	GetOwnerRoom()->_physicsQueue->Push(job::Job([this]()
		{
			physx::PxBoxControllerDesc desc = {};
			desc.position = physx::PxExtendedVec3(_position.x, _position.y, _position.z);
			desc.halfSideExtent = _colliderInfo.box.hx;
			desc.halfHeight = _colliderInfo.box.hy;
			desc.halfForwardExtent = _colliderInfo.box.hz;
			desc.upDirection = physx::PxVec3(0, 1, 0);
			desc.slopeLimit = cosf(physx::PxPi * 0.25f); // 45 degree
			desc.material = PhysicsManager::Instance().GetDefaultMaterial();
			_controller = static_cast<physx::PxCapsuleController*>(GetOwnerRoom()->_pxControllerManager->createController(desc));
			ASSERT_CRASH(_controller != nullptr)

			_controller->getActor()->userData = this;
		}));
}

void Bot::Update()
{
	float deltaTime = static_cast<float>(TICK_INTERVAL_S);

	physx::PxControllerFilters filters;
	physx::PxControllerCollisionFlags collisionFlags = _controller->move(_horizontalVelocity * deltaTime, 0.01f, deltaTime, filters);


	physx::PxExtendedVec3 pos = _controller->getPosition();
	_position = physx::PxVec3(static_cast<float>(pos.x), static_cast<float>(pos.y), static_cast<float>(pos.z));

	if (_position.x <= -20.f || _position.x >= 20.f)
		_horizontalVelocity.x = -_horizontalVelocity.x;
}

Protocol::Transform* Bot::GetTransform()
{
	return Super::GetTransform();
}

void Bot::ToggleColor()
{
	_color = (_color == _magenta) ? _cyan : _magenta;
}


