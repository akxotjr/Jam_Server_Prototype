#include "pch.h"
#include "PhysicsUtils.h"

physx::PxVec3 PhysicsUtils::GetFowardVecFromYawPitch(float yaw, float pitch)
{
	float cy = cosf(yaw);
	float sy = sinf(yaw);
	float cp = cosf(pitch);
	float sp = sinf(pitch);

	return physx::PxVec3(sy * cp, sp, cy * cp).getNormalized();
}

physx::PxVec3 PhysicsUtils::GetRotatedVecFromYawPitch(float yaw, float pitch, const physx::PxVec3& origin)
{
	physx::PxQuat yawRot(yaw, physx::PxVec3(0, 1, 0));
	physx::PxQuat pitchRot(pitch, physx::PxVec3(1, 0, 0));
	physx::PxQuat rotation = yawRot * pitchRot;

	return rotation.rotate(origin);
}
