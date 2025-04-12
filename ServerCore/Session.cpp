#include "pch.h"
#include "Session.h"
#include "SocketUtils.h"
#include "Service.h"
//#include "SendBuffer.h"

/*--------------
	Session
---------------*/

//Session::Session() : _recvBuffer(BUFFER_SIZE)
//{
//	_socket = SocketUtils::CreateSocket();
//}
//
//Session::~Session()
//{
//	SocketUtils::Close(_socket);
//}
//
//void Session::Send(SendBufferRef sendBuffer)
//{
//	if (IsConnected() == false)
//		return;
//
//	bool registerSend = false;
//
//	{
//		WRITE_LOCK;
//		_sendQueue.push(sendBuffer);
//
//		if (_sendRegistered.exchange(true) == false)
//			registerSend = true;
//	}
//
//	if (registerSend)
//		RegisterSend();
//}
//
//bool Session::Conenect()
//{
//	return RegisterConnect();
//}
//
//void Session::Disconnect(const WCHAR* cause)
//{
//	if (_connected.exchange(false) == false)
//		return;
//
//	// TEMP
//	wcout << "Disconnect : " << cause << endl;
//
//	RegisterDisconnect();
//}
//
//HANDLE Session::GetHandle()
//{
//	return reinterpret_cast<HANDLE>(_socket);
//}
//
//void Session::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes)
//{
//	switch (iocpEvent->eventType)
//	{
//	case EventType::Connect:
//		ProcessConnect();
//		break;
//	case EventType::Disconnect:
//		ProcessDisconnect();
//		break;
//	case EventType::Recv:
//		ProcessRecv(numOfBytes);
//		break;
//	case EventType::Send:
//		ProcessSend(numOfBytes);
//	default:
//		break;
//	}
//}
//
//bool Session::RegisterConnect()
//{
//	if (IsConnected()) return false;
//
//	if (GetService()->GetServiceType() != ServiceType::Client) return false;
//
//	if (SocketUtils::SetReuseAddress(_socket, true) == false) return false;
//
//	if (SocketUtils::BindAnyAddress(_socket, 0) == false) return false;
//
//	_connectEvent.Init();
//	_connectEvent.owner = shared_from_this();
//
//	DWORD numOfBytes = 0;
//	SOCKADDR_IN sockAddr = GetService()->GetNetAddress().GetSockAddr();
//	if (SOCKET_ERROR == SocketUtils::ConnectEx(_socket, reinterpret_cast<SOCKADDR*>(&sockAddr), sizeof(sockAddr), nullptr, 0, &numOfBytes, &_connectEvent))
//	{
//		int32 errorCode = ::WSAGetLastError();
//		if (errorCode != WSA_IO_PENDING)
//		{
//			_connectEvent.owner = nullptr;
//			return false;
//		}
//	}
//	return true;
//}
//
//bool Session::RegisterDisconnect()
//{
//	_disconnectEvent.Init();
//	_disconnectEvent.owner = shared_from_this();
//
//	if (false == SocketUtils::DisconnectEx(_socket, &_disconnectEvent, TF_REUSE_SOCKET, 0))
//	{
//		int32 errorCode = ::WSAGetLastError();
//		if (errorCode != WSA_IO_PENDING)
//		{
//			_disconnectEvent.owner = nullptr;
//			return false;
//		}
//	}
//	return true;
//}
//
//void Session::RegisterRecv()
//{
//	if (IsConnected() == false)
//		return;
//
//	_recvEvent.Init();
//	_recvEvent.owner = shared_from_this();
//
//	WSABUF wsaBuf;
//	wsaBuf.buf = reinterpret_cast<char*>(_recvBuffer.WritePos());
//	wsaBuf.len = _recvBuffer.FreeSize();
//
//	DWORD numOfBytes = 0;
//	DWORD flags = 0;
//	if (SOCKET_ERROR == ::WSARecv(_socket, &wsaBuf, 1, OUT &numOfBytes, OUT &flags, &_recvEvent, nullptr))
//	{
//		int32 errorCode = ::WSAGetLastError();
//		if (errorCode != WSA_IO_PENDING)
//		{
//			HandleError(errorCode);
//			_recvEvent.owner = nullptr;		// RELEASSE_REF
//		}
//	}
//}
//
//void Session::RegisterSend()
//{
//	if (IsConnected() == false)
//		return;
//
//	_sendEvent.Init();
//	_sendEvent.owner = shared_from_this();
//
//	// 보낼 데이터를 sendEvent에 등록
//	{
//		WRITE_LOCK;
//
//		int32 writeSize = 0;
//		while (_sendQueue.empty() == false)
//		{
//			SendBufferRef sendBuffer = _sendQueue.front();
//
//			writeSize += sendBuffer->WriteSize();
//			// TODO : 예외 체크
//
//			_sendQueue.pop();
//			_sendEvent.sendBuffers.push_back(sendBuffer);
//		}
//	}
//
//	// Scatter-Gather (흩어져 있는 데이터들을 모아서 한번에 보냄)
//	Vector<WSABUF> wsaBufs;
//	wsaBufs.reserve(_sendEvent.sendBuffers.size());
//	for (SendBufferRef sendBuffer : _sendEvent.sendBuffers)
//	{
//		WSABUF wsaBuf;
//		wsaBuf.buf = reinterpret_cast<char*>(sendBuffer->Buffer());
//		wsaBuf.len = static_cast<LONG>(sendBuffer->WriteSize());
//		wsaBufs.push_back(wsaBuf);
//	}
//
//	DWORD numOfBytes = 0;
//	if (SOCKET_ERROR == ::WSASend(_socket, wsaBufs.data(), static_cast<DWORD>(wsaBufs.size()), OUT & numOfBytes, 0, &_sendEvent, nullptr))
//	{
//		int32 errorCode = ::WSAGetLastError();
//		if (errorCode != WSA_IO_PENDING)
//		{
//			HandleError(errorCode);
//			_sendEvent.owner = nullptr;
//			_sendEvent.sendBuffers.clear();
//			_sendRegistered.store(false);
//		}
//	}
//}
//
//void Session::ProcessConnect()
//{
//	_connectEvent.owner = nullptr;
//
//	_connected.store(true);
//
//	//세션 등록
//	GetService()->AddSession(GetSessionRef());
//
//	// 콘텐츠 코드에서 오버로딩
//	OnConnected();
//
//	// 수신 등록
//	RegisterRecv();
//}
//
//void Session::ProcessDisconnect()
//{
//	_disconnectEvent.owner = nullptr;
//
//	OnDisconnected(); // 컨텐츠 코드에서 오버로딩
//	GetService()->ReleaseSession(GetSessionRef());
//}
//
//void Session::ProcessRecv(int32 numOfBytes)
//{
//	_recvEvent.owner = nullptr;
//
//	if (numOfBytes == 0)
//	{
//		Disconnect(L"Recv 0");
//		return;
//	}
//
//	if (_recvBuffer.OnWrite(numOfBytes) == false)
//	{
//		Disconnect(L"OnWrite Overflow");
//		return;
//	}
//
//	int32 dataSize = _recvBuffer.DataSize();
//	int32 processLen = OnRecv(_recvBuffer.ReadPos(), dataSize);
//
//	if (processLen < 0 || dataSize < processLen || _recvBuffer.OnRead(processLen) == false)
//	{
//		Disconnect(L"OnRead Overflow");
//		return;
//	}
//
//	_recvBuffer.Clean();
//
//	RegisterRecv();
//}
//
//void Session::ProcessSend(int32 numOfBytes)
//{
//	_sendEvent.owner = nullptr;
//	_sendEvent.sendBuffers.clear();
//
//	if (numOfBytes == 0)
//	{
//		Disconnect(L"Send 0");
//		return;
//	}
//
//	// 콘텐츠에서 오버로딩
//	OnSend(numOfBytes);
//
//	WRITE_LOCK
//	if (_sendQueue.empty())
//		_sendRegistered.store(false);
//	else
//		RegisterSend();
//}
//
//void Session::HandleError(int32 errorCode)
//{
//	switch (errorCode)
//	{
//	case WSAECONNRESET:
//	case WSAECONNABORTED:
//		Disconnect(L"Handle Error");
//		break;
//	default:
//		// TODO : Log
//		cout << "Handle Error : " << errorCode << endl;
//		break;
//	}
//}
//
//PacketSession::PacketSession()
//{
//}
//
//PacketSession::~PacketSession()
//{
//}
//
//int32 PacketSession::OnRecv(BYTE* buffer, int32 len)
//{
//	int32 processLen = 0;
//
//	while (true)
//	{
//		int32 dataSize = len - processLen;
//
//		// 최소한 헤더는 파싱할 수 있어야 한다.
//		if (dataSize < sizeof(PacketHeader))
//			break;
//
//		PacketHeader header = *reinterpret_cast<PacketHeader*>(&buffer[processLen]);
//
//		// 헤더에 기록된 패킷 크기를 파싱할 수 있어야 한다.
//		if (dataSize < header.size)
//			break;
//
//		// 패킷 조립 성공
//		OnRecvPacket(&buffer[0], header.size);
//		processLen += header.size;
//	}
//	return processLen;
//}

/*--------------
	Session
---------------*/

Session::~Session()
{

}




/*-----------------
	 TcpSession 
------------------*/

TcpSession::TcpSession() : _recvBuffer(BUFFER_SIZE)
{
	_socket = SocketUtils::CreateSocket(ProtocolType::PROTOCOL_TCP);
}

TcpSession::~TcpSession()
{
	SocketUtils::Close(_socket);
}

bool TcpSession::Connect()
{
	return RegisterConnect();
}

void TcpSession::Disconnect(const WCHAR* cause)
{
	if (_connected.exchange(false) == false)
		return;

	RegisterDisconnect();
}

void TcpSession::Send(SendBufferRef sendBuffer)
{
	if (IsConnected() == false)
		return;

	bool registerSend = false;

	{
		WRITE_LOCK;
		_sendQueue.push(sendBuffer);

		if (_sendRegistered.exchange(true) == false)
			registerSend = true;
	}

	if (registerSend)
		RegisterSend();
}

HANDLE TcpSession::GetHandle()
{
	return reinterpret_cast<HANDLE>(_socket);
}

void TcpSession::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes)
{
	switch (iocpEvent->eventType)
	{
	case EventType::Connect:
		ProcessConnect();
		break;
	case EventType::Disconnect:
		ProcessDisconnect();
		break;
	case EventType::Recv:
		ProcessRecv(numOfBytes);
		break;
	case EventType::Send:
		ProcessSend(numOfBytes);
	default:
		break;
	}
}

bool TcpSession::RegisterConnect()
{
	if (IsConnected()) return false;

	if (GetService()->GetServiceType() != ServiceType::Client) return false;

	if (SocketUtils::SetReuseAddress(_socket, true) == false) return false;

	if (SocketUtils::BindAnyAddress(_socket, 0) == false) return false;

	_connectEvent.Init();
	_connectEvent.owner = shared_from_this();

	DWORD numOfBytes = 0;
	SOCKADDR_IN sockAddr = GetService()->GetNetAddress().GetSockAddr();
	if (SOCKET_ERROR == SocketUtils::ConnectEx(_socket, reinterpret_cast<SOCKADDR*>(&sockAddr), sizeof(sockAddr), nullptr, 0, &numOfBytes, &_connectEvent))
	{
		const int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			_connectEvent.owner = nullptr;
			return false;
		}
	}
	return true;
}

bool TcpSession::RegisterDisconnect()
{
	_disconnectEvent.Init();
	_disconnectEvent.owner = shared_from_this();

	if (false == SocketUtils::DisconnectEx(_socket, &_disconnectEvent, TF_REUSE_SOCKET, 0))
	{
		const int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			_disconnectEvent.owner = nullptr;
			return false;
		}
	}
	return true;
}

void TcpSession::RegisterRecv()
{
	if (IsConnected() == false)
		return;

	_recvEvent.Init();
	_recvEvent.owner = shared_from_this();

	WSABUF wsaBuf;
	wsaBuf.buf = reinterpret_cast<char*>(_recvBuffer.WritePos());
	wsaBuf.len = _recvBuffer.FreeSize();

	DWORD numOfBytes = 0;
	DWORD flags = 0;
	if (SOCKET_ERROR == ::WSARecv(_socket, &wsaBuf, 1, OUT &numOfBytes, OUT &flags, &_recvEvent, nullptr))
	{
		const int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			_recvEvent.owner = nullptr;
		}
	}
}

void TcpSession::RegisterSend()
{
	if (IsConnected() == false)
		return;

	_sendEvent.Init();
	_sendEvent.owner = shared_from_this();

	{
		WRITE_LOCK;

		int32 writeSize = 0;
		while (_sendQueue.empty() == false)
		{
			SendBufferRef sendBuffer = _sendQueue.front();

			writeSize += sendBuffer->WriteSize();
			// TODO: exception check

			_sendQueue.pop();
			_sendEvent.sendBuffers.push_back(sendBuffer);
		}
	}

	// Scatter-Gather
	Vector<WSABUF> wsaBufs;
	wsaBufs.reserve(_sendEvent.sendBuffers.size());
	for (const SendBufferRef& sendBuffer : _sendEvent.sendBuffers)
	{
		WSABUF wsaBuf;
		wsaBuf.buf = reinterpret_cast<char*>(sendBuffer->Buffer());
		wsaBuf.len = static_cast<LONG>(sendBuffer->WriteSize());
		wsaBufs.push_back(wsaBuf);
	}

	DWORD numOfBytes = 0;
	if (SOCKET_ERROR == ::WSASend(_socket, wsaBufs.data(), static_cast<DWORD>(wsaBufs.size()), OUT & numOfBytes, 0, &_sendEvent, nullptr))
	{
		const int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			_sendEvent.owner = nullptr;
			_sendEvent.sendBuffers.clear();
			_sendRegistered.store(false);
		}
	}
}

void TcpSession::ProcessConnect()
{
	_connectEvent.owner = nullptr;
	_connected.store(true);

	GetService()->AddSession(GetSessionRef());
	OnConnected();
	RegisterRecv();
}

void TcpSession::ProcessDisconnect()
{
	_disconnectEvent.owner = nullptr;

	OnDisconnected();
	GetService()->ReleaseSession(GetSessionRef());
}

void TcpSession::ProcessRecv(int32 numOfBytes)
{
	_recvEvent.owner = nullptr;

	if (numOfBytes == 0)
	{
		Disconnect(L"Receive 0 byte");
		return;
	}

	if (_recvBuffer.OnWrite(numOfBytes) == false)
	{
		Disconnect(L"OnWrite Overflow");
		return;
	}

	const int32 dataSize = _recvBuffer.DataSize();
	const int32 processLen = IsParsingPacket(_recvBuffer.ReadPos(), dataSize);

	if (processLen < 0 || dataSize < processLen || _recvBuffer.OnRead(processLen) == false)
	{
		Disconnect(L"OnRead Overflow");
		return;
	}

	_recvBuffer.Clean();

	RegisterRecv();
}

void TcpSession::ProcessSend(int32 numOfBytes)
{
	_sendEvent.owner = nullptr;
	_sendEvent.sendBuffers.clear();

	if (numOfBytes == 0)
	{
		Disconnect(L"Send 0 byte");
		return;
	}

	OnSend(numOfBytes);

	WRITE_LOCK
	if (_sendQueue.empty())
		_sendRegistered.store(false);
	else
		RegisterSend();
}

int32 TcpSession::IsParsingPacket(BYTE* buffer, const int32 len)
{
	int32 processLen = 0;

	while (true)
	{
		int32 dataSize = len - processLen;

		// 최소한 헤더는 파싱할 수 있어야 한다.
		if (dataSize < sizeof(TcpPacketHeader))
			break;

		TcpPacketHeader header = *reinterpret_cast<TcpPacketHeader*>(&buffer[processLen]);

		// 헤더에 기록된 패킷 크기를 파싱할 수 있어야 한다.
		if (dataSize < header.size)
			break;

		// 패킷 조립 성공
		OnRecv(&buffer[0], header.size);
		processLen += header.size;
	}
	return processLen;
}

void TcpSession::HandleError(int32 errorCode)
{
	switch (errorCode)
	{
	case WSAECONNRESET:
	case WSAECONNABORTED:
		Disconnect(L"Handle Error");
		break;
	default:
		cout << "Handle Error : " << errorCode << '\n';
		break;
	}
}





/*--------------------------
	 ReliableUdpSession
---------------------------*/


ReliableUdpSession::ReliableUdpSession() : _recvBuffer(BUFFER_SIZE)
{
	_socket = SocketUtils::CreateSocket(ProtocolType::PROTOCOL_UDP);
}

ReliableUdpSession::~ReliableUdpSession()
{
	SocketUtils::Close(_socket);
}

bool ReliableUdpSession::Connect()
{
	return false;
}

void ReliableUdpSession::Disconnect(const WCHAR* cause)
{
}

void ReliableUdpSession::Send(SendBufferRef sendBuffer)
{
	if (IsConnected() == false)
		return;

	bool registerSend = false;

	{
		WRITE_LOCK;
		_sendQueue.push(sendBuffer);

		if (_sendRegistered.exchange(true) == false)
			registerSend = true;
	}

	if (registerSend)
		RegisterSend();
}

void ReliableUdpSession::SendReliable(SendBufferRef sendBuffer, float timestamp)
{
	uint16 seq = _sendSeq++;

	UdpPacketHeader* header = reinterpret_cast<UdpPacketHeader*>(sendBuffer->Buffer());
	header->sequence = seq;

	PendingPacket pkt = { .buffer= sendBuffer, .sequence= seq, .timestamp= timestamp, .retryCount= 0};

	{
		WRITE_LOCK;
		_pendingAckMap[seq] = pkt;
	}

	Send(sendBuffer);
}



HANDLE ReliableUdpSession::GetHandle()
{
	return reinterpret_cast<HANDLE>(_socket);
}

void ReliableUdpSession::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes)
{
	switch (iocpEvent->eventType)
	{
	case EventType::Recv:
		ProcessRecv(numOfBytes);
		break;
	case EventType::Send:
		ProcessSend(numOfBytes);
	default:
		break;
	}
}

void ReliableUdpSession::RegisterSend()
{
	if (IsConnected() == false)
		return;

	_sendEvent.Init();
	_sendEvent.owner = shared_from_this();

	{
		WRITE_LOCK;

		int32 writeSize = 0;
		while (_sendQueue.empty() == false)
		{
			SendBufferRef sendBuffer = _sendQueue.front();

			writeSize += sendBuffer->WriteSize();
			// TODO: exception check

			_sendQueue.pop();
			_sendEvent.sendBuffers.push_back(sendBuffer);
		}
	}

	// Scatter-Gather
	Vector<WSABUF> wsaBufs;
	wsaBufs.reserve(_sendEvent.sendBuffers.size());
	for (const SendBufferRef& sendBuffer : _sendEvent.sendBuffers)
	{
		WSABUF wsaBuf;
		wsaBuf.buf = reinterpret_cast<char*>(sendBuffer->Buffer());
		wsaBuf.len = static_cast<LONG>(sendBuffer->WriteSize());
		wsaBufs.push_back(wsaBuf);
	}

	DWORD numOfBytes = 0;

	if (SOCKET_ERROR == ::WSASendTo(_socket, wsaBufs.data(), static_cast<DWORD>(wsaBufs.size()), OUT &numOfBytes, 0, reinterpret_cast<SOCKADDR*>(&_remoteAddr.GetSockAddr()), sizeof(SOCKADDR_IN), &_sendEvent, nullptr))
	{
		const int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			_sendEvent.owner = nullptr;
			_sendEvent.sendBuffers.clear();
			_sendRegistered.store(false);
		}
	}
}

void ReliableUdpSession::RegisterRecv()
{
	int32 fromLen = sizeof(_remoteAddr.GetSockAddr());

	_recvEvent.Init();
	_recvEvent.owner = shared_from_this();

	WSABUF wsaBuf = {};
	wsaBuf.len = _recvBuffer.FreeSize();
	wsaBuf.buf = reinterpret_cast<CHAR*>(_recvBuffer.WritePos());

	DWORD numOfBytes = 0;
	DWORD flags = 0;

	if (SOCKET_ERROR == ::WSARecvFrom(_socket, &wsaBuf, 1, OUT &numOfBytes, OUT& flags, reinterpret_cast<SOCKADDR*>(&_remoteAddr.GetSockAddr()), OUT &fromLen, &_recvEvent, nullptr));
	{
		const int errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			std::cout << "WSARecvFrom failed: " << errorCode << std::endl;
			_recvEvent.owner = nullptr;
		}
	}

}

void ReliableUdpSession::ProcessSend(int32 numOfBytes)
{
	_sendEvent.owner = nullptr;
	_sendEvent.sendBuffers.clear();

	if (numOfBytes == 0)
	{
		Disconnect(L"Send 0 byte");
		return;
	}

	OnSend(numOfBytes);

	WRITE_LOCK
	if (_sendQueue.empty())
		_sendRegistered.store(false);
	else
		RegisterSend();
}

void ReliableUdpSession::ProcessRecv(int32 numOfBytes)
{
	_recvEvent.owner = nullptr;

	if (_recvBuffer.OnWrite(numOfBytes) == false)
	{
		return;
	}

	const int32 dataSize = _recvBuffer.DataSize();
	const int32 processLen = IsParsingPacket(_recvBuffer.ReadPos(), dataSize);

	if (processLen < 0 || dataSize < processLen || _recvBuffer.OnRead(processLen) == false)
	{
		return;
	}

	_recvBuffer.Clean();

	RegisterRecv();
	return;
}

void ReliableUdpSession::Update(float serverTime)
{
	Vector<uint16> resendList;

	{
		WRITE_LOCK

		for (auto& [seq, pkt] : _pendingAckMap)
		{
			float elapsed = serverTime - pkt.timestamp;

			if (elapsed >= _resendIntervalMs)
			{
				pkt.timestamp = serverTime;
				pkt.retryCount++;

				resendList.push_back(seq);
			}

			if (pkt.retryCount > 5)
			{
				std::cout << "[ReliableUDP] Max retry reached. Disconnecting.\n";
				Disconnect(L"Too many retries");
				continue;
			}
		}
	}

	// 락 밖에서 재전송 호출
	for (uint16 seq : resendList)
	{
		auto it = _pendingAckMap.find(seq);
		if (it != _pendingAckMap.end())
		{
			std::cout << "[ReliableUDP] Re-sending seq: " << seq << "\n";
			Send(it->second.buffer);
		}
	}
}

void ReliableUdpSession::HandleAck(uint16 ackSeq)
{
	WRITE_LOCK;
	_pendingAckMap.erase(ackSeq);
}

void ReliableUdpSession::HandleError(int32 errorCode)
{
	switch (errorCode)
	{
	case WSAECONNRESET:
	case WSAECONNABORTED:
		Disconnect(L"Handle Error");
		break;
	default:
		cout << "Handle Error : " << errorCode << '\n';
		break;
	}
}

bool ReliableUdpSession::IsParsingPacket(BYTE* buffer, const int32 len)
{
	int32 processLen = 0;

	while (true)
	{
		int32 dataSize = len - processLen;

		if (dataSize < sizeof(UdpPacketHeader))
			break;

		UdpPacketHeader header = *reinterpret_cast<UdpPacketHeader*>(&buffer[processLen]);

		if (dataSize < header.size)
			break;

		OnRecv(&buffer[0], header.size);
		processLen += header.size;
	}
	return processLen;
}
