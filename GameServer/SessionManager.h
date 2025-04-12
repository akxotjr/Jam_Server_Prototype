#pragma once

class Service;
class ServerService;
class SendBuffer;
class Session;
class SendBuffer;
class GameSession;



class SessionManager
{
public:
	void Init();

	void Add(SessionRef session);
	void Remove(SessionRef session);

	void Broadcast(SessionType type, SendBufferRef sendBuffer);

	SessionRef GetSessionById(int32 sessionId);
	//SessionRef GetSessionByType();

private:
	USE_LOCK

	weak_ptr<ServerService> _serverService;

	unordered_map<int32, GameTcpSessionRef> _gameSessions;  // key - SessionId, value - SessionRef
	unordered_map<int32, SessionRef> _chatSessions;
};

extern SessionManager GSessionManager;