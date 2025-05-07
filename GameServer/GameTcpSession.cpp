#include "pch.h"
#include "GameTcpSession.h"
#include "ClientPacketHandler.h"
#include "SessionManager.h"
#include "Room.h"

void GameTcpSession::OnConnected()
{
	cout << "[TCP] OnConnected\n";
}

void GameTcpSession::OnDisconnected()
{
	cout << "[TCP] User ID = " << GetId() << " : OnDisconnected\n";
	SessionManager::Instance().Remove(GetSessionRef());

	//if (_currentPlayer)
	//{
	//	if (auto room = _room.lock())
	//	{
	//		room->DoAsync(&Room::Leave, _currentPlayer);
	//	}
	//}

	//_currentPlayer = nullptr;
}

void GameTcpSession::OnRecv(BYTE* buffer, int32 len)
{
	SessionRef session = GetSessionRef();
	ClientPacketHandler::HandlePacket<TcpPacketHeader>(session, buffer, len);
}

void GameTcpSession::OnSend(int32 len)
{
}
