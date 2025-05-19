#include "pch.h"
#include "CoreGlobal.h"
#include "SocketUtils.h"
#include "ThreadManager.h"
#include "TimeManager.h"
#include "Logger.h"


namespace core
{
	memory::Memory* GMemory = nullptr;

	class CoreGlobal
	{	
	public:
		CoreGlobal()
		{
			GMemory = new memory::Memory();
			TimeManager::Instance().Init();
			thread::ThreadManager::Instance().Init();	// for Main Thread
			network::SocketUtils::Init();
			Logger::Instance().Init();
		}

		~CoreGlobal()
		{
			delete GMemory;

			thread::ThreadManager::Instance().Shutdown();
			network::SocketUtils::Clear();
		}
	} GCoreGlobal;
}
