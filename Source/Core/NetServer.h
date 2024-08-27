#pragma once

#include "slikenet/types.h"

#include "BitStream.h"
#include "NetServerInterface.h"
#include "Packet.h"
#include "PacketChannels.h"
#include "PacketPriorities.h"
#include "PacketReliabilities.h"
#include "RakNetInterface.h"
#include "RpcIdentifiers.h"
#include "Containers/List.h"
#include "Containers/String.h"

#ifdef SetPort
#undef SetPort
#endif

class CPlayerSocket;

class CNetServer : CRakNetInterface, public CNetServerInterface {
protected:
    SLNet::RakPeerInterface *m_pRakPeer;
    CString m_sPassword;
    un::TPacketHandler m_fnPacketHandler;
    CList<CPlayerSocket *> m_aPlayerSockets;

    un::PacketId ProcessPacket(SLNet::SystemAddress const &SystemAddress, un::PacketId nPacket, unsigned char *pData,
                               unsigned int nLength);

    CPacket *Receive();

    void DeallocatePacket(CPacket *pPacket);

    void RejectKick(un::EntityId nPlayer);

public:
    CNetServer();

    ~CNetServer() override { SAFE_DELETE(m_pRakPeer); }

    bool Startup(unsigned short nPort, int nMaxPlayers, CString const &sHostAddress) override;

    void Shutdown(int nBlockDuration) override { m_pRakPeer->Shutdown(nBlockDuration); }

    void Process() override;

    void SetPassword(CString const &sPassword) override;

    void GetPassword(CString &sPassword) override { return sPassword.Assign(m_sPassword); }

    void KickPlayer(un::EntityId nPlayer, bool bSendDisconnectionNotification = true,
                    ePacketPriority nPacketPriority = PRIORITY_LOW) override;

    CPlayerSocket *GetPlayerSocket(un::EntityId nPlayer) override;

    void SetPacketHandler(un::TPacketHandler const &fnPacketHandler) override { m_fnPacketHandler = fnPacketHandler; }
    un::TPacketHandler const &GetPacketHandler() override { return m_fnPacketHandler; }

    unsigned int Send(CBitStream *pBitStream, ePacketPriority nPriority, ePacketReliability nReliability,
                      un::EntityId nPlayer, bool bBroadcast, char cOrderingChannel = PACKET_CHANNEL_DEFAULT) override;

    unsigned int RPC(un::RpcIdentifier nRpc, CBitStream *pBitStream, ePacketPriority nPriority,
                     ePacketReliability nReliability, un::EntityId nPlayer, bool bBroadcast,
                     char cOrderingChannel = PACKET_CHANNEL_DEFAULT) override;

    bool IsPlayerConnected(un::EntityId nPlayer) { return GetPlayerSocket(nPlayer) != nullptr; }
    void BanIp(CString const &sAddress, unsigned int const nTime) const { m_pRakPeer->AddToBanList(*sAddress, nTime); }
    void UnbanIp(CString const &sAddress) const { m_pRakPeer->RemoveFromBanList(*sAddress); }
};
