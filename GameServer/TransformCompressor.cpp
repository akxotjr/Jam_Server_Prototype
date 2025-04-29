#include "pch.h"
#include "TransformCompressor.h"

uint16_t TransformCompressor::PackPosition(float value)
{
    float normalized = (value - POSITION_MIN) / (POSITION_MAX - POSITION_MIN);
    normalized = std::fmaxf(0.0f, std::fminf(1.0f, normalized));
    return static_cast<uint16_t>(normalized * 65535.0f);
}

float TransformCompressor::UnpackPosition(uint16_t packed)
{
    float normalized = static_cast<float>(packed) / 65535.0f;
    return normalized * (POSITION_MAX - POSITION_MIN) + POSITION_MIN;
}

uint64_t TransformCompressor::PackVelocityAndSpeed(float vx, float vy, float vz, float moveSpeed)
{
    int16_t vx16 = static_cast<int16_t>(vx * 100.0f);
    int16_t vy16 = static_cast<int16_t>(vy * 100.0f);
    int16_t vz16 = static_cast<int16_t>(vz * 100.0f);
    int16_t speed16 = static_cast<int16_t>(moveSpeed * 100.0f);

    uint64_t packed = 0;
    packed |= (static_cast<uint64_t>(vx16) & 0xFFFF) << 48;
    packed |= (static_cast<uint64_t>(vy16) & 0xFFFF) << 32;
    packed |= (static_cast<uint64_t>(vz16) & 0xFFFF) << 16;
    packed |= (static_cast<uint64_t>(speed16) & 0xFFFF);

    return packed;
}

void TransformCompressor::UnpackVelocityAndSpeed(uint64_t packed, float& vx, float& vy, float& vz, float& moveSpeed)
{
    int16_t vx16 = static_cast<int16_t>((packed >> 48) & 0xFFFF);
    int16_t vy16 = static_cast<int16_t>((packed >> 32) & 0xFFFF);
    int16_t vz16 = static_cast<int16_t>((packed >> 16) & 0xFFFF);
    int16_t speed16 = static_cast<int16_t>(packed & 0xFFFF);

    vx = static_cast<float>(vx16) / 100.0f;
    vy = static_cast<float>(vy16) / 100.0f;
    vz = static_cast<float>(vz16) / 100.0f;
    moveSpeed = static_cast<float>(speed16) / 100.0f;
}

uint16_t TransformCompressor::PackRotation(float value)
{
    float normalized = (value + 180.0f) / 360.0f;
    normalized = std::fmaxf(0.0f, std::fminf(1.0f, normalized));
    return static_cast<uint16_t>(normalized * 65535.0f);
}

float TransformCompressor::UnpackRotation(uint16_t packed)
{
    float normalized = static_cast<float>(packed) / 65535.0f;
    return normalized * 360.0f - 180.0f;
}
