#include "pch.h"
#include "GameUdpReceiver.h"
#include "ClientPacketHandler.h"



GameUdpReceiver::GameUdpReceiver()
{
}

GameUdpReceiver::~GameUdpReceiver()
{
}

void GameUdpReceiver::OnRecv(SessionRef& session, BYTE* buffer, int32 len)
{
	ClientPacketHandler::HandlePacket<UdpPacketHeader>(session, buffer, len);
}
