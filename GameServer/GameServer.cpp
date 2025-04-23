#include "pch.h"
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
#include "TimeManager.h"
#include "Values.h"


void DoWorkerJob(ServiceRef& service)
{
	while (true)
	{
		LEndTickCount = ::GetTickCount64() + WORKER_TICK_MS;

		service->GetIocpCore()->Dispatch(10);

		core::thread::ThreadManager::Instance().DistributeReservedJob();
		core::thread::ThreadManager::Instance().DoGlobalQueueWork();
	}
}

void MainLoop()
{
	//GTimeManager.Init();

	double elapsedTime = 0.0;

	while (true)
	{
		GTimeManager.Update();
		elapsedTime += GTimeManager.GetDeltaTime();

		while (elapsedTime >= TICK_INTERVAL_S)
		{
			GRoomManager.Update();
			elapsedTime -= TICK_INTERVAL_S;
		}
	}

	GThreadManager->Join();
}

int main()
{
	GTimeManager.Init();
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

	TransportConfig config = {
		.tcpAddress = NetAddress(L"127.0.0.1", 7777),
		.udpAddress = NetAddress(L"127.0.0.1", 8888)
	};

	ServiceRef service = MakeShared<Service>(config, MakeShared<IocpCore>(), 50, 50);
	service->SetSessionFactory<GameTcpSession, GameUdpSession>();
	service->SetUdpReceiver(MakeShared<GameUdpReceiver>());
	ASSERT_CRASH(service->Start());

	// Main Thread
	MainLoop();

	return 0;
}