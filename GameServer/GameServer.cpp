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

enum
{
	WORKER_TICk = 64
};

void DoWorkerJob(ServiceRef& service)
{
	while (true)
	{
		// TODO
		// hardcoding 된 WORKER_TICK을 DoGlobalQueueWork() 에 맞춰 자동보정되게 설정
		LEndTickCount = ::GetTickCount64() + WORKER_TICk;

		service->GetIocpCore()->Dispatch(10);

		// 예약된 일감 처리
		ThreadManager::DistributeReservedJob();

		// 글로벌 큐
		ThreadManager::DoGlobalQueueWork();
	}
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

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([&service]()
			{
				DoWorkerJob(service);
			});
	}

	// Main Thread
	
	while (true)
	{
		GTimeManager.Update();
		GRoomManager.Update();

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	GThreadManager->Join();

	return 0;
}