#include "RpcHandler.h"

#include "Packet.h"
#include "BitStream.h"
#include "PacketIdentifiers.h"

CRpcHandler::~CRpcHandler() {
    for (auto itRPCData = m_aRpcDatas.Begin(); itRPCData != m_aRpcDatas.End(); ++itRPCData) {
        delete *itRPCData;
    }
}

void CRpcHandler::Add(un::RpcIdentifier const nRpc, un::TRpcCallback const &fnCallback) {
    SRpcData *pRpcData = GetRpcData(nRpc);

    if (pRpcData) {
        return;
    }

    pRpcData = new SRpcData{};
    pRpcData->nRpc = nRpc;
    pRpcData->fnCallback = fnCallback;

    m_aRpcDatas.PushBack(pRpcData);
}

void CRpcHandler::Remove(un::RpcIdentifier const nRpc) {
    SRpcData *pRpcData = GetRpcData(nRpc);

    if (!pRpcData) {
        return;
    }

    m_aRpcDatas.Remove(pRpcData);

    delete pRpcData;
}

SRpcData *CRpcHandler::GetRpcData(un::RpcIdentifier const nRpc) {
    for (auto itRPCData = m_aRpcDatas.Begin(); itRPCData != m_aRpcDatas.End(); ++itRPCData) {
        if ((*itRPCData)->nRpc == nRpc) {
            return *itRPCData;
        }
    }

    return nullptr;
}

bool CRpcHandler::HandlePacket(CPacket const *pPacket) {
    if (pPacket->nPacket == PACKET_RPC) {
        CBitStream BitStream{pPacket->pData, pPacket->nLength, false};

        if (un::RpcIdentifier nRpc = RPC_INVALID; BitStream.Read(nRpc)) {
            if (SRpcData const *pFunction = GetRpcData(nRpc)) {
                pFunction->fnCallback(&BitStream, pPacket->pPlayerSocket);
                return true;
            }
        }
    }

    return false;
}
