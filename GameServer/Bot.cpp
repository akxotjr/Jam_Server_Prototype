#include "pch.h"
#include "Bot.h"
#include "GameSession.h"
#include "TimeManager.h"
#include <algorithm>

Bot::Bot()
{
}

Bot::~Bot()
{
}

void Bot::Init()
{
	Super::Init();
	
    _info->set_name("Bot" + to_string(_info->id()));
    _info->set_type(Protocol::ActorType::ACTOR_TYPE_BOT);
}

void Bot::Update()
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

    nextPos.x = std::clamp(nextPos.x, 0.0f, static_cast<float>(GWinSizeX));
    nextPos.y = std::clamp(nextPos.y, 0.0f, static_cast<float>(GWinSizeY));

    _position = nextPos;
}
