#pragma once

#define OUT

/*---------------
	  Lock
---------------*/

#define USE_MANY_LOCKS(count)	core::thread::Lock _locks[count];
#define USE_LOCK				USE_MANY_LOCKS(1)
#define	READ_LOCK_IDX(idx)		core::thread::ReadLockGuard readLockGuard_##idx(_locks[idx], typeid(this).name());
#define READ_LOCK				READ_LOCK_IDX(0)
#define	WRITE_LOCK_IDX(idx)		core::thread::WriteLockGuard writeLockGuard_##idx(_locks[idx], typeid(this).name());
#define WRITE_LOCK				WRITE_LOCK_IDX(0)

/*---------------
	  Crash
---------------*/

#define CRASH(cause)						\
{											\
	uint32* crash = nullptr;				\
	__analysis_assume(crash != nullptr);	\
	*crash = 0xDEADBEEF;					\
}

#define ASSERT_CRASH(expr)			\
{									\
	if (!(expr))					\
	{								\
		CRASH("ASSERT_CRASH");		\
		__analysis_assume(expr);	\
	}								\
}

/*---------------
	Singleton
---------------*/

#define DECLARE_SINGLETON(ClassName)                \
public:                                             \
    static ClassName& Instance() {                  \
        static ClassName instance;                  \
        return instance;                            \
    }                                               \
private:                                            \
    ClassName() = default;                          \
    ~ClassName() = default;                         \
    ClassName(const ClassName&) = delete;           \
    ClassName& operator=(const ClassName&) = delete;


/*---------------
	  Logger
---------------*/
#define LOG_TRACE(...) Logger::Instance().GetLogger()->trace(__VA_ARGS__)
#define LOG_DEBUG(...) Logger::Instance().GetLogger()->debug(__VA_ARGS__)
#define LOG_INFO(...)  Logger::Instance().GetLogger()->info(__VA_ARGS__)
#define LOG_WARN(...)  Logger::Instance().GetLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...) Logger::Instance().GetLogger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...) Logger::Instance().GetLogger()->critical(__VA_ARGS__)
#define LOG_OFF(...) Logger::Instance().GetLogger()->off(__VA_ARGS__)

