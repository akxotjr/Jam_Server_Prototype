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
public:
	void					Add(SessionRef session);
	void					Remove(SessionRef session);

	void					Broadcast(ProtocolType type, SendBufferRef sendBuffer, bool reliable);

	SessionRef				GetSessionById(ProtocolType type, uint32 id);

private:
	USE_LOCK

	unordered_map<uint64, SessionBundle> _sessionMap; // sessionId // todo: UID
};

extern SessionManager GSessionManager;