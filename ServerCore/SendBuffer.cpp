#include "pch.h"
#include "SendBuffer.h"

namespace core::network
{

	/*----------------
		SendBuffer
	-----------------*/

	SendBuffer::SendBuffer(SendBufferChunkRef owner, BYTE* buffer, int32 allocSize)
		: _owner(owner), _buffer(buffer), _allocSize(allocSize)
	{

	}

	void SendBuffer::Close(uint32 writeSize)
	{
		ASSERT_CRASH(_allocSize >= writeSize)
		_writeSize = writeSize;

		_owner->Close(writeSize);
	}




	/*---------------------
		SendBufferChunk
	----------------------*/

	SendBufferChunk::SendBufferChunk()
	{
	}

	SendBufferChunk::~SendBufferChunk()
	{
	}

	void SendBufferChunk::Reset()
	{
		_open = false;
		_usedSize = 0;
	}

	SendBufferRef SendBufferChunk::Open(uint32 allocSize)
	{
		ASSERT_CRASH(allocSize <= SEND_BUFFER_CHUNK_SIZE)
		ASSERT_CRASH(_open == false)

		if (allocSize > FreeSize())
			return nullptr;

		_open = true;

		return memory::ObjectPool<SendBuffer>::MakeShared(shared_from_this(), Buffer(), allocSize);
	}

	void SendBufferChunk::Close(uint32 writeSize)
	{
		ASSERT_CRASH(_open == true)

		_open = false;
		_usedSize += writeSize;
	}

	/*---------------------
	   SendBufferManager
	----------------------*/

	SendBufferRef SendBufferManager::Open(uint32 size)
	{
		if (thread::LSendBufferChunk == nullptr)
		{
			thread::LSendBufferChunk = Pop();
			thread::LSendBufferChunk->Reset();
		}

		ASSERT_CRASH(thread::LSendBufferChunk->IsOpen() == false)

		if (thread::LSendBufferChunk->FreeSize() < size)
		{
			thread::LSendBufferChunk = Pop();
			thread::LSendBufferChunk->Reset();
		}

		return thread::LSendBufferChunk->Open(size);
	}

	SendBufferChunkRef SendBufferManager::Pop()
	{
		{
			WRITE_LOCK
			if (_sendBufferChunks.empty() == false)
			{
				SendBufferChunkRef sendBufferChunk = _sendBufferChunks.back();
				_sendBufferChunks.pop_back();
				return sendBufferChunk;
			}
		}
		return SendBufferChunkRef(memory::xnew<SendBufferChunk>(), PushGlobal);
	}

	void SendBufferManager::Push(SendBufferChunkRef buffer)
	{
		WRITE_LOCK
		_sendBufferChunks.push_back(buffer);
	}

	void SendBufferManager::PushGlobal(SendBufferChunk* buffer)
	{
		SendBufferManager::Instance().Push(SendBufferChunkRef(buffer, PushGlobal));
	}

}
