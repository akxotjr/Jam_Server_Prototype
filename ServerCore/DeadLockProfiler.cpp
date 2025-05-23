#include "pch.h"
#include "DeadLockProfiler.h"

namespace core::thread
{
	/*--------------------
		DeadLockProfiler
	---------------------*/

	void DeadLockProfiler::PushLock(const char* name)
	{
		LockGuard guard(_lock);

		// 아이디를 찾거나 발급한다.
		int32 lockId = 0;

		auto findIt = _nameToId.find(name);
		if (findIt == _nameToId.end())
		{
			lockId = static_cast<int32>(_nameToId.size());
			_nameToId[name] = lockId;
			_idToName[lockId] = name;
		}
		else
		{
			lockId = findIt->second;
		}

		// 잡고 있는 락이 있었다면
		if (LLockStack.empty() == false)
		{
			// 기존에 발견되지 않은 케이스라면 데드락 여부 다시 확인한다.
			const int32 prevId = LLockStack.top();
			if (lockId != prevId)
			{
				set<int32>& history = _lockHistory[prevId];
				if (history.find(lockId) == history.end())
				{
					history.insert(lockId);
					CheckCycle();
				}
			}
		}

		LLockStack.push(lockId);
	}

	void DeadLockProfiler::PopLock(const char* name)
	{
		LockGuard guard(_lock);

		if (LLockStack.empty())
			CRASH("MULTIPLE_UNLOCK");

		int32 lockId = _nameToId[name];
		if (LLockStack.top() != lockId)
			CRASH("INVALID_UNLOCK");

		LLockStack.pop();
	}

	void DeadLockProfiler::CheckCycle()
	{
		const int32 lockCount = static_cast<int32>(_nameToId.size());
		_discoveredOrder = Vector<int32>(lockCount, -1);
		_discoveredCount = 0;
		_finished = Vector<bool>(lockCount, false);
		_parent = vector<int32>(lockCount, -1);

		for (int32 lockId = 0; lockId < lockCount; lockId++)
			Dfs(lockId);

		_discoveredOrder.clear();
		_finished.clear();
		_parent.clear();
	}

	void DeadLockProfiler::Dfs(int32 here)
	{
		if (_discoveredOrder[here] != -1)
			return;

		_discoveredOrder[here] = _discoveredCount++;

		auto findIt = _lockHistory.find(here);
		if (findIt == _lockHistory.end())
		{
			_finished[here] = true;
			return;
		}

		set<int32>& nextSet = findIt->second;
		for (int32 there : nextSet)
		{
			if (_discoveredOrder[there] == -1)
			{
				_parent[there] = here;
				Dfs(there);
				continue;
			}

			if (_discoveredOrder[here] < _discoveredOrder[there])
				continue;

			// 순방향이 아니고, Dfs(there)가 아직 종료하지 않았다면, there는 here의 선조이다. (역방향 간선)
			if (_finished[there] == false)
			{
				printf("%s -> %s\n", _idToName[here], _idToName[there]);

				int32 now = here;
				while (true)
				{
					printf("%s -> %s\n", _idToName[_parent[now]], _idToName[now]);
					now = _parent[now];
					if (now == there)
						break;
				}

				CRASH("DEADLOCK_DETECTED")
			}
		}

		_finished[here] = true;
	}
}


