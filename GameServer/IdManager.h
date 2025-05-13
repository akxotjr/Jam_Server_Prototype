#pragma once
#include "Actor.h"

enum class IdType
{
	Session,
	Room,
	Actor,

	MAX
};

enum class ActorTypePrefix : uint8
{
	None = 0x0,
	Player = 0x1,
	Monster = 0x2,
	Floor = 0x3,	// temp
	Projectile = 0x4,

	COUNT
};


class IdManager
{
	DECLARE_SINGLETON(IdManager)

public:
	void				Init();

	uint32				Generate(IdType type, std::optional<ActorTypePrefix> actorType = ActorTypePrefix::None);

	ActorTypePrefix		GetActorType(uint32 actorId);
	uint32				GetActorInstanceId(uint32 actorId);

private:
	Array<Atomic<uint32>, static_cast<uint32>(IdType::MAX)>				_idPools;
	Array<Atomic<uint32>, static_cast<uint32>(ActorTypePrefix::COUNT)>	_actorIdPools;
};