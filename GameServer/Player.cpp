#include "pch.h"
#include "Player.h"
#include "GameSession.h"
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
	_info.set_name("player" + to_string(_info.id()));
}

void Player::Update()
{
	
}
