#pragma once

#include <stack>

#include "JobQueue.h"

extern thread_local uint32				LThreadId;
extern thread_local double						LEndTime;

extern thread_local std::stack<int32>			LLockStack;
extern thread_local core::network::SendBufferChunkRef			LSendBufferChunk;
extern thread_local core::job::JobQueue*		LCurrentJobQueue;