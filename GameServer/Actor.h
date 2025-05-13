#pragma once

class Actor : public enable_shared_from_this<Actor>
{
public:
	Actor() = default;
	virtual ~Actor();

	virtual void					Init(RoomRef room);
	virtual void					Update() = 0;
	virtual	Protocol::Transform*	GetTransform();

	RoomRef							GetOwnerRoom() const { return _ownerRoom.lock(); }

	physx::PxActor*					GetPxActor() const { return _pxActor; }
	uint32							GetActorId() const { return _actorId; }

	bool							IsDirty() const { return _isDirty; }

	float							GetYawFromPxQuat() const;
	float							GetPitchFromPxQuat() const;

	float							GetYaw() { return _yaw; }
	float							GetPitch() { return _pitch; }

protected:
	std::weak_ptr<Room>				_ownerRoom;

	uint32							_actorId = 0;

	bool							_isDirty = false;

	physx::PxVec3					_position = { 0.0f, 5.0f, 0.0f };
	physx::PxQuat					_rotation = { 0.0f, 0.0f, 0.0f, 1.0f };
	float							_yaw = 0.0f;
	float							_pitch = 0.0f;

	physx::PxActor*					_pxActor = nullptr;
};

