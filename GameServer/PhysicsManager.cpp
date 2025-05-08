#include "pch.h"
#include "PhysicsManager.h"

#include "GlobalQueue.h"
#include "TimeManager.h"

void PhysicsManager::Init()
{
	_pxFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, _allocatorCallback, _errorCallback);
	_pxPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *_pxFoundation, physx::PxTolerancesScale(), true, nullptr);

	_pxSceneDesc.gravity = physx::PxVec3(0.0f, -9.8f, 0.0f);
	_pxSceneDesc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(1);	// TODO
	_pxSceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
}

void PhysicsManager::Shutdown()
{
	if (_pxPhysics) _pxPhysics->release();
	if (_pxFoundation) _pxFoundation->release();
}

int32 PhysicsManager::Do()
{
	int32 workCount = 0;

	while (true)
	{
		double now = TimeManager::Instance().GetServerTime();
		if (now > core::thread::LEndTime)
			break;

		// 

		++workCount;
	}
	return workCount;
}

void PhysicsManager::AddPhysicsQueue(uint32 roomId, shared_ptr<core::thread::LockQueue<job::Job>> physicsQueue)
{
	_physicsQueues.insert({ roomId, physicsQueue });
}

void PhysicsManager::RemovePhysicsQueue(uint32 roomId)
{
	auto it = _physicsQueues.find(roomId);
	if (it == _physicsQueues.end()) return;

	_physicsQueues.erase(roomId);
}



physx::PxScene* PhysicsManager::CreatePxScene()
{
	return _pxPhysics->createScene(_pxSceneDesc);
}

