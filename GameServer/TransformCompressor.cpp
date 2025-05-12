#include "pch.h"
#include "TransformCompressor.h"


uint64 TransformCompressor::PackPosition(float px, float py, float pz)
{
    uint16 px16 = CompressPosition(px);
    uint16 py16 = static_cast<uint16>(py * 128.0f); // 128 -> y precision = 1/128 = 0.0078125, max y range = 65535 / 128.f = 511.99
    uint16 pz16 = CompressPosition(pz);

    uint64 packed = 0;
    packed |= (static_cast<uint64>(px16) & 0xFFFF) << 48;
    packed |= (static_cast<uint64>(py16) & 0xFFFF) << 32;
    packed |= (static_cast<uint64>(pz16) & 0xFFFF) << 16;

    return packed;
}

void TransformCompressor::UnPackPosition(uint64 packed, OUT float& px, OUT float& py, OUT float& pz)
{
    int16 px16 = static_cast<int16>((packed >> 48) & 0xFFFF);
    int16 py16 = static_cast<int16>((packed >> 32) & 0xFFFF);
    int16 pz16 = static_cast<int16>((packed >> 16) & 0xFFFF);

    px = ExpandPosition(px16);
    py = static_cast<float>(py16) / 128.0f;
    pz = ExpandPosition(pz16);
}

uint32 TransformCompressor::PackRotation(float yaw, float pitch)
{
    uint16 yaw16 = static_cast<uint16>((yaw + PI) / (2.0f * PI) * 65535.0f);
    uint16 pitch16 = static_cast<uint16>((pitch + (PI / 2.0f)) / PI * 65535.0f);

    uint32 packed = 0;
    packed |= static_cast<uint32>(yaw16) << 16;
    packed |= static_cast<uint32>(pitch16);

    return packed;
}

void TransformCompressor::UnPackRotation(uint32 packed, OUT float& yaw, OUT float& pitch)
{
    uint16 yaw16 = static_cast<uint16>((packed >> 16) & 0xFFFF);
    uint16 pitch16 = static_cast<uint16>(packed & 0xFFFF);

    yaw = (static_cast<float>(yaw16) / 65535.0f) * (2.0f * PI) - PI;
    pitch = (static_cast<float>(pitch16) / 65535.0f) * PI - (PI / 2.0f);
}


uint64 TransformCompressor::PackVelocityAndSpeed(float vx, float vy, float vz, float moveSpeed)
{
    int16 vx16 = static_cast<int16>(vx * 100.0f);
    int16 vy16 = static_cast<int16>(vy * 100.0f);
    int16 vz16 = static_cast<int16>(vz * 100.0f);
    int16 speed16 = static_cast<int16>(moveSpeed * 100.0f);

    uint64 packed = 0;
    packed |= (static_cast<uint64>(vx16) & 0xFFFF) << 48;
    packed |= (static_cast<uint64>(vy16) & 0xFFFF) << 32;
    packed |= (static_cast<uint64>(vz16) & 0xFFFF) << 16;
    packed |= (static_cast<uint64>(speed16) & 0xFFFF);

    return packed;
}

void TransformCompressor::UnpackVelocityAndSpeed(uint64 packed, float& vx, float& vy, float& vz, float& moveSpeed)
{
    int16 vx16 = static_cast<int16>((packed >> 48) & 0xFFFF);
    int16 vy16 = static_cast<int16>((packed >> 32) & 0xFFFF);
    int16 vz16 = static_cast<int16>((packed >> 16) & 0xFFFF);
    int16 speed16 = static_cast<int16>(packed & 0xFFFF);

    vx = static_cast<float>(vx16) / 100.0f;
    vy = static_cast<float>(vy16) / 100.0f;
    vz = static_cast<float>(vz16) / 100.0f;
    moveSpeed = static_cast<float>(speed16) / 100.0f;
}

uint16 TransformCompressor::CompressPosition(float value)
{
    float normalized = (value - WORLD_RANGE_MIN) / (WORLD_RANGE_MAX - WORLD_RANGE_MIN);
    normalized = std::fmaxf(0.0f, std::fminf(1.0f, normalized));
    return static_cast<uint16>(normalized * 65535.0f);
}

float TransformCompressor::ExpandPosition(uint16 packed)
{
    float normalized = static_cast<float>(packed) / 65535.0f;
    return normalized * (WORLD_RANGE_MAX - WORLD_RANGE_MIN) + WORLD_RANGE_MIN + HALF_RES;
}
