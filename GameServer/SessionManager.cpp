#include "pch.h"
#include "SessionManager.h"
#include "GameSession.h"

SessionManager GSessionManager;

void SessionManager::Add(SessionRef session)
{
    int32 id = session->GetId();
    auto type = static_cast<SessionType>(id / 1000);

    {
        WRITE_LOCK
        switch (type)
        {
        case SessionType::GAME_SESSION:
            _gameSessions.insert({ id, static_pointer_cast<GameSession>(session) });
            break;
        case SessionType::CHAT_SESSION:
            _chatSessions.insert({ id, session });
            break;
        default:
            break;
        }
    }
}

void SessionManager::Remove(SessionRef session)
{
    int32 id = session->GetId();
    auto type = static_cast<SessionType>(id / 1000);

    {
        WRITE_LOCK
        switch (type)
        {
        case SessionType::GAME_SESSION:
            _gameSessions.erase(id);
            break;
        case SessionType::CHAT_SESSION:
            _chatSessions.erase(id);
            break;
        default:
            break;
        }
    }
}

void SessionManager::Broadcast(SessionType type, SendBufferRef sendBuffer)
{
    WRITE_LOCK

    switch (type)
    {
    case SessionType::GAME_SESSION:
        for (auto& [id, session] : _gameSessions)
        {
            session->Send(sendBuffer);
        }
        break;
    case SessionType::CHAT_SESSION:
        for (auto& [id, session] : _chatSessions)
        {
            session->Send(sendBuffer);
        }
        break;
    default:
        break;
    }
}

SessionRef SessionManager::GetSessionById(int32 sessionId)
{
    SessionType type = static_cast<SessionType>(sessionId / 1000);

    WRITE_LOCK
    switch (type)
    {
    case SessionType::GAME_SESSION:
        for (auto& [id, session] : _gameSessions)
        {
            if (id == sessionId)
            {
                return session;
            }
        }
        break;
    case SessionType::CHAT_SESSION:
        for (auto& [id, session] : _chatSessions)
        {
            if (id == sessionId)
            {
                return session;
            }
        }
        break;
    default:
        break;
    }

    return nullptr;
}