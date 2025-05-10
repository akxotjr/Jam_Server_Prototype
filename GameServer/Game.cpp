#include "pch.h"
#include "Game.h"
#include "RoomManager.h"
#include <TimeManager.h>
#include <ThreadManager.h>
#include "PhysicsManager.h"
#include "GameUdpReceiver.h"
#include "ClientPacketHandler.h"
#include "IdManager.h"
#include "Service.h"
#include "Values.h"
#include "GameTcpSession.h"
#include "GameUdpSession.h"

Game::Game()
{
	
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
	PhysicsManager::Instance().Shutdown();
}

void Game::Init()
{
	IdManager::Instance().Init();
	PhysicsManager::Instance().Init(2);
	ClientPacketHandler::Init();
	RoomManager::Instance().Init();
}

void Game::Loop()
{
	ASSERT_CRASH(_service->Start())

	PhysicsThreadLoop();		// TODO : ¼ø¼­ »ý°¢ÇØºÁ¾ßµÊ
	NetworkThreadLoop();
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

void Game::NetworkThreadLoop()
{
	for (int32 i = 0; i < 5; i++)
	{
		core::thread::ThreadManager::Instance().Launch([this]()
			{
				DoNetworkJob();
			});
	}
}

void Game::DoNetworkJob()
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

void Game::PhysicsThreadLoop()
{
	for (int32 i = 0; i < 1; i++)
	{
		core::thread::ThreadManager::Instance().Launch([this]()
			{
				DoPhysicsJob();
			});
	}
}

void Game::DoPhysicsJob()
{
	while (true)
	{
		int32 workCount = PhysicsManager::Instance().Execute();

		double nextTime = 0.0;

		if (workCount == 0)
			nextTime = 0.01;
		else if (workCount < 5)
			nextTime = 0.005;
		else
			nextTime = 0.001;

		core::thread::LEndTime = TimeManager::Instance().GetServerTime() + nextTime;
	}
}
