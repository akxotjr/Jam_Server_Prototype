#pragma once

class Game
{
public:
	Game();
	~Game();

	void							Init();
	void							Loop();

private:
	void							MainThreadLoop();
	void							WorkerThreadLoop();
	void							DoWorkerJob();

private:
	physx::PxFoundation*			_foundation = nullptr;
	physx::PxDefaultAllocator		_allocatorCallback;
	physx::PxDefaultErrorCallback	_errorCallback;

	network::ServiceRef				_service = nullptr;
};

