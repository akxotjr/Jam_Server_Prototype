#include "pch.h"
#include "ClientPacketHandler.h"
#include "Service.h"
#include "GameSession.h"
#include "ThreadManager.h"
#include "RoomManager.h"
#include "Room.h"

enum
{
	WORKER_TICk = 64
};

void DoWorkerJob(ServerServiceRef& service)
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
	ClientPacketHandler::Init();

	// temp
	//{
	//	uint32 roomId = GRoomManager.CreateRoom();
	//	RoomRef room = GRoomManager.GetRoomById(roomId);
	//	for (int i = 0; i < 4; i++)
	//	{
	//		BotRef bot = MakeShared<Bot>();
	//		room->AddBot(bot);
	//	}
	//}

	ServerServiceRef service = MakeShared<ServerService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IocpCore>(),
		MakeShared<GameSession>, // TODO : SessionManager 등
		100);


	ASSERT_CRASH(service->Start());

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([&service]()
			{
				DoWorkerJob(service);
			});
	}

	// Main Thread
	DoWorkerJob(service);

	GThreadManager->Join();

	return 0;
}