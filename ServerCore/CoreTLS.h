#pragma once

#include <stack>

#include "JobQueue.h"

namespace core::thread
{
	extern thread_local uint32										LThreadId;
	extern thread_local double										LEndTime;
	extern thread_local std::stack<int32>							LLockStack;
	extern thread_local network::SendBufferChunkRef					LSendBufferChunk;
	extern thread_local job::JobQueue*								LCurrentJobQueue;
}
