#pragma once

#include "UECustomNet/PacketHandler.h"

class CClientPacketHandler : public CPacketHandler
{
private:
	static void ConnectionRejected(CBitStream* pBitStream, CPlayerSocket const* pSenderSocket);
	static void ConnectionSucceeded(CBitStream* pBitStream, CPlayerSocket const* pSenderSocket);
	static void ConnectionFailed(CBitStream* pBitStream, CPlayerSocket const* pSenderSocket);
	static void AlreadyConnected(CBitStream* pBitStream, CPlayerSocket const* pSenderSocket);
	static void ServerFull(CBitStream* pBitStream, CPlayerSocket const* pSenderSocket);
	static void Disconnected(CBitStream* pBitStream, CPlayerSocket const* pSenderSocket);
	static void LostConnection(CBitStream* pBitStream, CPlayerSocket const* pSenderSocket);
	static void Banned(CBitStream* pBitStream, CPlayerSocket const* pSenderSocket);
	static void PasswordInvalid(CBitStream* pBitStream, CPlayerSocket const* pSenderSocket);
	
	static void CharacterOnfootData(CBitStream* pBitStream, CPlayerSocket const* pSenderSocket);

public:
	void Register();
	void Unregister();
};
