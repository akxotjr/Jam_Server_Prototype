#include "pch.h"
#include "TransformCompressor.h"


uint64 TransformCompressor::PackPosition(float px, float py, float pz)
{
    uint16 px16 = CompressPosition(px);
    uint16 py16 = CompressPosition(py);
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
    py = ExpandPosition(py16);
    pz = ExpandPosition(pz16);
}

uint64 TransformCompressor::PackRotation(float rx, float ry, float rz)
{
    uint16 rx16 = CompressRotation(rx);
    uint16 ry16 = CompressRotation(ry);
    uint16 rz16 = CompressRotation(rz);

    uint64 packed = 0;
    packed |= (static_cast<uint64>(rx16) & 0xFFFF) << 48;
    packed |= (static_cast<uint64>(ry16) & 0xFFFF) << 32;
    packed |= (static_cast<uint64>(rz16) & 0xFFFF) << 16;

    return packed;
}

void TransformCompressor::UnPackRotation(uint64 packed, OUT float& rx, OUT float& ry, OUT float& rz)
{
    uint16 rx16 = static_cast<int16>((packed >> 48) & 0xFFFF);
    uint16 ry16 = static_cast<int16>((packed >> 32) & 0xFFFF);
    uint16 rz16 = static_cast<int16>((packed >> 16) & 0xFFFF);

    rx = ExpandRotation(rx16);
    ry = ExpandRotation(ry16);
    rz = ExpandRotation(rz16);
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
    return normalized * (WORLD_RANGE_MAX - WORLD_RANGE_MIN) + WORLD_RANGE_MIN;
}

uint16 TransformCompressor::CompressRotation(float value)
{
    float normalized = (value + 180.0f) / 360.0f;
    normalized = std::fmaxf(0.0f, std::fminf(1.0f, normalized));
    return static_cast<uint16>(normalized * 65535.0f);
}

float TransformCompressor::ExpandRotation(uint16 packed)
{
    float normalized = static_cast<float>(packed) / 65535.0f;
    return normalized * 360.0f - 180.0f;
}
