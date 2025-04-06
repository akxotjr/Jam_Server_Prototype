#pragma once

#include "IocpCore.h"
#include "IocpEvent.h"
#include "NetAddress.h"
#include "RecvBuffer.h"

class Service;

struct PacketHeader
{
	uint16 size;
	uint16 id;	// Protocol ID
};

class Session : public IocpObject
{
public:
	Session() = default;
	virtual ~Session();

	virtual bool			Connect() = 0;
	virtual void			Disconnect(const WCHAR* cause) = 0;
	virtual void			Send(SendBufferRef sendBuffer) = 0;

	ServiceRef				GetService() { return _service.lock(); }
	void					SetService(ServiceRef service) { _service = service; }

	NetAddress				GetNetAddress() { return _netAddress; }
	void					SetNetAddress(NetAddress address) { _netAddress = address; }
	SOCKET					GetSocket() { return _socket; }
	bool					IsConnected() { return _connected; }
	SessionRef				GetSessionRef() { return static_pointer_cast<Session>(shared_from_this()); }
	int32					GetId() { return _id; }
	void					SetId(int32 id) { _id = id; }

protected:
	virtual void			OnConnected() {}
	virtual void			OnDisconnected() {}
	virtual void			OnSend(int32 len) {}
	virtual int32			OnRecv(BYTE* buffer, int32 len) { return len; }

protected:
	SOCKET					_socket = INVALID_SOCKET;
	Atomic<bool>			_connected = false;
private:
	weak_ptr<Service>		_service;
	NetAddress				_netAddress = {};
	uint32					_id = 0;
};

class TcpSession : public Session
{
	enum { BUFFER_SIZE = 0x10000 }; // 64KB

public:
	TcpSession();
	virtual ~TcpSession();

public:
	virtual bool			Connect() override;
	virtual void			Disconnect(const WCHAR* cause) override;
	virtual void			Send(SendBufferRef sendBuffer) override;

private:
	/* Iocp Object */
	virtual HANDLE			GetHandle() override;
	virtual void			Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) override;

private:
	// 전송 관련
	bool					RegisterConnect();
	bool					RegisterDisconnect();
	void					RegisterRecv();
	void					RegisterSend();

	void					ProcessConnect();
	void					ProcessDisconnect();
	void					ProcessRecv(int32 numOfBytes);
	void					ProcessSend(int32 numOfBytes);

	int32					IsParsingPacket(BYTE* buffer, int32 len);

	void					HandleError(int32 errorCode);

private:
	USE_LOCK

	RecvBuffer				_recvBuffer;
	Queue<SendBufferRef>	_sendQueue;
	Atomic<bool>			_sendRegistered = false;

private:
	ConnectEvent			_connectEvent;
	DisconnectEvent			_disconnectEvent;
	RecvEvent				_recvEvent;
	SendEvent				_sendEvent;
};


/*--------------------------
	 ReliableUdpSession
---------------------------*/

class ReliableUdpSession : public Session
{
	enum { BUFFER_SIZE = 0x10000 }; // 64KB

public:
	ReliableUdpSession();
	virtual ~ReliableUdpSession();

public:
	virtual bool			Connect() override;
	virtual void			Disconnect(const WCHAR* cause) override;
	virtual void			Send(SendBufferRef sendBuffer) override;

private:
	/* Iocp Object */
	virtual HANDLE			GetHandle() override;
	virtual void			Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) override;

private:
	void					RegisterSend();
	void					RegisterRecv();

	void					ProcessConnect();
	void					ProcessDisconnect();
	void					ProcessSend(int32 numOfBytes);
	void					ProcessRecv(int32 numOfBytes);

	void					Update();

	void					HandleError(int32 errorCode);

private:
	USE_LOCK

	RecvBuffer				_recvBuffer;
	Queue<SendBufferRef>	_sendQueue;
	Atomic<bool>			_sendRegistered = false;

	NetAddress				_fromAddr;

private:
	RecvEvent				_recvEvent;
	SendEvent				_sendEvent;
};



//class Service;
//
//class Session : public IocpObject
//{
//	friend class Listener;
//	friend class IocpCore;
//	friend class Service;
//
//	enum { BUFFER_SIZE = 0x10000 }; // 64KB
//
//public:
//	Session();
//	virtual ~Session();
//
//public:
//	void					Send(SendBufferRef sendBuffer);
//	bool					Conenect();
//	void					Disconnect(const WCHAR* cause);
//
//
//	shared_ptr<Service>		GetService() { return _service.lock(); }
//	void					SetService(shared_ptr<Service> service) { _service = service; }
//
//public:
//	// 정보 관련
//	void					SetNetAddress( NetAddress address) { _netAddress = address; }
//	NetAddress				GetAddress() { return _netAddress; }
//	SOCKET					GetSocket() { return _socket; }
//	bool					IsConnected() { return _connected; }
//	SessionRef				GetSessionRef() { return static_pointer_cast<Session>(shared_from_this()); }
//	void					SetId(int32 id) { _id = id; }
//	int32					GetId() { return _id; }
//
//private:
//	// 인터페이스 관련
//	virtual HANDLE	GetHandle() override;
//	virtual void	Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) override;
//
//private:
//	// 전송 관련
//	bool			RegisterConnect();
//	bool			RegisterDisconnect();
//	void			RegisterRecv();
//	void			RegisterSend();
//
//	void			ProcessConnect();
//	void			ProcessDisconnect();
//	void			ProcessRecv(int32 numOfBytes);
//	void			ProcessSend(int32 numOfBytes);
//
//	void			HandleError(int32 errorCode);
//
//protected:
//	// 컨텐츠 코드에서 오버로딩
//	virtual void	OnConnected() {}
//	virtual int32	OnRecv(BYTE* buffer, int32 len) { return len; }
//	virtual void	OnSend(int32 len) {}
//	virtual void	OnDisconnected() {}
//
//
//private:
//	weak_ptr<Service>	_service;
//	SOCKET				_socket = INVALID_SOCKET;
//	NetAddress			_netAddress = {};
//	Atomic<bool>		_connected = false;
//	int32				_id = -1;
//
//private:
//	USE_LOCK
//
//	// 수신 관련
//	RecvBuffer			_recvBuffer;
//	// 송신 관련
//	Queue<SendBufferRef> _sendQueue;
//	Atomic<bool>	     _sendRegistered = false;
//
//
//private:
//	ConnectEvent		_connectEvent;
//	DisconnectEvent		_disconnectEvent;
//	RecvEvent			_recvEvent;
//	SendEvent			_sendEvent;
//};
//
///*------------------
//	PacketSession
//-------------------*/
//
//struct PacketHeader
//{
//	uint16 size;
//	uint16 id;	// Protocol ID
//};
//
//class PacketSession : public Session
//{
//public:
//	PacketSession();
//	virtual ~PacketSession();
//
//	PacketSessionRef	GetPacketSessionRef() { return static_pointer_cast<PacketSession>(shared_from_this()); }
//
//protected:
//	virtual int32		OnRecv(BYTE* buffer, int32 len) sealed;
//	virtual void		OnRecvPacket(BYTE* buffer, int32 len) abstract;
//};