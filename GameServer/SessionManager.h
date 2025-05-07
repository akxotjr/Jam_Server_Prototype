#pragma once

class Service;
class SendBuffer;

struct SessionBundle
{
	GameTcpSessionRef tcpSession = nullptr;
	GameUdpSessionRef udpSession = nullptr;

	bool IsPaired() const
	{
		return tcpSession != nullptr && udpSession != nullptr;
	}
};


class SessionManager
{
	DECLARE_SINGLETON(SessionManager)

public:
	void							Add(network::SessionRef session);
	void							Remove(network::SessionRef session);

	void							Multicast(ProtocolType type, network::SendBufferRef sendBuffer, bool reliable = false);

	network::SessionRef				GetSessionByUserId(ProtocolType type, uint32 userId);

private:
	USE_LOCK

	unordered_map<uint32, SessionBundle> _sessionMap; // key - user id
};
