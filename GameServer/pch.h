#pragma once

#define WIN32_LEAN_AND_MEAN


#ifdef _DEBUG
#pragma comment(lib, "ServerCore\\Debug\\ServerCore.lib")

#else
#pragma comment(lib, "ServerCore\\Release\\ServerCore.lib")

#endif

#include "CorePch.h"
#include "Types.h"
#include "Values.h"
#include "Enum.pb.h"
#include "Protocol.pb.h"
#include <physx/PxPhysicsAPI.h>

using namespace core;
using namespace core::memory;