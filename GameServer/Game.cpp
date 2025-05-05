#include "pch.h"
#include "Game.h"
#include "RoomManager.h"
#include <TimeManager.h>
#include <ThreadManager.h>
#include "GameUdpReceiver.h"
#include "ClientPacketHandler.h"
#include "IdManager.h"
#include "Service.h"
#include "Values.h"
#include "GameTcpSession.h"
#include "GameUdpSession.h"

Game::Game()
{
	_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, _allocatorCallback, _errorCallback);

	network::TransportConfig config = {
		.tcpAddress = network::NetAddress(L"127.0.0.1", 7777),
		.udpAddress = network::NetAddress(L"127.0.0.1", 8888)
	};	// TODO : temp 

	_service = MakeShared<network::Service>(config, MakeShared<network::IocpCore>(), 50, 50);
	_service->SetSessionFactory<GameTcpSession, GameUdpSession>();
	_service->SetUdpReceiver(MakeShared<GameUdpReceiver>());
}

Game::~Game()
{
	_foundation->release();
	_foundation = nullptr;
}

void Game::Init()
{
	ClientPacketHandler::Init();
	IdManager::Instance().Init();
	RoomManager::Instance().Init(_foundation);


	// temp
	{
		RoomRef room = RoomManager::Instance().CreateRoom();	// <- room id = 1
		RoomManager::Instance().AddRoom(room);
	}
}

void Game::Loop()
{
	ASSERT_CRASH(_service->Start())

	WorkerThreadLoop();
	MainThreadLoop();
}

void Game::MainThreadLoop()
{
	double elapsedTime = 0.0;

	while (true)
	{
		core::TimeManager::Instance().Update();
		elapsedTime += core::TimeManager::Instance().GetDeltaTime();

		while (elapsedTime >= TICK_INTERVAL_S)
		{
			RoomManager::Instance().Update();
			elapsedTime -= TICK_INTERVAL_S;
		}
	}

	core::thread::ThreadManager::Instance().Join();
}

void Game::WorkerThreadLoop()
{
	for (int32 i = 0; i < 5; i++)
	{
		core::thread::ThreadManager::Instance().Launch([this]()
			{
				DoWorkerJob();
			});
	}
}

void Game::DoWorkerJob()
{
	while (true)
	{
		_service->GetIocpCore()->Dispatch(10);

		core::thread::ThreadManager::Instance().DistributeReservedJob();

		int32 workCount = core::thread::ThreadManager::Instance().DoGlobalQueueWork();

		double nextTime = 0.0;

		if (workCount == 0)
			nextTime = 0.01;
		else if (workCount < 5)
			nextTime = 0.005;
		else
			nextTime = 0.001;

		core::thread::LEndTime = TimeManager::Instance().GetServerTime() + nextTime;

		std::this_thread::yield();
	}
}
