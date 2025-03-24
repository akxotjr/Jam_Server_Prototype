#include "pch.h"
#include "Player.h"
#include "GameSession.h"
#include "TimeManager.h"

Player::Player(GameSessionRef gameSession)
	: Super(gameSession)
{
}

Player::~Player()
{
}

void Player::Init()
{
	Super::Init();

	//_playerProto.set_id(1);
	//_playerProto.set_name("abc");
	//_playerProto.set_playertype(Protocol::PLAYER_TYPE_NONE);

	//float serverTime = GTimeManager.GetServerTime();
	//_playerProto.set_timestamp(serverTime);
}

void Player::Update()
{
	Super::Update();
}

void Player::UpdateMovement()
{

}
