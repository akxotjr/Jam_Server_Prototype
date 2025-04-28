#include "pch.h"

#include <TimeManager.h>

#include "ClientPacketHandler.h"
#include "Service.h"
#include "GameTcpSession.h"
#include "GameUdpSession.h"
#include "GameUdpReceiver.h"
#include "ThreadManager.h"
#include "RoomManager.h"
#include "Room.h"
#include "Bot.h"
#include "IdManager.h"
#include "Values.h"


void DoWorkerJob(network::ServiceRef& service)
{
	while (true)
	{
		service->GetIocpCore()->Dispatch(10);

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

		//std::this_thread::yield();
	}
}

void MainLoop()
{
	double elapsedTime = 0.0;

	while (true)
	{
		TimeManager::Instance().Update();
		elapsedTime += TimeManager::Instance().GetDeltaTime();

		while (elapsedTime >= TICK_INTERVAL_S)
		{
			GRoomManager.Update();
			elapsedTime -= TICK_INTERVAL_S;
		}
	}

	core::thread::ThreadManager::Instance().Join();
}

int main()
{
	ClientPacketHandler::Init();

	// temp
	{
		uint32 roomId = GRoomManager.CreateRoom();
		RoomRef room = GRoomManager.GetRoomById(roomId);
		for (int i = 0; i < 4; i++)
		{
			BotRef bot = MakeShared<Bot>();
			bot->Init();
			room->AddCharacter(bot);
		}
	}

	network::TransportConfig config = {
		.tcpAddress = network::NetAddress(L"127.0.0.1", 7777),
		.udpAddress = network::NetAddress(L"127.0.0.1", 8888)
	};

	network::ServiceRef service = MakeShared<network::Service>(config, MakeShared<network::IocpCore>(), 50, 50);
	service->SetSessionFactory<GameTcpSession, GameUdpSession>();
	service->SetUdpReceiver(MakeShared<GameUdpReceiver>());
	ASSERT_CRASH(service->Start());


	for (int32 i = 0; i < 5; i++)
	{
		core::thread::ThreadManager::Instance().Launch([&service]()
			{
				DoWorkerJob(service);
			});
	}

	// Main Thread
	MainLoop();

	return 0;
}