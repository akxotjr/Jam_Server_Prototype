#include "pch.h"
#include "IdManager.h"

void IdManager::Init()
{
	for (auto& id : _idPools)
		id.store(1);
}

uint32 IdManager::Generate(IdType type, std::optional<ActorTypePrefix> actorType)
{
	if (type != IdType::Actor)
		return _idPools[static_cast<uint32>(type)]++;

	if (actorType.has_value())
	{
		uint32 counter = _actorIdPools[static_cast<uint8>(actorType.value())].fetch_add(1);
		return (static_cast<uint32>(actorType.value()) << 28) | (counter & 0x0FFFFFFF);
	}

	return 0;
}

ActorTypePrefix IdManager::GetActorType(uint32 actorId)
{
	return static_cast<ActorTypePrefix>(actorId >> 28);
}

uint32 IdManager::GetActorInstanceId(uint32 actorId)
{
	return actorId & 0x0FFFFFFF;
}
