#pragma once

#include "NetClientInterface.h"
#include "Packet.h"
#include "PlayerSocket.h"
#include "RakNetInterface.h"
#include "Serial.h"

#ifdef SetPort
#undef SetPort
#endif

class CNetClient : CRakNetInterface, public CNetClientInterface {
protected:
    SLNet::RakPeerInterface *m_pRakPeer;
    SLNet::SystemAddress m_ServerAddress;
    bool m_bConnected;
    CString m_sHost;
    unsigned short m_nPort;
    CString m_sPassword;
    un::TPacketHandler m_fnPacketHandler;
    CPlayerSocket m_ServerSocket;
    CSerial m_Serial;

    un::PacketId ProcessPacket(SLNet::SystemAddress const &SystemAddress, un::PacketId nPacket);

    CPacket *Receive();

    void DeallocatePacket(CPacket *pPacket);

public:
    CNetClient();

    ~CNetClient() override { SAFE_DELETE(m_pRakPeer); }

    bool Startup() override;

    void Shutdown(int nBlockDuration) override;

    eConnectionAttemptResult Connect() override;

    void Disconnect() override;

    void Process() override;

    void SetHost(CString const &sHost) override { m_sHost.Assign(sHost); }
    void GetHost(CString &sHost) override { sHost.Assign(m_sHost); }

    void SetPort(unsigned short nPort) override { m_nPort = nPort; }
    unsigned short GetPort() override { return m_nPort; }

    void SetPassword(CString const &sPassword) override;

    void GetPassword(CString &sPassword) override { sPassword.Assign(m_sPassword); }

    bool IsConnected() override { return m_bConnected; }
    void GetSerial(CString &sSerial) override { m_Serial.GetSerial(sSerial); }
    void SetPacketHandler(un::TPacketHandler const &fnPacketHandler) override { m_fnPacketHandler = fnPacketHandler; }
    un::TPacketHandler const &GetPacketHandler() override { return m_fnPacketHandler; }

    unsigned int Send(CBitStream *pBitStream, ePacketPriority nPriority, ePacketReliability nReliability,
                      char cOrderingChannel = PACKET_CHANNEL_DEFAULT) override;

    unsigned int RPC(un::RpcIdentifier nRpc, CBitStream *pBitStream, ePacketPriority nPriority,
                     ePacketReliability nReliability, char cOrderingChannel = PACKET_CHANNEL_DEFAULT) override;

    SNetStats *GetNetStats() override;

    CPlayerSocket *GetServerSocket() { return &m_ServerSocket; }
};
