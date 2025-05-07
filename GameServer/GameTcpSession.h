#pragma once

class GameTcpSession : public core::network::TcpSession
{
public:
	GameTcpSession() = default;
	virtual ~GameTcpSession() override = default;

	/* Session impl */
	virtual void			OnConnected() override;
	virtual void			OnDisconnected() override;
	virtual void			OnRecv(BYTE* buffer, int32 len) override;
	virtual void			OnSend(int32 len) override;

	void					SetRoom(RoomRef room) { _room = room; }
	RoomRef					GetRoom() { return _room.lock(); }

public:
	PlayerRef				_currentPlayer;
	std::weak_ptr<Room>		_room;
};

