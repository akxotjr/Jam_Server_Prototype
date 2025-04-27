#include "pch.h"
#include "ThreadManager.h"
#include "GlobalQueue.h"
#include "JobQueue.h"
#include "TimeManager.h"

/*------------------
	ThreadManager
-------------------*/

namespace core::thread
{
	void ThreadManager::Launch(std::function<void(void)> callback)
	{
		LockGuard guard(_lock);
	
		_threads.push_back(std::thread([=]()
			{
				InitTLS();
				callback();
				DestroyTLS();
			}));
	}
	
	void ThreadManager::Join()
	{
		for (std::thread& t : _threads)
		{
			if (t.joinable())
				t.join();
		}
		_threads.clear();
	}
	
	void ThreadManager::InitTLS()
	{
		static Atomic<uint32> SThreadId = 1;
		LThreadId = SThreadId.fetch_add(1);
	}
	
	void ThreadManager::DestroyTLS()
	{
	
	}
	
	int32 ThreadManager::DoGlobalQueueWork()
	{
		int32 workCount = 0;

		while (true)
		{
			double now = TimeManager::Instance().GetServerTime();
			if (now > LEndTime)
				break;

			job::JobQueueRef jobQueue = job::GlobalQueue::Instance().Pop();
			if (jobQueue == nullptr)
				break;
	
			jobQueue->Execute();
			++workCount;
		}
		return workCount;
	}
	
	void ThreadManager::DistributeReservedJob()
	{
		const double now = TimeManager::Instance().GetServerTime();
	
		job::JobTimer::Instance().Distribute(now);
	}

}