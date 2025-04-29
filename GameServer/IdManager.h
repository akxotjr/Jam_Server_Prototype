#pragma once

enum class IdType
{
	Session,
	Room,
	Actor,

	MAX
};

enum class ActorTypePrefix : uint8_t
{
	None = 0x0,
	Player = 0x1,
	Monster = 0x2,
	Bot = 0x3,
	Projectile = 0x4,
};


class IdManager
{
	DECLARE_SINGLETON(IdManager)

public:
	void	Init();

	uint32	Generate(IdType type)
	{
		return _idPools[static_cast<uint32>(type)]++;
	}

	uint32_t GenerateActorId(ActorTypePrefix type, uint32_t instanceId)
	{
		return (static_cast<uint32_t>(type) << 28) | (instanceId & 0x0FFFFFFF);
	}

	ActorTypePrefix GetActorType(uint32_t actorId)
	{
		return static_cast<ActorTypePrefix>(actorId >> 28);
	}

	uint32_t GetActorInstanceId(uint32_t actorId)
	{
		return actorId & 0x0FFFFFFF;
	}

private:
	Array<Atomic<uint32>, static_cast<uint32>(IdType::MAX)> _idPools;
};