#include "pch.h"
#include "CoreGlobal.h"
#include "Memory.h"
#include "SocketUtils.h"
#include "ThreadManager.h"

core::memory::Memory*				GMemory = nullptr;


namespace core
{
	class CoreGlobal
	{	
	public:
		CoreGlobal()
		{
			GMemory = new memory::Memory();

			thread::ThreadManager::Instance().Init();	// for Main Thread
			network::SocketUtils::Init();
		}

		~CoreGlobal()
		{
			delete GMemory;

			thread::ThreadManager::Instance().Shutdown();
			network::SocketUtils::Clear();
		}
	} GCoreGlobal;
}