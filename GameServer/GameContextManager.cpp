#include "pch.h"
#include "GameContextManager.h"

PlayerRef GameContextManager::GetPlayerRef(uint32 userId)
{
    auto ctx = GetContext(userId);
    return ctx ? ctx->player : nullptr;
}

RoomRef GameContextManager::GetRoomRef(uint32 userId)
{
    auto ctx = GetContext(userId);
    return ctx ? ctx->room : nullptr;
}

GameTcpSessionRef GameContextManager::GetGameTcpSessionRef(uint32 userId)
{
    auto ctx = GetContext(userId);
    return ctx ? ctx->tcpSession : nullptr;
}

GameUdpSessionRef GameContextManager::GetGameUdpSessionRef(uint32 userId)
{
    auto ctx = GetContext(userId);
    return ctx ? ctx->udpSession : nullptr;
}

GameContext* GameContextManager::GetContext(uint32 userId)
{
    auto it = _gameContexts.find(userId);
    return (it != _gameContexts.end()) ? &it->second : nullptr;
}
