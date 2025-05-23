#pragma once
#include "Allocator.h"

namespace core::memory
{
	class MemoryPool;

	/*-------------
		Memory
	---------------*/

	class Memory
	{
		enum
		{
			// ~1024까지 32단위, ~2048까지 128단위, ~4096까지 256단위
			POOL_COUNT = (1024 / 32) + (1024 / 128) + (2048 / 256),
			MAX_ALLOC_SIZE = 4096
		};

	public:
		Memory();
		~Memory();

		void*						Allocate(int32 size);
		void						Release(void* ptr);

	private:
		std::vector<MemoryPool*>	_pools;
		MemoryPool*					_poolTable[MAX_ALLOC_SIZE + 1];
	};


	template<typename Type, typename... Args>
	Type* xnew(Args&&... args)
	{
		Type* memory = static_cast<Type*>(PoolAllocator::Alloc(sizeof(Type)));
		new(memory)Type(std::forward<Args>(args)...); // placement new
		return memory;
	}

	template<typename Type>
	void xdelete(Type* obj)
	{
		obj->~Type();
		PoolAllocator::Release(obj);
	}

	template<typename Type, typename... Args>
	shared_ptr<Type> MakeShared(Args&&... args)
	{
		return std::shared_ptr<Type>{ xnew<Type>(std::forward<Args>(args)...), xdelete<Type> };
	}
}

