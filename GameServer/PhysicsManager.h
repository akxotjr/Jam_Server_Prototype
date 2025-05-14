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




struct ColliderInfo
{
	enum class Type { Box, Capsule, Sphere, Plane };

	Type type;

	union
	{
		struct { float radius, halfHeight; } capsule;
		struct { float radius; } sphere;
		struct { float hx, hy, hz; } box;
	};

	// 로컬 변환 (필요 시)
	physx::PxVec3 localOffset = physx::PxVec3(0, 0, 0);
	physx::PxQuat localRotation = physx::PxQuat(0, 0, 0, 1);

	static ColliderInfo MakeCapsule(float radius, float halfHeight)
	{
		ColliderInfo info;
		info.type = Type::Capsule;
		info.capsule = { radius, halfHeight };
		return info;
	}

	static ColliderInfo MakeSphere(float radius)
	{
		ColliderInfo info;
		info.type = Type::Sphere;
		info.sphere = { radius };
		return info;
	}

	static ColliderInfo MakeBox(float hx, float hy, float hz)
	{
		ColliderInfo info;
		info.type = Type::Box;
		info.box = { hx, hy, hz };
		return info;
	}

	static ColliderInfo MakePlane()
	{
		ColliderInfo info;
		info.type = Type::Plane;
		return info;
	}
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
	physx::PxMaterial*				CreateMaterial(physx::PxReal staticFriction, physx::PxReal dynamicFriction, physx::PxReal restitution);
	physx::PxRigidStatic*			CreateRigidStatic(physx::PxVec3& position, physx::PxQuat& rotation);
	physx::PxRigidStatic*			CreatePlane(float nx, float ny, float nz, float distance);
	physx::PxRigidDynamic*			CreateRigidDynamic(physx::PxVec3& position, physx::PxQuat& rotation);

	physx::PxShape*					CreateShape(const ColliderInfo& info);

	std::unordered_set<uint32>&					GetAllRoomIds() { return _allRoomIds; }

	physx::PxMaterial*				GetDefaultMaterial() { return _defaultMaterial; }

private:
	physx::PxFoundation*			_pxFoundation = nullptr;
	physx::PxDefaultAllocator		_allocatorCallback;
	physx::PxDefaultErrorCallback	_errorCallback;

	physx::PxPhysics*				_pxPhysics = nullptr;

	physx::PxSceneDesc				_pxSceneDesc = physx::PxSceneDesc(physx::PxTolerancesScale());

	physx::PxMaterial*				_defaultMaterial = nullptr;

	Vector<std::unique_ptr<PhysicsWorker>> _workers;
	std::unordered_set<uint32> _allRoomIds;
	std::unordered_map<uint32, shared_ptr<core::thread::LockQueue<job::Job>>> _physicsQueues;	// key - roomId
};

