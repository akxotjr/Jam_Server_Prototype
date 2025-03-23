#include "pch.h"
#include "Bot.h"
#include "GameSession.h"
#include "TimeManager.h"
#include <algorithm>

Bot::Bot(GameSessionRef gameSession)
    : Super(gameSession)
{
}

Bot::~Bot()
{
}

void Bot::Init()
{
	Super::Init();
	// todo : first direction is random dir
}

void Bot::Update()
{
	Super::Update();
}

void Bot::UpdateMovement()
{
    float deltaTime = GTimeManager.GetDeltaTime();
    Vec2 nextPos = _position + _velocity * deltaTime;

    bool isChangedVelocity = false;

    if (nextPos.x < 0.f || nextPos.x > GWinSizeX)
    {
        _direction.x = -_direction.x;
        _velocity.x = -_velocity.x;
        isChangedVelocity = true;
    }

    if (nextPos.y < 0.f || nextPos.y > GWinSizeY)
    {
        _direction.y = -_direction.y;
        _velocity.y = -_velocity.y;
        isChangedVelocity = true;
    }

    // clamp Ã³¸®
    nextPos.x = std::clamp(nextPos.x, 0.0f, static_cast<float>(GWinSizeX));
    nextPos.y = std::clamp(nextPos.y, 0.0f, static_cast<float>(GWinSizeY));

    _position = nextPos;
}
