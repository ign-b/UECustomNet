#pragma once

#include "RpcHandler.h"

class CBitStream;
class CPlayerSocket;

class CServerRPCHandler : public CRpcHandler {
protected:
    static void CharacterRequestSpawn(CBitStream *pBitStream, CPlayerSocket const *pSenderSocket);

public:
    void Register();

    void Unregister();
};
