#pragma once

class Game
{
public:
	Game();
	~Game();

	void							Init();
	void							Loop();

private:
	void							MainThreadLoop();
	void							NetworkThreadLoop();
	void							DoNetworkJob();

	void							PhysicsThreadLoop();
	void							DoPhysicsJob();

private:
	network::ServiceRef				_service = nullptr;
};

