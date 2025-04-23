#include "pch.h"
#include "GlobalQueue.h"

namespace core::job
{
	void GlobalQueue::Push(const JobQueueRef& jobQueue)
	{
		_jobQueues.Push(jobQueue);
	}

	JobQueueRef GlobalQueue::Pop()
	{
		return _jobQueues.Pop();
	}

}

