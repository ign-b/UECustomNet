#include "ClientRPCHandler.h"

#include "ExampleProject/Gameplay/CNGameMode.h"
#include "ExampleProject/Network/ClientApp.h"

#include "UECustomNet/BitStream.h"
#include "UECustomNet/NetTypes.h"
#include "UECustomNet/PlayerSocket.h"

void CClientRPCHandler::CharacterWorldAdd(CBitStream* pBitStream, CPlayerSocket const*) {
	un::EntityId nRemotePlayer{};
	SPlacementContext RemotePlacementContext{};
	pBitStream->Read(nRemotePlayer);
	pBitStream->Read(RemotePlacementContext);

	auto* CharactersStore = ACNGameMode::ClientApp->GetCharactersStore();
	CharactersStore->Add(nRemotePlayer, new CRemotePlayer{RemotePlacementContext});
}

void CClientRPCHandler::CharacterWorldRemove(CBitStream* pBitStream, CPlayerSocket const*) {
	un::EntityId nRemotePlayer{};
	pBitStream->Read(nRemotePlayer);

	auto* CharactersStore = ACNGameMode::ClientApp->GetCharactersStore();

	auto* pRemoteCharacter = CharactersStore->Get(nRemotePlayer);
	if(pRemoteCharacter) {
		delete pRemoteCharacter;
	}
	
	CharactersStore->Remove(nRemotePlayer);
}

void CClientRPCHandler::Register() {
	Add(RPC_CHARACTER_WORLD_ADD, CharacterWorldAdd);
	Add(RPC_CHARACTER_WORLD_REMOVE, CharacterWorldRemove);
}

void CClientRPCHandler::Unregister() {
	Remove(RPC_CHARACTER_WORLD_ADD);
	Remove(RPC_CHARACTER_WORLD_REMOVE);
}
