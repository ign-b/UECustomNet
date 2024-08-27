#pragma once

#include "UECustomNet/RpcHandler.h"

class CClientRPCHandler : public CRpcHandler
{
protected:
	static void CharacterWorldAdd(CBitStream* pBitStream, CPlayerSocket const* pSenderSocket);
	static void CharacterWorldRemove(CBitStream* pBitStream, CPlayerSocket const* pSenderSocket);

public:
	void Register();
	void Unregister();
};
