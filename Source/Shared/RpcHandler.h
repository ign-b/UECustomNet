#pragma once

#include <functional>

#include "Common.h"
#include "Containers/List.h"
#include "RpcIdentifiers.h"

class CBitStream;
class CPlayerSocket;
class CPacket;

namespace un {
    using TRpcCallback = std::function<void(CBitStream *, CPlayerSocket const*)>;
};

struct SRpcData {
    un::RpcIdentifier nRpc;
    un::TRpcCallback fnCallback;
};

class UNET_API CRpcHandler {
protected:
    CList<SRpcData *> m_aRpcDatas;

public:
    ~CRpcHandler();

    void Add(un::RpcIdentifier nRpc, un::TRpcCallback const &fnCallback);

    void Remove(un::RpcIdentifier nRpc);

    SRpcData *GetRpcData(un::RpcIdentifier nRpc);

    bool HandlePacket(const CPacket *pPacket);
};
