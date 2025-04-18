#pragma once

#include <bitset>

#include "IocpCore.h"
#include "IocpEvent.h"
#include "NetAddress.h"
#include "RecvBuffer.h"

class Service;
class UdpReceiver;

struct TcpPacketHeader
{
	uint16 size;
	uint16 id;
};

class Session : public IocpObject
{
public:
	Session() = default;
	virtual ~Session() = default;

	virtual bool							Connect() = 0;
	virtual void							Disconnect(const WCHAR* cause) = 0;
	virtual void							Send(SendBufferRef sendBuffer) = 0;
	virtual bool							IsTcp() const = 0;
	virtual bool							IsUdp() const = 0;

	ServiceRef								GetService() { return _service.lock(); }
	void									SetService(ServiceRef service) { _service = service; }

	NetAddress&								GetRemoteNetAddress() { return _remoteAddress; }
	void									SetRemoteNetAddress(NetAddress address) { _remoteAddress = address; }
	SOCKET									GetSocket() { return _socket; }	//todo
	bool									IsConnected() { return _connected; }
	SessionRef								GetSessionRef() { return static_pointer_cast<Session>(shared_from_this()); }
	int32									GetId() { return _id; }
	void									SetId(int32 id) { _id = id; }

protected:
	virtual void							OnConnected() {}
	virtual void							OnDisconnected() {}
	virtual void							OnRecv(BYTE* buffer, int32 len) {}
	virtual void							OnSend(int32 len) {}


protected:
	SOCKET									_socket = INVALID_SOCKET;
	Atomic<bool>							_connected = false;

private:
	weak_ptr<Service>						_service;
	NetAddress								_remoteAddress = {};
	uint32									_id = 0;
};

class TcpSession : public Session
{
	enum { BUFFER_SIZE = 0x10000 }; // 64KB

	friend class Listener;
	friend class IocpCore;
	friend class Service;

public:
	TcpSession();
	virtual ~TcpSession();

public:
	virtual bool							Connect() override;
	virtual void							Disconnect(const WCHAR* cause) override;
	virtual void							Send(SendBufferRef sendBuffer) override;
	virtual bool							IsTcp() const override { return true; }
	virtual bool							IsUdp() const override { return false; }

private:
	/* Iocp Object impl */
	virtual HANDLE							GetHandle() override;
	virtual void							Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) override;

private:
	// 전송 관련
	bool									RegisterConnect();
	bool									RegisterDisconnect();
	void									RegisterRecv();
	void									RegisterSend();

	void									ProcessConnect();
	void									ProcessDisconnect();
	void									ProcessRecv(int32 numOfBytes);
	void									ProcessSend(int32 numOfBytes);

	int32									IsParsingPacket(BYTE* buffer, int32 len);

	void									HandleError(int32 errorCode);

private:
	USE_LOCK

	RecvBuffer								_recvBuffer;
	Queue<SendBufferRef>					_sendQueue;
	Atomic<bool>							_sendRegistered = false;

private:
	ConnectEvent							_connectEvent;
	DisconnectEvent							_disconnectEvent;
	RecvEvent								_recvEvent;
	SendEvent								_sendEvent;
};


/*--------------------------
	 ReliableUdpSession
---------------------------*/

struct UdpPacketHeader
{
	uint16 size;
	uint16 id;
	uint16 sequence = 0;
};

struct PendingPacket
{
	SendBufferRef buffer;
	uint16 sequence;
	float timestamp;
	uint32 retryCount = 0;
};

class ReliableUdpSession : public Session
{
	enum { BUFFER_SIZE = 0x10000 }; // 64KB

	friend class UdpReceiver;
	friend class IocpCore;
	friend class Service;

public:
	ReliableUdpSession();
	virtual ~ReliableUdpSession();

public:
	virtual bool							Connect() override;
	virtual void							Disconnect(const WCHAR* cause) override;
	virtual void							Send(SendBufferRef sendBuffer) override;
	virtual void							SendReliable(SendBufferRef sendBuffer, float timestamp);

	virtual bool							IsTcp() const override { return false; }
	virtual bool							IsUdp() const override { return true; }

	void									HandleAck(uint16 latestSeq, uint32 bitfield);
	bool									CheckAndRecordReceiveHistory(uint16 seq);
	uint32									GenerateAckBitfield(uint16 latestSeq);

private:
	/* Iocp Object impl */
	virtual HANDLE							GetHandle() override;
	virtual void							Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) override;

private:
	//void									RegisterConnect();
	//void									RegisterDisconnect();
	void									RegisterSend();
	//void									RegisterRecv();


	void									ProcessConnect();
	void									ProcessDisconnect();
	void									ProcessSend(int32 numOfBytes);
	//void									ProcessRecv(int32 numOfBytes);

	void									Update(float serverTime);

	void									HandleError(int32 errorCode);
	bool									IsParsingPacket(BYTE* buffer, int32 len);

private:
	USE_LOCK

	RecvBuffer								_recvBuffer;
	Queue<SendBufferRef>					_sendQueue;
	Atomic<bool>							_sendRegistered = false;

protected:
	unordered_map<uint16, PendingPacket>	_pendingAckMap;
	bitset<1024>							_receiveHistory;

	uint16									_sendSeq = 0;			// 다음 보낼 sequence
	float									_resendIntervalMs = 0.1f; // 재전송 대기 시간

private:
	RecvEvent								_recvEvent;
	SendEvent								_sendEvent;
};