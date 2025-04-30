#pragma once

class GameTcpSession : public core::network::TcpSession
{
public:
	GameTcpSession();
	virtual ~GameTcpSession();


	virtual void		OnConnected() override;
	virtual void		OnDisconnected() override;
	virtual void		OnRecv(BYTE* buffer, int32 len) override;
	virtual void		OnSend(int32 len) override;

	void				SetRoom(RoomRef room) { _room = room; }
	RoomRef				GetRoom() { return _room.lock(); }

public:
	PlayerRef			_currentPlayer;
	weak_ptr<Room>		_room;
};

