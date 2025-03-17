#pragma once

#define WIN32_LEAN_AND_MEAN


#ifdef _DEBUG
#pragma comment(lib, "ServerCore\\Debug\\ServerCore.lib")
#else
#pragma comment(lib, "ServerCore\\Release\\ServerCore.lib")
#endif

#include "CorePch.h"
#include "Types.h"
#include "Enum.pb.h"
#include "Protocol.pb.h"

//using GameSessionRef = shared_ptr<class GameSession>;
//using PlayerRef = shared_ptr<class Player>;