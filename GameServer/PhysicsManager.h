#pragma once


class PhysicsWorker
{
public:
	void							AssignRoom(uint32 roomId);
	int32							Execute();

	void SetSharedQueueMap(std::unordered_map<uint32, std::shared_ptr<core::thread::LockQueue<job::Job>>>* sharedQueue) { _sharedQueues = sharedQueue; }
	void SetWorkerIndex(int32 workerIndex) { _workerIndex = workerIndex; }

private:
	int32 TryExecuute(uint32 roomId);


private:
	Vector<uint32>				_assignedRoomIds;

	std::unordered_map<uint32, std::shared_ptr<core::thread::LockQueue<job::Job>>>* _sharedQueues = nullptr;
	int32 _workerIndex = 0;
};


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

	Vector<uint32>&					GetAllRoomIds() { return _allRoomIds; }

private:
	physx::PxFoundation*			_pxFoundation = nullptr;
	physx::PxDefaultAllocator		_allocatorCallback;
	physx::PxDefaultErrorCallback	_errorCallback;

	physx::PxPhysics*				_pxPhysics = nullptr;

	physx::PxSceneDesc				_pxSceneDesc = physx::PxSceneDesc(physx::PxTolerancesScale());


	Vector<std::unique_ptr<PhysicsWorker>> _workers;
	Vector<uint32> _allRoomIds;
	std::unordered_map<uint32, shared_ptr<core::thread::LockQueue<job::Job>>> _physicsQueues;	// key - roomId
};

