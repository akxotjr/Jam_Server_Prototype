#include <numbers>

#pragma once

constexpr double TICK_RATE = 30.0;	// 30 tick/s => tick interval(1 tick) = 0.0333.. s = 33.3.. ms
constexpr double TICK_INTERVAL_S = 1.0 / TICK_RATE;
constexpr uint64 WORKER_TICK_MS = static_cast<uint64>(TICK_INTERVAL_S * 1000.0);

constexpr float WORLD_RANGE_MIN = -2048.0f;
constexpr float WORLD_RANGE_MAX = 2048.0f;

constexpr float YAW_SPEED_MIN = -10.0f;
constexpr float YAW_SPEED_MAX = +10.0f;
constexpr float YAW_SPEED_RANGE = YAW_SPEED_MAX - YAW_SPEED_MIN;

constexpr float POSITION_RESOLUTION = (WORLD_RANGE_MAX - WORLD_RANGE_MIN) / 65535.0f;
constexpr float HALF_RES = POSITION_RESOLUTION * 0.5f;

constexpr float PI = std::numbers::pi_v<float>;
constexpr float GRAVITY = 9.8f;

constexpr int32 MAX_PLAYERS_PER_ROOM = 4;

constexpr float MAX_CHARACTER_MOVE_SPEED = 15.f;
constexpr float MAX_CHARACTER_DIST = TICK_INTERVAL_S * MAX_CHARACTER_MOVE_SPEED;

constexpr float MAX_PROJECTILE_SPEED = 300.f;
constexpr float MAX_PROJECTILE_DIST = TICK_INTERVAL_S * MAX_PROJECTILE_SPEED;
