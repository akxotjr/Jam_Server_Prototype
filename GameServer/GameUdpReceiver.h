#pragma once
#include "UdpReceiver.h"

class GameUdpReceiver : public UdpReceiver
{
public:
	GameUdpReceiver();
	virtual ~GameUdpReceiver();

	virtual void OnRecv(SessionRef& session, BYTE* buffer, int32 len) override;
};

