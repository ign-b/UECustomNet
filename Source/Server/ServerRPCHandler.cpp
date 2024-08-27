#include "ServerRPCHandler.h"

#include "Main.h"
#include "BitStream.h"
#include "PlayerSocket.h"
#include "CharacterFlow.h"

void CServerRPCHandler::CharacterRequestSpawn(CBitStream *pBitStream, CPlayerSocket const *pSenderSocket) {
    auto const nPlayer = pSenderSocket->GetPlayerId();

    SPlacementContext PlacementContext{};
    pBitStream->Read(PlacementContext);

    auto *pCharacterFlow = g_pServerApp->GetCharacterFlow();
    pCharacterFlow->UpdateCharacterPosition(nPlayer, PlacementContext, {});

    printf("[Server:Debug] Character %i spawned\n", nPlayer);
}

void CServerRPCHandler::Register() {
    Add(RPC_CHARACTER_REQUEST_SPAWN, CharacterRequestSpawn);
}

void CServerRPCHandler::Unregister() {
    Remove(RPC_CHARACTER_REQUEST_SPAWN);
}
