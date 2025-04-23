#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "Memory.h"
#include "DeadLockProfiler.h"
#include "GlobalQueue.h"
#include "JobTimer.h"
#include "SocketUtils.h"
#include "SendBuffer.h"

//ThreadManager*		GThreadManager = nullptr;
Memory*				GMemory = nullptr;
//DeadLockProfiler*	GDeadLockProfiler = nullptr;
//GlobalQueue*		GGlobalQueue = nullptr;
//JobTimer*			GJobTimer = nullptr;

SendBufferManager*	GSendBufferManager = nullptr;


namespace core
{
	class CoreGlobal
	{	
	public:
		CoreGlobal()
		{
			//GThreadManager = new ThreadManager();
			GMemory = new Memory();
			GSendBufferManager = new SendBufferManager();
			//GGlobalQueue = new GlobalQueue();
			//GJobTimer = new JobTimer();
			//GDeadLockProfiler = new DeadLockProfiler();

			thread::ThreadManager::Instance().Init();	// for Main Thread
			SocketUtils::Init();
		}

		~CoreGlobal()
		{
			//delete GThreadManager;
			delete GMemory;
			delete GSendBufferManager;
			//delete GGlobalQueue;
			//delete GJobTimer;
			//delete GDeadLockProfiler;

			thread::ThreadManager::Instance().Shutdown();
			SocketUtils::Clear();
		}
	} GCoreGlobal;
}