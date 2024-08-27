#include "ServerPacketHandler.h"

#include "BitStream.h"
#include "CharacterFlow.h"
#include "Main.h"
#include "NetServerInterface.h"
#include "PacketIdentifiers.h"
#include "PlayerSocket.h"

void CServerPacketHandler::NewConnection(CBitStream *, CPlayerSocket const *pSenderSocket) {
    auto const nPlayer = pSenderSocket->GetPlayerId();

    printf("[Server:Enter] %s:%i joined as player %i (serial: %s)\n", *pSenderSocket->GetAddress(),
           pSenderSocket->GetPort(), nPlayer, *pSenderSocket->GetSerial());

    auto *pCharacterFlow = g_pServerApp->GetCharacterFlow();
    pCharacterFlow->AddCharacter(nPlayer, {});
}

void CServerPacketHandler::Disconnected(CBitStream *, CPlayerSocket const *pSenderSocket) {
    auto const nPlayer = pSenderSocket->GetPlayerId();

    printf("[Server:Leave] Player %i has disconnected\n", nPlayer);

    auto *pCharacterFlow = g_pServerApp->GetCharacterFlow();
    pCharacterFlow->RemoveCharacter(nPlayer);
}

void CServerPacketHandler::LostConnection(CBitStream *, CPlayerSocket const *pSenderSocket) {
    auto const nPlayer = pSenderSocket->GetPlayerId();

    printf("[Server:Leave] Player %i has lost connection\n", nPlayer);

    auto *pCharacterFlow = g_pServerApp->GetCharacterFlow();
    pCharacterFlow->RemoveCharacter(nPlayer);
}

void CServerPacketHandler::CharacterOnfootData(CBitStream *pBitStream, CPlayerSocket const *pSenderSocket) {
    auto const nPlayer = pSenderSocket->GetPlayerId();

    SPlacementContext PlacementContext{};
    SMovementContext MovementContext{};
    pBitStream->Read(PlacementContext);
    pBitStream->Read(MovementContext);

    auto *pCharacterFlow = g_pServerApp->GetCharacterFlow();
    pCharacterFlow->UpdateCharacterPosition(nPlayer, PlacementContext, MovementContext);
}

void CServerPacketHandler::Register() {
    Add(PACKET_NEW_CONNECTION, NewConnection);
    Add(PACKET_DISCONNECTED, Disconnected);
    Add(PACKET_LOST_CONNECTION, LostConnection);

    Add(PACKET_CHARACTER_ONFOOT_DATA, CharacterOnfootData);
}

void CServerPacketHandler::Unregister() {
    Remove(PACKET_NEW_CONNECTION);
    Remove(PACKET_DISCONNECTED);
    Remove(PACKET_LOST_CONNECTION);

    Remove(PACKET_CHARACTER_ONFOOT_DATA);
}
