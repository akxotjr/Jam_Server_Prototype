#pragma once

#define WIN32_LEAN_AND_MEAN


#ifdef _DEBUG
#pragma comment(lib, "ServerCore\\Debug\\ServerCore.lib")
#pragma comment(lib, "PhysX5\\Release\\PhysX_64.lib")			// current PhysX is release version
#pragma comment(lib, "PhysX5\\Release\\PhysXCommon_64.lib")
#pragma comment(lib, "PhysX5\\Release\\PhysXFoundation_64.lib")
#else
#pragma comment(lib, "ServerCore\\Release\\ServerCore.lib")
#pragma comment(lib, "PhysX5\\Release\\PhysX_64.lib")
#pragma comment(lib, "PhysX5\\Release\\PhysXCommon_64.lib")
#pragma comment(lib, "PhysX5\\Release\\PhysXFoundation_64.lib")
#endif

#include "CorePch.h"
#include "Types.h"
#include "Enum.pb.h"
#include "Protocol.pb.h"
#include <PxPhysicsAPI.h>

using namespace core;
using namespace core::memory;