#pragma once

#include <optional>

namespace core::thread
{
	template<typename T>
	class LockQueue
	{
	public:
		void Push(T job)
		{
			WRITE_LOCK
			_items.push(job);
		}

		std::optional<T> TryPop()
		{
			WRITE_LOCK
				if (_items.empty())
					return std::nullopt;

			T job = std::move(_items.front());
			_items.pop();
			return job;
		}

		T Pop()
		{
			WRITE_LOCK
			if (_items.empty())
				return T();

			T ret = _items.front();
			_items.pop();
			return ret;
		}

		void PopAll(OUT Vector<T>& items)
		{
			WRITE_LOCK
			while (T item = Pop())
			{
				items.push_back(item);
			}
		}

		void Clear()
		{
			WRITE_LOCK
			_items = Queue<T>();
		}

	private:
		USE_LOCK
		Queue<T> _items;
	};
}