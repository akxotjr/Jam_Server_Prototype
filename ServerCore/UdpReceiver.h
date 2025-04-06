#pragma once
#include "IocpCore.h"
#include "IocpEvent.h"
#include "NetAddress.h"
#include "Service.h"

class UdpReceiver : public IocpObject
{
    enum { BUFFER_SIZE = 0x10000 };

public:
    UdpReceiver();

    bool                Start(ServiceRef service);

    /* IocpObject interface impl */
    virtual HANDLE      GetHandle() override;
    virtual void        Dispatch(IocpEvent* iocpEvent, int32 numOfBytes = 0) override;

private:
    void                RegisterRecv();
    int32               IsParsingPacket(BYTE* buffer, const int32 len);

private:
    SOCKET                      _socket = INVALID_SOCKET;
    RecvBuffer                  _recvBuffer;
    SOCKADDR_IN                 _fromAddr = {};
    RecvEvent                   _recvEvent;

    //NetAddress                  _remoteAddr;

    std::weak_ptr<Service>      _service;
};

