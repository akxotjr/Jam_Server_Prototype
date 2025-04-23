#pragma once

#include <thread>
#include <functional>

#include "Memory.h"
#include "ObjectPool.h"

/*------------------
	ThreadManager
-------------------*/

namespace core::thread
{
	class ThreadManager
	{
		DECLARE_SINGLETON(ThreadManager);

	public:
		void					Init() { InitTLS(); }
		void					Shutdown() { Join(); }

		void					Launch(std::function<void(void)> callback);
		void					Join();

		void					InitTLS();
		void					DestroyTLS();

		void					DoGlobalQueueWork();
		void					DistributeReservedJob();

	private:
		Mutex							_lock;
		std::vector<std::thread>		_threads;
	};
}

