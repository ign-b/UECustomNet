#pragma once

#include "PacketHandler.h"

class CBitStream;
class CPlayerSocket;

class CServerPacketHandler : public CPacketHandler {
protected:
    static void NewConnection(CBitStream *pBitStream, CPlayerSocket const *pSenderSocket);

    static void Disconnected(CBitStream *pBitStream, CPlayerSocket const *pSenderSocket);

    static void LostConnection(CBitStream *pBitStream, CPlayerSocket const *pSenderSocket);

    static void CharacterOnfootData(CBitStream *pBitStream, CPlayerSocket const *pSenderSocket);

public:
    void Register();

    void Unregister();
};
