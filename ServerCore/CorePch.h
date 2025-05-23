#pragma once

#include <windows.h>
#include <winnt.h>         // �� SLIST_ENTRY, SLIST_HEADER ��
#include <intrin.h>        // �� InterlockedPushEntrySList ��

#include "CoreTypes.h"
#include "CoreEnums.h"
#include "CoreValues.h"
#include "CoreMacro.h"
#include "CoreTLS.h"
#include "CoreGlobal.h"
#include "Container.h"

#include <iostream>
using namespace std;

#include <winsock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>
#include "Logger.h"

#include "Memory.h"
#include "Lock.h"
#include "ObjectPool.h"
#include "TypeCast.h"
#include "SendBuffer.h"
#include "Session.h"
#include "LockQueue.h"
