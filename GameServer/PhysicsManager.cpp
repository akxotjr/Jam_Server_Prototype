#include "pch.h"
#include "PhysicsManager.h"
#include "TimeManager.h"


void PhysicsWorker::AssignRoom(uint32 roomId)
{
	_assignedRoomIds.push_back(roomId);
}

int32 PhysicsWorker::Execute()
{
	int32 workerCount = 0;

	bool didWork = false;

	for (uint32 roomId : _assignedRoomIds)
	{
		if (int32 count = TryExecuute(roomId))
		{
			workerCount += count;
			didWork = true;
		}
	}

	if (!didWork)
	{
		for (uint32 roomId : PhysicsManager::Instance().GetAllRoomIds())
		{
			if (int32 count = TryExecuute(roomId))
			{
				workerCount += count;
			}
		}
	}

	return workerCount;
}

int32 PhysicsWorker::TryExecuute(uint32 roomId)
{
	int32 workCount = 0;

	double now = TimeManager::Instance().GetServerTime();
	if (now > core::thread::LEndTime)
		return workCount;

	auto it = _sharedQueues->find(roomId);
	if (it == _sharedQueues->end())
		return 0;

	auto lockQueue = it->second;

	while (true)
	{
		double nowInner = TimeManager::Instance().GetServerTime();
		if (nowInner > core::thread::LEndTime)
			break;

		auto job = lockQueue->TryPop();
		if (!job.has_value())
			break;

		job->Execute();

		++workCount;
	}

	return workCount;
}


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
}

int32 PhysicsManager::Execute()
{
	int32 workCount = 0;

	while (true)
	{
		double now = TimeManager::Instance().GetServerTime();
		if (now > core::thread::LEndTime)
			break;

		for (auto& worker : _workers)
		{
			workCount += worker->Execute();
		}
	}
	return workCount;
}

void PhysicsManager::AddPhysicsQueue(uint32 roomId, shared_ptr<core::thread::LockQueue<job::Job>> physicsQueue)
{
	_physicsQueues.insert({ roomId, physicsQueue });
	_allRoomIds.push_back(roomId);

	int32 workerIndex = static_cast<int32>(roomId) % _workers.size();
	_workers[workerIndex]->AssignRoom(roomId);
}

void PhysicsManager::RemovePhysicsQueue(uint32 roomId)
{
	_allRoomIds.erase(std::remove(_allRoomIds.begin(), _allRoomIds.end(), roomId), _allRoomIds.end());

	auto it = _physicsQueues.find(roomId);
	if (it == _physicsQueues.end()) return;

	_physicsQueues.erase(roomId);
}



physx::PxScene* PhysicsManager::CreatePxScene()
{
	return _pxPhysics->createScene(_pxSceneDesc);
}
