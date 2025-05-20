#pragma once
#include <stack>
#include <map>
#include <vector>

namespace core::thread
{
	/*--------------------
		DeadLockProfiler
	---------------------*/

	class DeadLockProfiler
	{
		DECLARE_SINGLETON(DeadLockProfiler)

	public:
		void PushLock(const char* name);
		void PopLock(const char* name);
		void CheckCycle();

	private:
		void Dfs(int32 index);

	private:
		unordered_map<const char*, int32>	_nameToId;
		unordered_map<int32, const char*>	_idToName;
		Map<int32, set<int32>>				_lockHistory;

		Mutex			_lock;

	private:
		Vector<int32>	_discoveredOrder;
		int32			_discoveredCount = 0;
		Vector<bool>	_finished;
		vector<int32>	_parent;
	};
}

