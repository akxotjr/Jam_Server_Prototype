#pragma once


class Actor : public enable_shared_from_this<Actor>
{
public:
	Actor();
	virtual ~Actor();

	virtual void Init();
	virtual void Update();

	void SetPosition(Vec2 pos) { _position = pos; }
	Vec2& GetPosition() { return _position; }

	void SetOwnerSession(GameTcpSessionRef ownerSession) { _ownerSession = ownerSession; }
	GameTcpSessionRef GetOwnerSession() { return _ownerSession.lock(); }

private:

	weak_ptr<GameTcpSession> _ownerSession;


protected:
	Vec2 _position = { GWinSizeX / 2, GWinSizeY / 2 };	//temp
};

