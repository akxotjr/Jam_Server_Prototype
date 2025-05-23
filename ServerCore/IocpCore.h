#pragma once

namespace core::network
{
	/*------------------
		IocpObject
	-------------------*/

	class IocpObject : public enable_shared_from_this<IocpObject>
	{
	public:
		virtual HANDLE	GetHandle() = 0;
		virtual void	Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) = 0;
	};

	/*------------------
		  IocpCore
	-------------------*/

	class IocpCore
	{
	public:
		IocpCore();
		~IocpCore();

		HANDLE			GetHandle() { return _iocpHandle; }

		bool			Register(IocpObjectRef iocpObject);
		bool			Dispatch(uint32 timeoutMs = INFINITE);

	private:
		HANDLE			_iocpHandle;
	};
}