#include "pch.h"
#include "TimeManager.h"
#include "ClientPacketHandler.h"

TimeManager GTimeManager;

void TimeManager::Init()
{
	::QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&_frequency));
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(_prevCount));
}

void TimeManager::Update()
{
	uint64 currentCount;
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentCount));

	_deltaTime = (currentCount - _prevCount) / static_cast<float>(_frequency);

	_serverTime += _deltaTime;
	//_sumTime += _deltaTime;
	//
	//if (_sumTime >= TIME_SYNK)
	//{
	//	
	//}


	_prevCount = currentCount;
}
