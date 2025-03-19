#pragma once

constexpr auto TIME_SYNK = 5.f;

class TimeManager
{

public:
	void Init();
	void Update();

	float GetServerTime() { return _serverTime; }

private:
	uint64 _frequency = 0;
	uint64 _prevCount = 0;
	float _deltaTime = 0.f;
	
	float _serverTime = 0.f;
	float _sumTime = 0.f;
};

extern TimeManager GTimeManager;