#pragma once


class Character
{
public:
	Character(GameSessionRef gameSession);
	virtual ~Character();

	virtual void Init();
	virtual void Update();

	virtual void UpdateMovement() abstract;

	GameSessionRef GetGameSession() { return _gamesession.lock(); }

	void SetPos(Vec2& pos) { _position = pos; }
	Vec2& GetPos() { return _position; }

private:
	weak_ptr<GameSession> _gamesession;

protected:
	Vec2 _position = { 400, 300 };
	Vec2 _direction = {};
	Vec2 _velocity = {};
	float _speed = 0.1f;
};

