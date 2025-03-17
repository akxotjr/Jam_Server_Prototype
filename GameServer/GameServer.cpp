#include "pch.h"
#include "ClientPacketHandler.h"
#include "GameSession.h"

int main()
{
	ClientPacketHandler::Init();

	ServerServiceRef service = MakeShared<ServerService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IocpCore>(),
		MakeShared<GameSession>, // TODO : SessionManager µî
		100);



	return 0;
}