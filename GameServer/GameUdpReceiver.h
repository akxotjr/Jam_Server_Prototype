#pragma once
#include "UdpReceiver.h"

class GameUdpReceiver : public core::network::UdpReceiver
{
public:
	GameUdpReceiver();
	virtual ~GameUdpReceiver();

	virtual void OnRecv(network::SessionRef& session, BYTE* buffer, int32 len) override;
};

