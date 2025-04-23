#include "pch.h"
#include "CoreTLS.h"

#include "JobQueue.h"

thread_local uint32					LThreadId = 0;
thread_local double					LEndTime = 0.0;

thread_local std::stack<int32>		LLockStack;
thread_local SendBufferChunkRef		LSendBufferChunk;
thread_local core::job::JobQueue*	LCurrentJobQueue = nullptr;
