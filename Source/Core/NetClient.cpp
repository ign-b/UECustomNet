#include "NetClient.h"

#include "slikenet/MessageIdentifiers.h"

#include "BitStream.h"
#include "NetStats.h"
#include "PacketIdentifiers.h"

CNetClient::CNetClient() : m_ServerAddress(SLNet::UNASSIGNED_SYSTEM_ADDRESS),
                           m_bConnected(false),
                           m_nPort(-1),
                           m_fnPacketHandler(nullptr),
                           m_ServerSocket(),
                           m_Serial(SERIAL_DUMMY) {
    m_pRakPeer = new SLNet::RakPeer();
    m_pRakPeer->AttachPlugin(this);
}

bool CNetClient::Startup() {
    SLNet::SocketDescriptor SocketDescriptor{};
    return m_pRakPeer->Startup(1, &SocketDescriptor, 1, THREAD_PRIORITY_NORMAL) == SLNet::RAKNET_STARTED;
}

void CNetClient::Shutdown(int const nBlockDuration) {
    m_pRakPeer->Shutdown(nBlockDuration);
}

eConnectionAttemptResult CNetClient::Connect() {
    Disconnect();

    if (!m_sHost.IsEmpty()) {
        return static_cast<eConnectionAttemptResult>(m_pRakPeer->Connect(
            *m_sHost, m_nPort, *m_sPassword, m_sPassword.Length()));
    }

    return NO_HOST_SET;
}

void CNetClient::Disconnect() {
    if (!IsConnected()) {
        return;
    }

    m_bConnected = false;
    m_pRakPeer->CloseConnection(m_ServerAddress, true);
    Shutdown(500);

    Startup();
    m_ServerAddress = SLNet::UNASSIGNED_SYSTEM_ADDRESS;
    m_ServerSocket.Clear();
}

void CNetClient::Process() {
    for (CPacket *pPacket = nullptr; (pPacket = Receive());) {
        if (m_fnPacketHandler) {
            m_fnPacketHandler(pPacket);
        }

        DeallocatePacket(pPacket);
    }
}

void CNetClient::SetPassword(CString const &sPassword) {
    m_pRakPeer->SetIncomingPassword(*sPassword, sPassword.Length());
    m_sPassword.Assign(sPassword);
}

unsigned int CNetClient::Send(CBitStream *pBitStream, ePacketPriority nPriority, ePacketReliability nReliability,
                              char const cOrderingChannel) {
    if (IsConnected() && pBitStream) {
        return m_pRakPeer->Send(reinterpret_cast<const char *>(pBitStream->GetData()),
                                pBitStream->GetNumberOfBytesUsed(),
                                static_cast<PacketPriority>(nPriority), static_cast<PacketReliability>(nReliability),
                                cOrderingChannel, m_ServerAddress, false);
    }
    return 0;
}

unsigned int CNetClient::RPC(un::RpcIdentifier const nRpc, CBitStream *pBitStream, ePacketPriority nPriority,
                             ePacketReliability nReliability, char const cOrderingChannel) {
    if (IsConnected()) {
        CBitStream BitStream{};
        BitStream.Write(static_cast<un::PacketId>(PACKET_RPC));
        BitStream.Write(nRpc);

        if (pBitStream) {
            BitStream.Write(reinterpret_cast<char *>(pBitStream->GetData()), pBitStream->GetNumberOfBytesUsed());
        }

        return m_pRakPeer->Send(reinterpret_cast<const char *>(BitStream.GetData()), BitStream.GetNumberOfBytesUsed(),
                                static_cast<PacketPriority>(nPriority), static_cast<PacketReliability>(nReliability),
                                cOrderingChannel, m_ServerAddress, false);
    }

    return 0;
}

un::PacketId CNetClient::ProcessPacket(SLNet::SystemAddress const &SystemAddress, un::PacketId const nPacket) {
    un::EntityId const nPlayer = SystemAddress.systemIndex;
    if (!IsConnected()) {
        if (nPacket != INTERNAL_PACKET_CONNECTION_REQUEST &&
            nPacket != INTERNAL_PACKET_CONNECTION_REQUEST_ACCEPTED) {
            return un::INVALID_PACKET_ID;
        }
    }

    switch (nPacket) {
        case INTERNAL_PACKET_CONNECTION_REQUEST: {
            m_ServerAddress = SystemAddress;

            m_ServerSocket.SetPlayerId(nPlayer);
            m_ServerSocket.SetBinaryAddress(SystemAddress.address.addr4.sin_addr.s_addr);
            m_ServerSocket.SetPort(ntohs(SystemAddress.address.addr4.sin_port));

            CString sSerial{};
            m_Serial.GetSerial(sSerial);

            CBitStream BitStream{};
            BitStream.Write(static_cast<un::PacketId>(INTERNAL_PACKET_CONNECTION_REQUEST));
            BitStream.Write(static_cast<uint8_t>(NETWORK_MODULE_VERSION));
            BitStream.Write(sSerial);

            m_bConnected = true;
            Send(&BitStream, PRIORITY_HIGH, RELIABILITY_RELIABLE_ORDERED, false);
            m_bConnected = false;

            return un::INVALID_PACKET_ID;
        }

        case INTERNAL_PACKET_CONNECTION_REQUEST_ACCEPTED: {
            m_bConnected = true;

            CBitStream BitStream{};
            BitStream.Write(static_cast<un::PacketId>(INTERNAL_PACKET_CONNECTION_REQUEST_ACCEPTED));

            Send(&BitStream, PRIORITY_HIGH, RELIABILITY_RELIABLE_ORDERED, false);
            return PACKET_CONNECTION_SUCCEEDED;
        }

        case ID_CONNECTION_ATTEMPT_FAILED: { return PACKET_CONNECTION_FAILED; }
        case ID_ALREADY_CONNECTED: { return PACKET_ALREADY_CONNECTED; }
        case ID_NO_FREE_INCOMING_CONNECTIONS: { return PACKET_SERVER_FULL; }
        case ID_DISCONNECTION_NOTIFICATION: { return PACKET_DISCONNECTED; }
        case ID_CONNECTION_LOST: { return PACKET_LOST_CONNECTION; }
        case PACKET_CONNECTION_REJECTED: { return PACKET_CONNECTION_REJECTED; }
        case ID_CONNECTION_BANNED: { return PACKET_BANNED; }
        case ID_INVALID_PASSWORD: { return PACKET_PASSWORD_INVALID; }
        default: { return nPacket; }
    }
}

CPacket *CNetClient::Receive() {
    if (SLNet::Packet *pRakPacket = m_pRakPeer->Receive()) {
        CPacket *pPacket = nullptr;
        unsigned char const *pData = pRakPacket->data + sizeof(un::PacketId);
        unsigned int const nLength = pRakPacket->length - sizeof(un::PacketId);

        if (un::PacketId const nPacket = ProcessPacket(pRakPacket->systemAddress, pRakPacket->data[0]);
            nPacket != un::INVALID_PACKET_ID) {
            pPacket = new CPacket{};
            pPacket->nPacket = nPacket;
            pPacket->pPlayerSocket = &m_ServerSocket;
            pPacket->nLength = nLength;

            if (nLength > 0) {
                pPacket->pData = static_cast<unsigned char *>(malloc(nLength));
                memcpy(pPacket->pData, pData, nLength);
            } else {
                pPacket->pData = nullptr;
            }
        }

        m_pRakPeer->DeallocatePacket(pRakPacket);
        return pPacket;
    }

    return nullptr;
}

void CNetClient::DeallocatePacket(CPacket *pPacket) {
    switch (pPacket->nPacket) {
        case PACKET_CONNECTION_REJECTED:
        case PACKET_DISCONNECTED:
        case PACKET_LOST_CONNECTION: {
            Disconnect();
            break;
        }
        default: { break; }
    }

    SAFE_FREE(pPacket->pData);
    SAFE_DELETE(pPacket);
}

SNetStats *CNetClient::GetNetStats() {
    SLNet::RakNetStatistics const *pRakStats = m_pRakPeer->GetStatistics(SLNet::UNASSIGNED_SYSTEM_ADDRESS);

    static SNetStats NetStats;
    memcpy(NetStats.nValueOverLastSecond, pRakStats->valueOverLastSecond,
           sizeof(un::TNetStat) * NET_STAT_METRICS_COUNT);
    memcpy(NetStats.nRunningTotal, pRakStats->runningTotal, sizeof(un::TNetStat) * NET_STAT_METRICS_COUNT);
    NetStats.nConnectionStartTime = pRakStats->connectionStartTime;
    NetStats.bIsLimitedByCongestionControl = pRakStats->isLimitedByCongestionControl;
    NetStats.nBPSLimitByCongestionControl = pRakStats->BPSLimitByCongestionControl;
    NetStats.bIsLimitedByOutgoingBandwidthLimit = pRakStats->isLimitedByOutgoingBandwidthLimit;
    NetStats.nBPSLimitByOutgoingBandwidthLimit = pRakStats->BPSLimitByOutgoingBandwidthLimit;
    memcpy(NetStats.nMessageInSendBuffer, pRakStats->messageInSendBuffer, sizeof(unsigned int) * PRIORITY_COUNT);
    memcpy(NetStats.dBytesInSendBuffer, pRakStats->bytesInSendBuffer, sizeof(double) * PRIORITY_COUNT);
    NetStats.nMessagesInResendBuffer = pRakStats->messagesInResendBuffer;
    NetStats.nBytesInResendBuffer = pRakStats->bytesInResendBuffer;
    NetStats.fPacketlossLastSecond = pRakStats->packetlossLastSecond;
    NetStats.fPacketlossTotal = pRakStats->packetlossTotal;

    return &NetStats;
}
