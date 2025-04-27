#include "pch.h"
#include "GameUdpReceiver.h"
#include "ClientPacketHandler.h"



GameUdpReceiver::GameUdpReceiver()
{
}

GameUdpReceiver::~GameUdpReceiver()
{
}

void GameUdpReceiver::OnRecv(network::SessionRef& session, BYTE* buffer, int32 len)
{
	ClientPacketHandler::HandlePacket<network::UdpPacketHeader>(session, buffer, len);
}
