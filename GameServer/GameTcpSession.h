#pragma once
#include <Session.h>

class GameTcpSession : public TcpSession
{
public:
	GameTcpSession();
	virtual ~GameTcpSession();


	virtual void OnConnected() override;
	virtual void OnDisconnected() override;
	virtual void OnRecv(BYTE* buffer, int32 len) override;
	virtual void OnSend(int32 len) override;

public:
	PlayerRef		_currentPlayer;
	weak_ptr<Room>	_room;
};

