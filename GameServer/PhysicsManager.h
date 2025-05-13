#pragma once


class PhysicsWorker
{
public:
	void							AssignRoom(uint32 roomId);
	void							UnassignRoom(uint32 roomId);
	int32							Execute();

	void SetSharedQueueMap(std::unordered_map<uint32, std::shared_ptr<core::thread::LockQueue<job::Job>>>* sharedQueue) { _sharedQueues = sharedQueue; }
	void SetWorkerIndex(int32 workerIndex) { _workerIndex = workerIndex; }

private:
	int32 TryExecute(uint32 roomId);


private:
	std::unordered_set<uint32>				_assignedRoomIds;

	std::unordered_map<uint32, std::shared_ptr<core::thread::LockQueue<job::Job>>>* _sharedQueues = nullptr;
	int32 _workerIndex = 0;
};

extern thread_local PhysicsWorker* LPhysicsWorker;


class PhysicsManager
{
	DECLARE_SINGLETON(PhysicsManager)

public:
	void							Init(int32 numWorkers);
	void							Shutdown();

	int32							Execute();

	void							AddPhysicsQueue(uint32 roomId, shared_ptr<core::thread::LockQueue<job::Job>> physicsQueue);
	void							RemovePhysicsQueue(uint32 roomId);

	physx::PxScene*					CreatePxScene();
	physx::PxMaterial*				CreateMaterial(physx::PxReal staticFriction, physx::PxReal dynamicFriction, physx::PxReal restitution);
	physx::PxRigidStatic*			CreateRigidStatic(physx::PxVec3& position, physx::PxQuat& rotation);
	physx::PxRigidStatic*			CreatePlane(float nx, float ny, float nz, float distance);
	physx::PxRigidDynamic*			CreateRigidDynamic(physx::PxVec3& position, physx::PxQuat& rotation);

	physx::PxShape*					CreateShape(EColliderType type, const physx::PxVec3& size);

	std::unordered_set<uint32>&					GetAllRoomIds() { return _allRoomIds; }

private:
	physx::PxFoundation*			_pxFoundation = nullptr;
	physx::PxDefaultAllocator		_allocatorCallback;
	physx::PxDefaultErrorCallback	_errorCallback;

	physx::PxPhysics*				_pxPhysics = nullptr;

	physx::PxSceneDesc				_pxSceneDesc = physx::PxSceneDesc(physx::PxTolerancesScale());


	Vector<std::unique_ptr<PhysicsWorker>> _workers;
	std::unordered_set<uint32> _allRoomIds;
	std::unordered_map<uint32, shared_ptr<core::thread::LockQueue<job::Job>>> _physicsQueues;	// key - roomId
};

