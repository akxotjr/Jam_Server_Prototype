#pragma once

struct GameContext
{
	PlayerRef player = nullptr;
	RoomRef room = nullptr;
	GameTcpSessionRef tcpSession = nullptr;
	GameUdpSessionRef udpSession = nullptr;

	bool IsValid() const
	{
		return player && room && tcpSession && udpSession;
	}
};

class GameContextManager
{
	DECLARE_SINGLETON(GameContextManager)
	
public:
	PlayerRef				GetPlayerRef(uint32 userId);
	RoomRef					GetRoomRef(uint32 userId);
	GameTcpSessionRef		GetGameTcpSessionRef(uint32 userId);
	GameUdpSessionRef		GetGameUdpSessionRef(uint32 userId);

	template<typename... Args>
	void AddOrUpdate(uint32 userId, Args&&... args)
	{
		auto& ctx = _gameContexts[userId];
		(..., Set(ctx, std::forward<Args>(args)));
	}

	void RemoveGameContext(uint32 userId)
	{
		_gameContexts.erase(userId);
	}

	bool IsValid(uint32 id)
	{
		auto ctx = GetContext(id);
		return ctx && ctx->IsValid();
	}

private:
	GameContext*			GetContext(uint32 userId);

	template<typename T>
	void Set(GameContext& ctx, std::shared_ptr<T> value)
	{
		if constexpr (std::is_same_v<T, Player>)
			ctx.player = value;
		else if constexpr (std::is_same_v<T, Room>)
			ctx.room = value;
		else if constexpr (std::is_same_v<T, core::network::TcpSession>)
			ctx.tcpSession = value;
		else if constexpr (std::is_same_v<T, core::network::UdpSession>)
			ctx.udpSession = value;
		else
			static_assert(always_false<T>, "Unsupported type for GameContext");
	}

	template<typename>
	inline static constexpr bool always_false = false;

private:
	unordered_map<uint32, GameContext> _gameContexts;	// key - userId, value - context
};

