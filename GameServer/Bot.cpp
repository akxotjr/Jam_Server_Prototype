#include "pch.h"
#include "Bot.h"
#include "GameSession.h"
#include "TimeManager.h"
#include <algorithm>
#include <random>

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

    // ·£´ý ¿£Áø ¼³Á¤
    static std::random_device rd;
    static std::mt19937 gen(rd());

    // 1. À§Ä¡ ·£´ý: x: 0~800, y: 0~600
    std::uniform_real_distribution<float> posXDist(0.0f, 800.0f);
    std::uniform_real_distribution<float> posYDist(0.0f, 600.0f);

    _position.x = posXDist(gen);
    _position.y = posYDist(gen);

    std::uniform_real_distribution<float> angleDist(0.0f, 2 * 3.1415926f); // 0 ~ 2¥ð
    float angle = angleDist(gen);

    _direction.x = std::cos(angle);
    _direction.y = std::sin(angle);

    _velocity = _direction * _speed;

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
