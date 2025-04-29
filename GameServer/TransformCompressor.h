#pragma once

#pragma once

#include <cstdint>
#include <cmath>

class TransformCompressor
{
public:
    // 월드 범위 (필요에 따라 조정 가능)
    static constexpr float POSITION_MIN = -10000.0f;
    static constexpr float POSITION_MAX = 10000.0f;

    // Position
    static uint16_t PackPosition(float value);
    static float UnpackPosition(uint16_t packed);

    // Velocity + MoveSpeed (vx, vy, vz, speed) -> 64bit packing
    static uint64_t PackVelocityAndSpeed(float vx, float vy, float vz, float moveSpeed);
    static void UnpackVelocityAndSpeed(uint64_t packed, float& vx, float& vy, float& vz, float& moveSpeed);

    // Rotation (x, y, z) [-180, +180] 범위 압축
    static uint16_t PackRotation(float value);
    static float UnpackRotation(uint16_t packed);
};


