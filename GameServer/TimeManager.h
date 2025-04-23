#pragma once

class TimeManager
{

public:
	void		Init();
	void		Update();

	double		GetServerTime() { return _serverTime; }
	double		GetDeltaTime() { return _deltaTime; }

private:
	uint64		_frequency = 0;
	uint64		_prevCount = 0;
	double		_deltaTime = 0.f;
	
	double		_serverTime = 0.f;
	double		_sumTime = 0.f;
};

extern TimeManager GTimeManager;