#pragma once


class Actor : public enable_shared_from_this<Actor>
{
public:
	Actor();
	virtual ~Actor();

	virtual void		Init();
	virtual void		Update() = 0;

	void				SetPosition(Vec2 pos) { _position = pos; }
	Vec2&				GetPosition() { return _position; }

	void				SetOwnerSession(GameTcpSessionRef ownerSession) { _ownerSession = ownerSession; }
	GameTcpSessionRef	GetOwnerSession() { return _ownerSession.lock(); }

	RoomRef				GetOwnerRoom() const { return _ownerRoom.lock(); }

private:
	weak_ptr<GameTcpSession> _ownerSession;

	std::weak_ptr<Room> _ownerRoom;

protected:
	Vec2 _position = { GWinSizeX / 2, GWinSizeY / 2 };	//temp

	uint64			_id = 0;
	physx::PxVec3	_pos = {};
};

