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

		void					Launch(function<void(void)> callback);
		void					Join();

		void					InitTLS();
		void					DestroyTLS();

		void					DoGlobalQueueWork();
		void					DistributeReservedJob();

	private:
		Mutex					_lock;
		vector<std::thread>		_threads;
	};
}

