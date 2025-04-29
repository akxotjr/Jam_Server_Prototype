#include "pch.h"
#include "IdManager.h"

void IdManager::Init()
{
	for (auto& id : _idPools)
		id.store(1);
}
