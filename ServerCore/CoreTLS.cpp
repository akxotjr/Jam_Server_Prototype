#include "pch.h"
#include "CoreTLS.h"

#include "JobQueue.h"

namespace core::thread
{
	thread_local uint32									LThreadId = 0;
	thread_local double									LEndTime = 0.0;
	thread_local std::stack<int32>						LLockStack;
	thread_local network::SendBufferChunkRef		LSendBufferChunk;
	thread_local job::JobQueue*					LCurrentJobQueue = nullptr;
}
