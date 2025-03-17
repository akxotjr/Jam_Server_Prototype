#pragma once

class SendBuffer;
class Session;
class SendBuffer;
class GameSession;

using GameSessionRef = shared_ptr<GameSession>;

class SessionManager
{
public:
	void Add(SessionRef session);
	void Remove(SessionRef session);

	void Broadcast(SessionType type, SendBufferRef sendBuffer);

	SessionRef GetSessionById(int32 sessionId);
	//SessionRef GetSessionByType();

private:
	USE_LOCK

	unordered_map<int32, GameSessionRef> _gameSessions;  // key - SessionId, value - SessionRef
	unordered_map<int32, SessionRef> _chatSessions;
};

extern SessionManager GSessionManager;