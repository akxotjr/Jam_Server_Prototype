#include "pch.h"
#include "GameUdpSession.h"
#include "IdManager.h"

GameUdpSession::GameUdpSession()
{
	uint32 id = GIdManager.Generate(IdType::Session);
	SetId(id);
}

GameUdpSession::~GameUdpSession()
{
}

void GameUdpSession::OnConnected()
{
	cout << "GameUdpSession [" << GetId() << "] : OnConnected\n";

	// todo add to sessionManager
}

void GameUdpSession::OnDisconnected()
{
	cout << "GameUdpSession [" << GetId() << "] : OnDisconnected\n";
}

void GameUdpSession::OnRecv(BYTE* buffer, int32 len)
{

}

void GameUdpSession::OnSend(int32 len)
{
	cout << "GameUdpSession [" << GetId() << "] : OnSend" << len << " bytes\n";
}
