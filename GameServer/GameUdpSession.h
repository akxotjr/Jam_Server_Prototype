#pragma once

class GameUdpSession : public core::network::ReliableUdpSession
{
public:
	GameUdpSession() = default;
	virtual ~GameUdpSession() override = default;

	/* Session impl */
	virtual void		OnConnected() override;
	virtual void		OnDisconnected() override;
	virtual void		OnRecv(BYTE* buffer, int32 len) override;
	virtual void		OnSend(int32 len) override;
};

