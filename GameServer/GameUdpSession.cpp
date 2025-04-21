#include "pch.h"
#include "GameUdpSession.h"
#include "IdManager.h"
#include "SessionManager.h"

GameUdpSession::GameUdpSession()
{
	//uint32 id = GIdManager.Generate(IdType::Session);
	//SetId(id);
}

GameUdpSession::~GameUdpSession()
{
}

void GameUdpSession::OnConnected()
{
	cout << "[UDP] OnConnected\n";

	// todo add to sessionManager
	//GSessionManager.Add(GetSessionRef());
}

void GameUdpSession::OnDisconnected()
{
	cout << "[UDP] ID = " << GetId() << " : OnDisconnected\n";
	GSessionManager.Remove(GetSessionRef());
}

void GameUdpSession::OnRecv(BYTE* buffer, int32 len)
{

}

void GameUdpSession::OnSend(int32 len)
{
	//cout << "Udp OnSend : " << GetId() << endl;
	//cout << "GameUdpSession [" << GetId() << "] : OnSend" << len << " bytes\n";
}
