#pragma once

class PhysicsUtils
{
public:
	static physx::PxVec3 GetFowardVecFromYawPitch(float yaw, float pitch);	/// return normalized forward vector

	static physx::PxVec3 GetRotatedVecFromYawPitch(float yaw, float pitch, const physx::PxVec3& origin);
};

