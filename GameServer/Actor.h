#pragma once


class Actor : public enable_shared_from_this<Actor>
{
public:
	Actor();
	virtual ~Actor();

	virtual void				Init(RoomRef room);
	virtual void				Update() = 0;

	RoomRef						GetOwnerRoom() const { return _ownerRoom.lock(); }

	physx::PxActor*				GetPxActor() const { return _pxActor; }
	uint32						GetId() const { return _id; }


protected:
	std::weak_ptr<Room>			_ownerRoom;

	uint32						_id = 0;

	physx::PxVec3				_position = { 0.0f, 0.0f, 0.0f };
	physx::PxQuat				_rotation = { 0.0f, 0.0f, 0.0f, 1.0f };
	physx::PxActor*				_pxActor = nullptr;
};

