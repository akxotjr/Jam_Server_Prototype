#pragma once

class GameUdpSession : public network::ReliableUdpSession
{
public:
	GameUdpSession();
	virtual ~GameUdpSession();


	virtual void		OnConnected() override;
	virtual void		OnDisconnected() override;
	virtual void		OnRecv(BYTE* buffer, int32 len) override;
	virtual void		OnSend(int32 len) override;
};

