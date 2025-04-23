#pragma once

namespace core::job
{
	class GlobalQueue
	{
		DECLARE_SINGLETON(GlobalQueue)

	public:
		void					Push(const JobQueueRef& jobQueue);
		JobQueueRef				Pop();

	private:
		LockQueue<JobQueueRef>	_jobQueues;
	};
}

