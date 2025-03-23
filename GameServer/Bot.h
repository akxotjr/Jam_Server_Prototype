#pragma once
#include "Character.h"

class Bot : public Character
{
	using Super = Character;

public:
	Bot(GameSessionRef gameSession);
	~Bot();

	virtual void Init();
	virtual void Update();

	virtual void UpdateMovement() override;

	void SetBotProto(Protocol::BotProto botProto) { _botProto = botProto; }
	Protocol::BotProto& GetBotProto() { return _botProto; }


private:
	Protocol::BotProto _botProto;
};

