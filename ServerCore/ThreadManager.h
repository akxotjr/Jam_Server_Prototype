#pragma once

#include <thread>
#include <functional>

/*------------------
	ThreadManager
-------------------*/

namespace core::thread
{
	class ThreadManager
	{
		DECLARE_SINGLETON(ThreadManager)

	public:
		void					Init() { InitTLS(); }
		void					Shutdown() { Join(); }

		void					Launch(std::function<void(void)> callback);
		void					Join();

		void					InitTLS();
		void					DestroyTLS();

		int32					DoGlobalQueueWork();
		void					DistributeReservedJob();

	private:
		Mutex							_lock;
		std::vector<std::thread>		_threads;
	};
}

