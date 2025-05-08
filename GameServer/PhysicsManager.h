#pragma once


class PhysicsManager
{
	DECLARE_SINGLETON(PhysicsManager)

public:
	void							Init();
	void							Shutdown();

	int32							Do();

	void							AddPhysicsQueue(uint32 roomId, shared_ptr<core::thread::LockQueue<job::Job>> physicsQueue);
	void							RemovePhysicsQueue(uint32 roomId);

	physx::PxScene*					CreatePxScene();

private:
	physx::PxFoundation*			_pxFoundation = nullptr;
	physx::PxDefaultAllocator		_allocatorCallback;
	physx::PxDefaultErrorCallback	_errorCallback;

	physx::PxPhysics*				_pxPhysics = nullptr;

	physx::PxSceneDesc				_pxSceneDesc = physx::PxSceneDesc(physx::PxTolerancesScale());


	unordered_map<uint32, shared_ptr<core::thread::LockQueue<job::Job>>> _physicsQueues;	// key - roomId
};

