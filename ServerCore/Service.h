#pragma once

#include "NetAddress.h"
#include <functional>
#include <optional>

#include "UdpReceiver.h"

using SessionFactory = function<SessionRef()>;

/*--------------
	 Service
---------------*/

struct TransportConfig
{
	optional<NetAddress> tcpAddress;
	optional<NetAddress> udpAddress;
};

class Service : public enable_shared_from_this<Service>
{
public:
	Service(TransportConfig config, IocpCoreRef core, int32 maxSTcpSessionCount = 1, int32 maxUdpSessionCount = 1);
	virtual ~Service();

	virtual bool		Start();
	bool				CanStart() { return _sessionFactory != nullptr; }

	virtual void		CloseService();
	void				SetSessionFactory(SessionFactory func) { _sessionFactory = func; }

	void				Broadcast(SendBufferRef sendBuffer);
	SessionRef			CreateSession();

	//void				AddSession(SessionRef session);
	//void				ReleaseSession(SessionRef session);

	void				AddTcpSession(TcpSessionRef session);
	void				ReleaseTcpSession(TcpSessionRef session);

	void				AddUdpSession(ReliableUdpSessionRef session);
	void				ReleaseUdpSession(ReliableUdpSessionRef session);


	int32				GetCurrentTcpSessionCount() { return _tcpSessionCount; }
	int32				GetMaxTcpSessionCount() { return _maxTcpSessionCount; }
	int32				GetCurrentUdpSessionCount() { return _udpSessionCount; }
	int32				GetMaxUdpSessionCount() { return _maxUdpSessionCount; }


	void				SetAndStartUdpReceiver(UdpReceiverRef udpReceiver);

	ReliableUdpSessionRef			FindOrCreateUdpSession(const NetAddress& from);
	void							CompleteUdpHandshake(const NetAddress& from);

public:
	NetAddress			GetTcpNetAddress() { return _config.tcpAddress.value_or(NetAddress(L"0.0.0.0", 0)); }
	NetAddress			GetUdpNetAddress() { return _config.udpAddress.value_or(NetAddress(L"0.0.0.0", 0)); }
	IocpCoreRef&		GetIocpCore() { return _iocpCore; }

protected:
	USE_LOCK

	TransportConfig		_config;

	IocpCoreRef			_iocpCore;

	//Set<SessionRef>		_sessions;

	Set<TcpSessionRef>									_tcpSessions;
	Set<ReliableUdpSessionRef>							_udpSessions;
	unordered_map<NetAddress, ReliableUdpSessionRef>	_pendingUdpSessions;


	int32				_sessionCount = 0;
	int32				_maxSessionCount = 0;

	int32				_tcpSessionCount = 0;
	int32				_maxTcpSessionCount = 0;
	int32				_udpSessionCount = 0;
	int32				_maxUdpSessionCount = 0;

	SessionFactory		_sessionFactory;

private:
	ListenerRef			_listener = nullptr;
	UdpReceiverRef		_udpReceiver = nullptr;
};
