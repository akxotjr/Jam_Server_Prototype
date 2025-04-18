#include "pch.h"
#include "Service.h"
#include "Session.h"
#include "Listener.h"
#include "UdpReceiver.h"

/*--------------
	 Service
---------------*/

Service::Service(TransportConfig config, IocpCoreRef core, int32 maxTcpSessionCount, int32 maxUdpSessionCount)
	: _config(config), _iocpCore(core), _maxTcpSessionCount(maxTcpSessionCount), _maxUdpSessionCount(maxUdpSessionCount)
{
}

Service::~Service()
{
	CloseService();
}

bool Service::Start()
{
	if (CanStart() == false)
		return false;

	_listener = MakeShared<Listener>();
	if (_listener == nullptr)
		return false;

	ServiceRef service = static_pointer_cast<Service>(shared_from_this());
	if (_listener->StartAccept(service) == false)
		return false;

	if (_udpReceiver == nullptr)
		return false;

	if (_udpReceiver->Start(service))
		return false;

	return true;
}

void Service::CloseService()
{
	// TODO
}

void Service::Broadcast(SendBufferRef sendBuffer)
{
	//WRITE_LOCK;
	//for (const auto& session : _sessions)
	//{
	//	session->Send(sendBuffer);
	//}
}

SessionRef Service::CreateSession(ProtocolType protocol)
{
	SessionRef session = nullptr;
	if (protocol == ProtocolType::PROTOCOL_TCP)
	{
		session = _tcpSessionFactory();
	}
	else if (protocol == ProtocolType::PROTOCOL_UDP)
	{
		session = _udpSessionFactory();
	}

	session->SetService(shared_from_this());

	if (_iocpCore->Register(session) == false)
		return nullptr;
	return session;
}

void Service::AddTcpSession(TcpSessionRef session)
{
	WRITE_LOCK
	_tcpSessionCount++;
	_tcpSessions.insert(session);
}

void Service::ReleaseTcpSession(TcpSessionRef session)
{
	WRITE_LOCK
	ASSERT_CRASH(_tcpSessions.erase(session) != 0);
	_tcpSessionCount--;
}

void Service::AddUdpSession(ReliableUdpSessionRef session)
{
	WRITE_LOCK
	_udpSessionCount++;
	_udpSessions.insert(session);
}

void Service::ReleaseUdpSession(ReliableUdpSessionRef session)
{
	WRITE_LOCK
	ASSERT_CRASH(_udpSessions.erase(session) != 0);
	_udpSessionCount--;
}

ReliableUdpSessionRef Service::FindOrCreateUdpSession(const NetAddress& from)
{
	WRITE_LOCK

	for (auto& session : _udpSessions)
	{
		if (session->GetRemoteNetAddress() == from)
			return session;
	}

	auto it = _pendingUdpSessions.find(from);
	if (it != _pendingUdpSessions.end())
	{
		return it->second;
	}

	auto newSession = static_pointer_cast<ReliableUdpSession>(CreateSession(ProtocolType::PROTOCOL_UDP));
	if (newSession == nullptr)
		return nullptr;

	newSession->SetRemoteNetAddress(from);
	_pendingUdpSessions[from] = newSession;

	return newSession;
}

void Service::CompleteUdpHandshake(const NetAddress& from)
{
	WRITE_LOCK

	auto it = _pendingUdpSessions.find(from);
	if (it != _pendingUdpSessions.end())
	{
		AddUdpSession(it->second);      // 정규 세션으로 등록
		//AddSession(it->second);              // 공용 세션 관리에 추가 (선택적)
		_pendingUdpSessions.erase(it);
	}
}
