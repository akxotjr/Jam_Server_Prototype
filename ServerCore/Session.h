#pragma once

#include "IocpCore.h"
#include "IocpEvent.h"
#include "NetAddress.h"
#include "RecvBuffer.h"

class Service;

/*----------------------------
	   Session Interface
-----------------------------*/

class SessionInterface
{
public:
	virtual ~SessionInterface() {}

	virtual void	Send(SendBufferRef sendBuffer) = 0;
	virtual bool	Connect() = 0;
	virtual void	Disconnect(const WCHAR* cause) = 0;

	virtual void	OnConnected() = 0;
	virtual int32	OnRecv(BYTE* buffer, int32 len) = 0;
	virtual void	OnSend(int32 len) = 0;
	virtual void	OnDisconnected() = 0;
};

class TCPSession : public SessionInterface, public IocpObject
{
	friend class Listener;
	friend class IocpCore;
	friend class Service;

	enum { BUFFER_SIZE = 0x10000 }; // 64KB

public:
	TCPSession();
	virtual ~TCPSession();

	/* Session Interface */
	virtual void			Send(SendBufferRef sendBuffer) override;
	virtual bool			Connect() override;
	virtual void			Disconnect(const WCHAR* cause) override;

	/* Getter & Setter */
	//ServiceRef				GetService() { return _service.lock(); }
	//void					SetService(ServiceRef service) { _service = service; }
	void					SetNetAddress(NetAddress address) { _netAddress = address; }
	NetAddress				GetAddress() { return _netAddress; }
	SOCKET					GetSocket() { return _socket; }
	bool					IsConnected() { return _connected; }
	SessionRef				GetSessionRef() { return static_pointer_cast<Session>(shared_from_this()); }
	void					SetId(int32 id) { _id = id; }
	int32					GetId() { return _id; }

protected:
	/* Session Interface */
	virtual void			OnConnected() override;
	virtual int32			OnRecv(BYTE* buffer, int32 len) override;
	virtual void			OnSend(int32 len) override;
	virtual void			OnDisconnected() override;

private:
	/* IocpObject Interface */
	virtual HANDLE			GetHandle() override;
	virtual void			Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) override;

private:
	bool					RegisterConnect();
	bool					RegisterDisconnect();
	void					RegisterRecv();
	void					RegisterSend();

	void					ProcessConnect();
	void					ProcessDisconnect();
	void					ProcessRecv(int32 numOfBytes);
	void					ProcessSend(int32 numOfBytes);

	void					HandleError(int32 errorCode);

private:
	USE_LOCK

	weak_ptr<Service>		_service;
	SOCKET					_socket = INVALID_SOCKET;
	NetAddress				_netAddress = {};
	Atomic<bool>			_connected = false;
	int32					_id = 0;

	// 수신 관련
	RecvBuffer				_recvBuffer;
	// 송신 관련
	Queue<SendBufferRef>	_sendQueue;
	Atomic<bool>			_sendRegistered = false;

	ConnectEvent			_connectEvent;
	DisconnectEvent			_disconnectEvent;
	RecvEvent				_recvEvent;
	SendEvent				_sendEvent;
};

class ReliableUDPSession : public SessionInterface
{
public:
	ReliableUDPSession();
	virtual ~ReliableUDPSession();

	/* Session Interface */
	virtual void			Send(SendBufferRef sendBuffer) override;
	virtual bool			Connect() override;
	virtual void			Disconnect(const WCHAR* cause) override;

protected:
	/* Session Interface */
	virtual void			OnConnected() override;
	virtual int32			OnRecv(BYTE* buffer, int32 len) override;
	virtual void			OnSend(int32 len) override;
	virtual void			OnDisconnected() override;

private:
	USE_LOCK

	weak_ptr<Service>		_service;
	SOCKET					_socket = INVALID_SOCKET;
	NetAddress				_netAddress = {};
	Atomic<bool>			_connected = false;
	int32					_id = 0;
};

class Session : public IocpObject
{
	friend class Listener;
	friend class IocpCore;
	friend class Service;

	enum { BUFFER_SIZE = 0x10000 }; // 64KB

public:
	Session();
	virtual ~Session();

public:
	void					Send(SendBufferRef sendBuffer);
	bool					Conenect();
	void					Disconnect(const WCHAR* cause);


	shared_ptr<Service>		GetService() { return _service.lock(); }
	void					SetService(shared_ptr<Service> service) { _service = service; }

public:
	// 정보 관련
	void					SetNetAddress( NetAddress address) { _netAddress = address; }
	NetAddress				GetAddress() { return _netAddress; }
	SOCKET					GetSocket() { return _socket; }
	bool					IsConnected() { return _connected; }
	SessionRef				GetSessionRef() { return static_pointer_cast<Session>(shared_from_this()); }
	void					SetId(int32 id) { _id = id; }
	int32					GetId() { return _id; }

private:
	// 인터페이스 관련
	virtual HANDLE	GetHandle() override;
	virtual void	Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) override;

private:
	// 전송 관련
	bool			RegisterConnect();
	bool			RegisterDisconnect();
	void			RegisterRecv();
	void			RegisterSend();

	void			ProcessConnect();
	void			ProcessDisconnect();
	void			ProcessRecv(int32 numOfBytes);
	void			ProcessSend(int32 numOfBytes);

	void			HandleError(int32 errorCode);

protected:
	// 컨텐츠 코드에서 오버로딩
	virtual void	OnConnected() {}
	virtual int32	OnRecv(BYTE* buffer, int32 len) { return len; }
	virtual void	OnSend(int32 len) {}
	virtual void	OnDisconnected() {}


private:
	weak_ptr<Service>	_service;
	SOCKET				_socket = INVALID_SOCKET;
	NetAddress			_netAddress = {};
	Atomic<bool>		_connected = false;
	int32				_id = -1;

private:
	USE_LOCK

	// 수신 관련
	RecvBuffer			_recvBuffer;
	// 송신 관련
	Queue<SendBufferRef> _sendQueue;
	Atomic<bool>	     _sendRegistered = false;


private:
	ConnectEvent		_connectEvent;
	DisconnectEvent		_disconnectEvent;
	RecvEvent			_recvEvent;
	SendEvent			_sendEvent;
};

/*------------------
	PacketSession
-------------------*/

struct PacketHeader
{
	uint16 size;
	uint16 id;	// Protocol ID
};

class PacketSession : public Session
{
public:
	PacketSession();
	virtual ~PacketSession();

	PacketSessionRef	GetPacketSessionRef() { return static_pointer_cast<PacketSession>(shared_from_this()); }

protected:
	virtual int32		OnRecv(BYTE* buffer, int32 len) sealed;
	virtual void		OnRecvPacket(BYTE* buffer, int32 len) abstract;
};