#include "PacketHandler.h"

#include "BitStream.h"
#include "Packet.h"

CPacketHandler::~CPacketHandler() {
    for (auto itPacketData = m_aPacketDatas.Begin(); itPacketData != m_aPacketDatas.End(); ++itPacketData) {
        delete *itPacketData;
    }
}

void CPacketHandler::Add(un::PacketIdentifier const nPacket, un::TPacketCallback const &fnCallback) {
    SPacketData *pPacketData = GetPacketData(nPacket);

    if (pPacketData) {
        return;
    }

    pPacketData = new SPacketData{};
    pPacketData->nPacket = nPacket;
    pPacketData->fnCallback = fnCallback;

    m_aPacketDatas.PushBack(pPacketData);
}

void CPacketHandler::Remove(un::PacketIdentifier nPacket) {
    SPacketData *pPacketData = GetPacketData(nPacket);

    if (!pPacketData) {
        return;
    }

    m_aPacketDatas.Remove(pPacketData);

    delete pPacketData;
}

SPacketData *CPacketHandler::GetPacketData(un::PacketIdentifier const nPacket) {
    for (auto itPacketData = m_aPacketDatas.Begin(); itPacketData != m_aPacketDatas.End(); ++itPacketData) {
        if ((*itPacketData)->nPacket == nPacket) {
            return *itPacketData;
        }
    }

    return nullptr;
}

bool CPacketHandler::HandlePacket(CPacket const *pPacket) {
    if (SPacketData const *pFunction = GetPacketData(pPacket->nPacket)) {
        CBitStream BitStream{pPacket->pData, pPacket->nLength, false};
        pFunction->fnCallback(&BitStream, pPacket->pPlayerSocket);
        return true;
    }

    return false;
}
