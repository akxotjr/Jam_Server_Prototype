#include "pch.h"
#include "PhysicsManager.h"
#include "TimeManager.h"

void PhysicsWorker::AssignRoom(uint32 roomId)
{
	_assignedRoomIds.insert(roomId);
}

void PhysicsWorker::UnassignRoom(uint32 roomId)
{
	_assignedRoomIds.erase(roomId);
}

int32 PhysicsWorker::Execute()
{
	int32 workerCount = 0;

	bool didWork = false;

	for (uint32 roomId : _assignedRoomIds)
	{
		if (int32 count = TryExecute(roomId))
		{
			workerCount += count;
			didWork = true;
		}
	}

	if (!didWork)
	{
		for (uint32 roomId : PhysicsManager::Instance().GetAllRoomIds())
		{
			auto it = _assignedRoomIds.find(roomId);
			if (it != _assignedRoomIds.end())
				continue;

			if (int32 count = TryExecute(roomId))
			{
				workerCount += count;
			}
		}
	}

	return workerCount;
}

int32 PhysicsWorker::TryExecute(uint32 roomId)
{
	int32 workCount = 0;

	double now = TimeManager::Instance().GetServerTime();
	if (now >= core::thread::LEndTime)
		return workCount;

	auto it = _sharedQueues->find(roomId);
	if (it == _sharedQueues->end())
		return 0;

	auto lockQueue = it->second;

	while (true)
	{
		double nowInner = TimeManager::Instance().GetServerTime();
		if (nowInner >= core::thread::LEndTime)
			break;

		auto job = lockQueue->TryPop();
		if (!job.has_value())
			break;

		job->Execute();

		++workCount;
	}

	return workCount;
}

thread_local PhysicsWorker* LPhysicsWorker = nullptr;


void PhysicsManager::Init(int32 numWorkers)
{
	_pxFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, _allocatorCallback, _errorCallback);
	_pxPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *_pxFoundation, physx::PxTolerancesScale(), true, nullptr);

	_pxSceneDesc.gravity = physx::PxVec3(0.0f, -9.8f, 0.0f);
	_pxSceneDesc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(1);	// TODO
	_pxSceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;

	_workers.reserve(numWorkers);
	for (int32 i = 0; i < numWorkers; ++i)
	{
		auto worker = std::make_unique<PhysicsWorker>();
		worker->SetWorkerIndex(i);
		worker->SetSharedQueueMap(&_physicsQueues);
		_workers.push_back(std::move(worker));
	}
}

void PhysicsManager::Shutdown()
{
	if (_pxPhysics) _pxPhysics->release();
	if (_pxFoundation) _pxFoundation->release();

	// TODO : LPhysicsWorker nullptr 로 밀어야되는데 
}

int32 PhysicsManager::Execute()
{
	int32 workCount = 0;

	if (LPhysicsWorker == nullptr)
	{
		int32 workerIndex = core::thread::LThreadId % _workers.size();
		LPhysicsWorker = _workers[workerIndex].get();
	}

	while (true)
	{
		double now = TimeManager::Instance().GetServerTime();
		if (now >= core::thread::LEndTime)
			break;

		workCount += LPhysicsWorker->Execute();
	}
	return workCount;
}

void PhysicsManager::AddPhysicsQueue(uint32 roomId, shared_ptr<core::thread::LockQueue<job::Job>> physicsQueue)
{
	if (_physicsQueues.contains(roomId))
		return;

	_physicsQueues.insert({ roomId, physicsQueue });
	_allRoomIds.insert(roomId);

	int32 workerIndex = static_cast<int32>(roomId) % _workers.size();
	_workers[workerIndex]->AssignRoom(roomId);
}

void PhysicsManager::RemovePhysicsQueue(uint32 roomId)
{
	int32 workerIndex = static_cast<int32>(roomId) % _workers.size();
	_workers[workerIndex]->UnassignRoom(roomId);

	_allRoomIds.erase(roomId);

	auto it = _physicsQueues.find(roomId);
	if (it == _physicsQueues.end()) return;

	_physicsQueues.erase(roomId);
}

physx::PxScene* PhysicsManager::CreatePxScene()
{
	return _pxPhysics->createScene(_pxSceneDesc);
}

physx::PxMaterial* PhysicsManager::CreateMaterial(physx::PxReal staticFriction, physx::PxReal dynamicFriction, physx::PxReal restitution)
{
	return _pxPhysics->createMaterial(staticFriction, dynamicFriction, restitution);
}

physx::PxRigidActor* PhysicsManager::CreateRigidStatic(physx::PxVec3& position, physx::PxQuat& rotation)
{
	return _pxPhysics->createRigidStatic(physx::PxTransform(position, rotation));
}

physx::PxRigidDynamic* PhysicsManager::CreateRigidDynamic(physx::PxVec3& position, physx::PxQuat& rotation)
{
	return _pxPhysics->createRigidDynamic(physx::PxTransform(position, rotation));
}
