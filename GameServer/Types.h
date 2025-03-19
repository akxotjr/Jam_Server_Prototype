#pragma once

enum class SessionType
{
	NONE,
	GAME_SESSION,
	CHAT_SESSION,

	COUNT
};

#define USING_SHARED_PTR(name) using name##Ref = std::shared_ptr<class name>;

USING_SHARED_PTR(GameSession)
USING_SHARED_PTR(Room)
USING_SHARED_PTR(Player)
