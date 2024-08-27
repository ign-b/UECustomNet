#pragma once

#include <functional>

#include "PacketChannels.h"
#include "PacketPriorities.h"
#include "PacketReliabilities.h"
#include "RpcIdentifiers.h"
#include "Containers/String.h"

#ifdef SetPort
#undef SetPort
#endif

struct SNetStats;
class CBitStream;
class CPacket;

namespace un {
    using TPacketHandler = std::function<void(CPacket *)>;
};

enum eConnectionAttemptResult {
    CONNECTION_ATTEMPT_STARTED,
    INVALID_PARAMETER,
    CANNOT_RESOLVE_DOMAIN_NAME,
    ALREADY_CONNECTED_TO_ENDPOINT,
    CONNECTION_ATTEMPT_ALREADY_IN_PROGRESS,
    SECURITY_INITIALIZATION_FAILED,
    NO_HOST_SET,
};

class CNetClientInterface {
public:
    virtual bool Startup() = 0;

    virtual void Shutdown(int nBlockDuration) = 0;

    virtual eConnectionAttemptResult Connect() = 0;

    virtual void Disconnect() = 0;

    virtual void Process() = 0;

    virtual void SetHost(CString const &sHost) = 0;

    virtual void GetHost(CString &sHost) = 0;

    virtual void SetPort(unsigned short nPort) = 0;

    virtual unsigned short GetPort() = 0;

    virtual void SetPassword(CString const &sPassword) = 0;

    virtual void GetPassword(CString &sPassword) = 0;

    virtual bool IsConnected() = 0;

    virtual void GetSerial(CString &sSerial) = 0;

    virtual void SetPacketHandler(un::TPacketHandler const &fnPacketHandler) = 0;

    virtual un::TPacketHandler const &GetPacketHandler() = 0;

    virtual unsigned int Send(CBitStream *pBitStream, ePacketPriority nPriority, ePacketReliability nReliability,
                              char cOrderingChannel = PACKET_CHANNEL_DEFAULT) = 0;

    virtual unsigned int RPC(un::RpcIdentifier nRpc, CBitStream *pBitStream, ePacketPriority nPriority,
                             ePacketReliability nReliability, char cOrderingChannel = PACKET_CHANNEL_DEFAULT) = 0;

    virtual SNetStats *GetNetStats() = 0;
};

UNET_API CNetClientInterface *GetNetClientInterface();

UNET_API void DestroyNetClientInterface(CNetClientInterface const *pNetClient);
