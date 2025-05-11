#pragma once

class TransformCompressor
{
public:
    // Position
    static uint64   PackPosition(float px, float py, float pz);
    static void     UnPackPosition(uint64 packed, OUT float& px, OUT float& py, OUT float& pz);

    // Rotation
    static uint32   PackRotation(float yaw, float yawSpeed);
    static void     UnPackRotation(uint32 packed, OUT float& yaw, OUT float& yawSpeed);

    // Velocity + MoveSpeed (vx, vy, vz, speed) -> 64bit packing
    static uint64   PackVelocityAndSpeed(float vx, float vy, float vz, float moveSpeed);
    static void     UnpackVelocityAndSpeed(uint64 packed, OUT float& vx, OUT float& vy, OUT float& vz, OUT float& moveSpeed);


private:
    // Position
    static uint16   CompressPosition(float value);
    static float    ExpandPosition(uint16 packed);

    //// Rotation (x, y, z) [-180, +180] 범위 압축
    //static uint16   CompressRotation(float value);
    //static float    ExpandRotation(uint16 packed);
};


