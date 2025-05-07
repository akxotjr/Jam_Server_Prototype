#include "pch.h"
#include "GameUdpSession.h"
#include "SessionManager.h"

void GameUdpSession::OnConnected()
{
	cout << "[UDP] OnConnected\n";
}

void GameUdpSession::OnDisconnected()
{
	cout << "[UDP] ID = " << GetId() << " : OnDisconnected\n";
	SessionManager::Instance().Remove(GetSessionRef());
}

void GameUdpSession::OnRecv(BYTE* buffer, int32 len)
{

}

void GameUdpSession::OnSend(int32 len)
{
	//cout << "Udp OnSend : " << GetId() << endl;
	//cout << "GameUdpSession [" << GetId() << "] : OnSend" << len << " bytes\n";
}
