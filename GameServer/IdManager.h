#pragma once

enum class IdType
{
	Session,
	Room,
	Actor,

	MAX
};

class IdManager
{
public:
	IdManager()
	{
		for (auto& id : _idPools)
			id.store(1);
	}

	uint32 Generate(IdType type)
	{
		return _idPools[static_cast<uint32>(type)]++;
	}

private:
	Array<Atomic<uint32>, static_cast<uint32>(IdType::MAX)> _idPools;
};

extern IdManager GIdManager;
