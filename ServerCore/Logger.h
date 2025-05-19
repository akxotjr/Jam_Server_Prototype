#pragma once

namespace core
{
	class Logger
	{
		using spdlogRef = std::shared_ptr<spdlog::logger>;

		DECLARE_SINGLETON(Logger)

	public:
		void			Init();
		spdlogRef&		GetLogger() { return _logger; }

	private:
		spdlogRef _logger;
	};
}

