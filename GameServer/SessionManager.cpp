#include "pch.h"
#include "SessionManager.h"
#include "GameTcpSession.h"
#include "GameUdpSession.h"
#include "TimeManager.h"

SessionManager GSessionManager;

void SessionManager::Add(network::SessionRef session)
{
    uint32 id = session->GetId();
    SessionBundle& bundle = _sessionMap[id];

    WRITE_LOCK

    if (session->IsTcp())
    {
        bundle.tcpSession = static_pointer_cast<GameTcpSession>(session);
    }
    else if (session->IsUdp())
    {
        bundle.udpSession = static_pointer_cast<GameUdpSession>(session);
    }
}

void SessionManager::Remove(network::SessionRef session)
{
    int32 id = session->GetId();
    SessionBundle& bundle = _sessionMap[id];
    
    WRITE_LOCK

    if (session->IsTcp())
    {
        bundle.tcpSession = nullptr;
    }
    else if (session->IsUdp())
    {
        bundle.udpSession = nullptr;
    }

    if (!bundle.tcpSession && !bundle.udpSession)
    {
        _sessionMap.erase(id);
    }
}

void SessionManager::Broadcast(ProtocolType type, network::SendBufferRef sendBuffer, bool reliable)
{
    WRITE_LOCK

    for (auto& [id, bundle] : _sessionMap)
    {
        if (type == ProtocolType::PROTOCOL_TCP)
        {
            bundle.tcpSession->Send(sendBuffer);
        }
        else if (type == ProtocolType::PROTOCOL_UDP)
        {
            if (reliable)
            {
                float timestmap = TimeManager::Instance().GetServerTime();
                bundle.udpSession->SendReliable(sendBuffer, timestmap);
            }
            else
            {
                bundle.udpSession->Send(sendBuffer);
            }
        }
    }
}

network::SessionRef SessionManager::GetSessionById(ProtocolType type, uint32 id)
{
    WRITE_LOCK
    

    if (type == ProtocolType::PROTOCOL_TCP)
    {
        return _sessionMap[id].tcpSession;
    }
    else if (type == ProtocolType::PROTOCOL_UDP)
    {
        return _sessionMap[id].udpSession;
    }
}