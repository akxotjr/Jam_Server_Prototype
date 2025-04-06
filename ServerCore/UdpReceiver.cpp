#include "pch.h"
#include "UdpReceiver.h"
#include "SocketUtils.h"


UdpReceiver::UdpReceiver() : _recvBuffer(BUFFER_SIZE)
{
    memset(&_fromAddr, 0, sizeof(_fromAddr));
}

bool UdpReceiver::Start(ServiceRef service)
{
    _service = service;
    if (_service.lock() == nullptr)
        return;

    _socket = SocketUtils::CreateSocket(ProtocolType::PROTOCOL_UDP);
    if (_socket == INVALID_SOCKET)
        return;

    if (_service.lock()->GetIocpCore()->Register(shared_from_this()) == false)
        return false;

    if (SocketUtils::SetReuseAddress(_socket, true) == false)
        return false;

    if (SocketUtils::SetLinger(_socket, 0, 0) == false)
        return false;

    if (SocketUtils::Bind(_socket, _service.lock()->GetNetAddress()) == false)
        return false;

    if (SocketUtils::Listen(_socket) == false)
        return false;

    RegisterRecv();
    return true;
}

HANDLE UdpReceiver::GetHandle()
{
    return reinterpret_cast<HANDLE>(_socket);
}

void UdpReceiver::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes)
{
    if (numOfBytes == 0)
        return;

    if (iocpEvent->eventType != EventType::Recv)
        return;

    if (_recvBuffer.OnWrite(numOfBytes) == false)
        return;


    NetAddress from(_fromAddr);     // _fromAddr 이 바뀌는 경우는? 
    auto service = _service.lock();
    if (service == nullptr)
        return;

    auto session = service->FindOrCreateUdpSession(from);
    // TODO

    RegisterRecv();
}

void UdpReceiver::RegisterRecv()
{
    int fromLen = sizeof(_fromAddr);

    _recvEvent.Init();
    _recvEvent.owner = shared_from_this();

    WSABUF wsaBuf = { _recvBuffer.FreeSize(), reinterpret_cast<CHAR*>(_recvBuffer.WritePos()) };

    DWORD numOfBytes = 0;
    DWORD flags = 0;

    if (SOCKET_ERROR == ::WSARecvFrom(_socket, &wsaBuf, 1, OUT &numOfBytes, OUT& flags, reinterpret_cast<SOCKADDR*>(&_fromAddr), OUT &fromLen, &_recvEvent, nullptr))
    {
        const int errorCode = ::WSAGetLastError();
        if (errorCode == WSA_IO_PENDING)
        {
            std::cout << "WSARecvFrom failed: " << errorCode << std::endl;
            _recvEvent.owner = nullptr;
        }
    }
}

int32 UdpReceiver::IsParsingPacket(BYTE* buffer, const int32 len)
{
    int32 processLen = 0;

    while (true)
    {
        int32 dataSize = len - processLen;

        // 최소한 헤더는 파싱할 수 있어야 한다.
        if (dataSize < sizeof(PacketHeader))
            break;

        PacketHeader header = *reinterpret_cast<PacketHeader*>(&buffer[processLen]);

        // 헤더에 기록된 패킷 크기를 파싱할 수 있어야 한다.
        if (dataSize < header.size)
            break;

        // 패킷 조립 성공
        //OnRecv(&buffer[0], header.size);
        processLen += header.size;
    }
    return processLen;
}