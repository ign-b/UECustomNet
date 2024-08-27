#pragma once

#include <functional>

#include "Common.h"
#include "PacketChannels.h"
#include "PacketPriorities.h"
#include "PacketReliabilities.h"
#include "PlayerSocket.h"
#include "RpcIdentifiers.h"
#include "Containers/String.h"

class CBitStream;
class CPlayerSocket;
class CPacket;

namespace un {
    using TPacketHandler = std::function<void(CPacket *)>;
};

class CNetServerInterface {
public:
    virtual bool Startup(unsigned short nPort, int nMaxPlayers, CString const &sHostAddress) = 0;

    virtual void Shutdown(int nBlockDuration) = 0;

    virtual void Process() = 0;

    virtual void SetPassword(CString const &sPassword) = 0;

    virtual void GetPassword(CString &sPassword) = 0;

    virtual void KickPlayer(un::EntityId nPlayer, bool bSendDisconnectionNotification = true,
                            ePacketPriority nDisconnectionPacketPriority = PRIORITY_LOW) = 0;

    virtual CPlayerSocket *GetPlayerSocket(un::EntityId nPlayer) = 0;

    virtual void SetPacketHandler(un::TPacketHandler const &fnPacketHandler) = 0;

    virtual un::TPacketHandler const &GetPacketHandler() = 0;

    virtual unsigned int Send(CBitStream *pBitStream, ePacketPriority nPriority, ePacketReliability nReliability,
                              un::EntityId nPlayer, bool bBroadcast,
                              char cOrderingChannel = PACKET_CHANNEL_DEFAULT) = 0;

    virtual unsigned int RPC(un::RpcIdentifier nRpc, CBitStream *pBitStream, ePacketPriority nPriority,
                             ePacketReliability nReliability, un::EntityId nPlayer, bool bBroadcast,
                             char cOrderingChannel = PACKET_CHANNEL_DEFAULT) = 0;
};

UNET_API CNetServerInterface *GetNetServerInterface();

UNET_API void DestroyNetServerInterface(CNetServerInterface const *pNetServer);
