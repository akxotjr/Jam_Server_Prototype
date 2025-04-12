#include "pch.h"
#include "Player.h"
#include "TimeManager.h"

Player::Player()
{
}

Player::~Player()
{
}

void Player::Init()
{
	Super::Init();

	_info->set_name("player" + to_string(_info->id()));
	_info->set_type(Protocol::ActorType::ACTOR_TYPE_PLAYER);
}

void Player::Update()
{
	float deltaTime = GTimeManager.GetDeltaTime();

	if ((_targetPos - _position).Length() < 1.5f)
	{
		_position = _targetPos;
		return;
	}

	_position = _position + _velocity * deltaTime;
}

void Player::ProcessPlayerInpuf(float timestamp, uint32 sequenceNumber, Protocol::KeyType key, float deltaTime, Vec2 targetPos)
{
	_lastProcessSequence = sequenceNumber;

	_targetPos = targetPos;
	_direction = targetPos - _position;
	_direction.Normalize();

	_velocity = _direction * _speed;

	_position = _position + _velocity * deltaTime;
}
