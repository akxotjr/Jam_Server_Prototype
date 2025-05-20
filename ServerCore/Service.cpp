#include "pch.h"
#include "Service.h"
#include "Session.h"
#include "Listener.h"
#include "UdpReceiver.h"

namespace core::network
{
	/*--------------
	 Service
---------------*/

	Service::Service(TransportConfig config, int32 maxTcpSessionCount, int32 maxUdpSessionCount)
		: _config(config), _maxTcpSessionCount(maxTcpSessionCount), _maxUdpSessionCount(maxUdpSessionCount)
	{
		_iocpCore = memory::MakeShared<IocpCore>();
	}

	Service::~Service()
	{
		CloseService();
	}

	bool Service::Start()
	{
		if (CanStart() == false)
			return false;

		_listener = memory::MakeShared<Listener>();
		if (_listener == nullptr)
			return false;

		auto self = static_pointer_cast<Service>(shared_from_this());
		if (_listener->StartAccept(self) == false)
			return false;

		if (_udpReceiver == nullptr)
			return false;

		if (_udpReceiver->Start(self) == false)
			return false;

		return true;
	}

	void Service::CloseService()
	{
		// TODO
	}


	SessionRef Service::CreateSession(ProtocolType protocol)
	{
		SessionRef session = nullptr;
		if (protocol == ProtocolType::PROTOCOL_TCP)
		{
			session = _tcpSessionFactory();
			if (_iocpCore->Register(session) == false)
				return nullptr;
		}
		else if (protocol == ProtocolType::PROTOCOL_UDP)
		{
			session = _udpSessionFactory();
		}

		session->SetService(shared_from_this());
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

		ASSERT_CRASH(_tcpSessions.erase(session) != 0)
		_tcpSessionCount--;
	}

	void Service::AddUdpSession(UdpSessionRef session)
	{
		WRITE_LOCK

		_udpSessionCount++;
		_udpSessions.insert(session);
	}

	void Service::ReleaseUdpSession(UdpSessionRef session)
	{
		WRITE_LOCK

		ASSERT_CRASH(_udpSessions.erase(session) != 0)
		_udpSessionCount--;
	}

	UdpSessionRef Service::FindOrCreateUdpSession(const NetAddress& from)
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

		auto newSession = static_pointer_cast<UdpSession>(CreateSession(ProtocolType::PROTOCOL_UDP));
		if (newSession == nullptr)
			return nullptr;

		newSession->SetRemoteNetAddress(from);

		_pendingUdpSessions[from] = newSession;

		newSession->ProcessConnect();

		return newSession;
	}

	void Service::CompleteUdpHandshake(const NetAddress& from)
	{
		WRITE_LOCK

		auto it = _pendingUdpSessions.find(from);
		if (it != _pendingUdpSessions.end())
		{
			AddUdpSession(it->second);
			_pendingUdpSessions.erase(it);
		}
	}

}

