#include "pch.h"
#include "TimeManager.h"

namespace core
{
	void TimeManager::Init()
	{
		::QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&_frequency));
		::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&_prevCount));
	}

	void TimeManager::Update()
	{
		uint64 currentCount;
		::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentCount));

		_deltaTime = (currentCount - _prevCount) / static_cast<double>(_frequency);
		_serverTime += _deltaTime;
		_prevCount = currentCount;
	}
}
