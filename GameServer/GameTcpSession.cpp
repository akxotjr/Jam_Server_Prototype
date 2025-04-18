#include "pch.h"
#include "GameTcpSession.h"
#include "IdManager.h"
#include "ClientPacketHandler.h"
#include "SessionManager.h"
#include "Room.h"

GameTcpSession::GameTcpSession()
{
	uint32 id = GIdManager.Generate(IdType::Session);
	SetId(id);
}

GameTcpSession::~GameTcpSession()
{
}

void GameTcpSession::OnConnected()
{
	cout << "GameTcpSession [" << GetId() << "] : OnConnected\n";
	//GSessionManager.Add(static_pointer_cast<GameTcpSession>(shared_from_this()));
}

void GameTcpSession::OnDisconnected()
{
	cout << "GameTcpSession [" << GetId() << "] : OnDisconnected\n";
	//GSessionManager.Remove(static_pointer_cast<GameTcpSession>(shared_from_this()));

	if (_currentPlayer)
	{
		if (auto room = _room.lock())
		{
			room->DoAsync(&Room::Leave, _currentPlayer);
		}
	}

	_currentPlayer = nullptr;
}

void GameTcpSession::OnRecv(BYTE* buffer, int32 len)
{
	cout << "GameTcpSession [" << GetId() << "] : OnRecv" << len << " bytes\n";
	SessionRef session = GetSessionRef();

	// TODO : packetId 대역 체크
	ClientPacketHandler::HandlePacket<TcpPacketHeader>(session, buffer, len);
}

void GameTcpSession::OnSend(int32 len)
{
	cout << "GameTcpSession [" << GetId() << "] : OnSend" << len << " bytes\n";
}
