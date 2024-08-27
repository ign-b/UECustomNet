#include "NetServer.h"

#include "slikenet/MessageIdentifiers.h"

#include "PacketIdentifiers.h"
#include "PlayerSocket.h"

CNetServer::CNetServer() : m_sPassword(),
                           m_fnPacketHandler(nullptr),
                           m_aPlayerSockets() {
    m_pRakPeer = new SLNet::RakPeer();
    m_pRakPeer->AttachPlugin(this);
}

bool CNetServer::Startup(unsigned short const nPort, int const nMaxPlayers, CString const &sHostAddress) {
    SLNet::SocketDescriptor SocketDescriptor{nPort, *sHostAddress};

    bool const bStarted = m_pRakPeer->Startup(nMaxPlayers, &SocketDescriptor, 1, THREAD_PRIORITY_NORMAL) ==
                          SLNet::RAKNET_STARTED;
    if (bStarted) {
        m_pRakPeer->SetMaximumIncomingConnections(nMaxPlayers);
    }

    return bStarted;
}

void CNetServer::Process() {
    for (CPacket *pPacket = nullptr; (pPacket = Receive());) {
        if (m_fnPacketHandler) {
            m_fnPacketHandler(pPacket);
        }

        DeallocatePacket(pPacket);
    }
}

void CNetServer::SetPassword(CString const &sPassword) {
    m_pRakPeer->SetIncomingPassword(*sPassword, sPassword.Length());
    m_sPassword.Assign(sPassword);
}

unsigned int CNetServer::Send(CBitStream *pBitStream, ePacketPriority nPriority, ePacketReliability nReliability,
                              un::EntityId const nPlayer, bool const bBroadcast, char const cOrderingChannel) {
    if (!pBitStream) {
        return 0;
    }

    return m_pRakPeer->Send(reinterpret_cast<const char *>(pBitStream->GetData()), pBitStream->GetNumberOfBytesUsed(),
                            static_cast<PacketPriority>(nPriority), static_cast<PacketReliability>(nReliability),
                            cOrderingChannel,
                            nPlayer == un::INVALID_ENTITY_ID
                                ? SLNet::UNASSIGNED_SYSTEM_ADDRESS
                                : m_pRakPeer->GetSystemAddressFromIndex(nPlayer), bBroadcast);
}

unsigned int CNetServer::RPC(un::RpcIdentifier const nRpc, CBitStream *pBitStream, ePacketPriority nPriority,
                             ePacketReliability nReliability, un::EntityId const nPlayer, bool const bBroadcast,
                             char const cOrderingChannel) {
    CBitStream BitStream{};
    BitStream.Write(static_cast<un::PacketId>(PACKET_RPC));
    BitStream.Write(nRpc);

    if (pBitStream) {
        BitStream.Write(reinterpret_cast<const char *>(pBitStream->GetData()), pBitStream->GetNumberOfBytesUsed());
    }

    return m_pRakPeer->Send(reinterpret_cast<const char *>(BitStream.GetData()), BitStream.GetNumberOfBytesUsed(),
                            static_cast<PacketPriority>(nPriority), static_cast<PacketReliability>(nReliability),
                            cOrderingChannel,
                            nPlayer == un::INVALID_ENTITY_ID
                                ? SLNet::UNASSIGNED_SYSTEM_ADDRESS
                                : m_pRakPeer->GetSystemAddressFromIndex(nPlayer), bBroadcast);
}

void CNetServer::RejectKick(un::EntityId const nPlayer) {
    CBitStream BitStream{};
    BitStream.Write(static_cast<un::PacketId>(PACKET_CONNECTION_REJECTED));

    Send(&BitStream, PRIORITY_IMMEDIATE, RELIABILITY_RELIABLE_ORDERED, nPlayer, false);

    KickPlayer(nPlayer, false);
}

un::PacketId CNetServer::ProcessPacket(SLNet::SystemAddress const &SystemAddress, un::PacketId const nPacket,
                                       unsigned char *pData,
                                       unsigned int const nLength) {
    un::EntityId const nPlayer = SystemAddress.systemIndex;
    if (!IsPlayerConnected(nPlayer)) {
        if (nPacket != ID_NEW_INCOMING_CONNECTION &&
            nPacket != INTERNAL_PACKET_CONNECTION_REQUEST) {
            return un::INVALID_PACKET_ID;
        }
    }

    switch (nPacket) {
        case ID_NEW_INCOMING_CONNECTION: {
            CBitStream BitStream{};
            BitStream.Write(static_cast<un::PacketId>(INTERNAL_PACKET_CONNECTION_REQUEST));

            Send(&BitStream, PRIORITY_HIGH, RELIABILITY_RELIABLE_ORDERED, nPlayer, false);
            return un::INVALID_PACKET_ID;
        }

        case INTERNAL_PACKET_CONNECTION_REQUEST: {
            CBitStream BitStream{pData, nLength, false};

            uint8_t nModuleVersion = 0;
            if (!BitStream.Read(nModuleVersion)) {
                RejectKick(nPlayer);
                printf("Can't read network module version\n");
                return un::INVALID_PACKET_ID;
            }

            if (nModuleVersion != NETWORK_MODULE_VERSION) {
                RejectKick(nPlayer);
                printf("Can't validate network module version\n");
                return un::INVALID_PACKET_ID;
            }

            CString sSerial{};
            if (!BitStream.Read(sSerial)) {
                RejectKick(nPlayer);
                return un::INVALID_PACKET_ID;
            }

            auto *pPlayerSocket = new CPlayerSocket{
                nPlayer, SystemAddress.address.addr4.sin_addr.s_addr,
                ntohs(SystemAddress.address.addr4.sin_port), sSerial
            };
            m_aPlayerSockets.PushBack(pPlayerSocket);

            BitStream.Reset();
            BitStream.Write(static_cast<un::PacketId>(INTERNAL_PACKET_CONNECTION_REQUEST_ACCEPTED));

            Send(&BitStream, PRIORITY_HIGH, RELIABILITY_RELIABLE_ORDERED, nPlayer, false);
            return un::INVALID_PACKET_ID;
        }

        case INTERNAL_PACKET_CONNECTION_REQUEST_ACCEPTED: { return PACKET_NEW_CONNECTION; }
        case ID_DISCONNECTION_NOTIFICATION: { return PACKET_DISCONNECTED; }
        case ID_CONNECTION_LOST: { return PACKET_LOST_CONNECTION; }
        default: { return nPacket; }
    }
}

CPacket *CNetServer::Receive() {
    if (SLNet::Packet *pRakPacket = m_pRakPeer->Receive()) {
        CPacket *pPacket = nullptr;
        unsigned char *pData = pRakPacket->data + sizeof(un::PacketId);
        unsigned int const nLength = pRakPacket->length - sizeof(un::PacketId);

        if (un::PacketId const nPacket = ProcessPacket(pRakPacket->systemAddress, pRakPacket->data[0], pData, nLength);
            nPacket != un::INVALID_PACKET_ID) {
            pPacket = new CPacket{};
            pPacket->pPlayerSocket = GetPlayerSocket(pRakPacket->systemAddress.systemIndex);
            pPacket->nPacket = nPacket;
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

void CNetServer::DeallocatePacket(CPacket *pPacket) {
    if (pPacket->nPacket == PACKET_DISCONNECTED ||
        pPacket->nPacket == PACKET_LOST_CONNECTION) {
        CPlayerSocket *pPlayerSocket = pPacket->pPlayerSocket;
        SAFE_DELETE(pPlayerSocket);

        m_aPlayerSockets.Remove(pPlayerSocket);
    }

    SAFE_FREE(pPacket->pData);
    SAFE_DELETE(pPacket);
}

void CNetServer::KickPlayer(un::EntityId const nPlayer, bool const bSendDisconnectionNotification,
                            ePacketPriority nPacketPriority) {
    m_pRakPeer->CloseConnection(m_pRakPeer->GetSystemAddressFromIndex(nPlayer), bSendDisconnectionNotification, 0,
                                static_cast<PacketPriority>(nPacketPriority));
}

CPlayerSocket *CNetServer::GetPlayerSocket(un::EntityId const nPlayer) {
    for (auto itPlayerSocket = m_aPlayerSockets.Begin(); itPlayerSocket != m_aPlayerSockets.End(); ++itPlayerSocket) {
        if (CPlayerSocket *pPlayerSocket{*itPlayerSocket}; pPlayerSocket->GetPlayerId() == nPlayer) {
            return pPlayerSocket;
        }
    }

    return nullptr;
}
