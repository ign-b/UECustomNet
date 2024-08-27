#include "ClientPacketHandler.h"

#include "Kismet/GameplayStatics.h"
#include "ExampleProject/Character/CNCharacterLocal.h"
#include "ExampleProject/Character/CNCharacterRemote.h"
#include "ExampleProject/Gameplay/CNGameMode.h"
#include "ExampleProject/Network/ClientApp.h"

#include "UECustomNet/BitStream.h"
#include "UECustomNet/NetClientInterface.h"
#include "UECustomNet/NetTypes.h"
#include "UECustomNet/PacketIdentifiers.h"
#include "UECustomNet/PlayerSocket.h"

void CClientPacketHandler::ConnectionRejected(CBitStream*, CPlayerSocket const*) {
    UE_LOG(LogTemp, Warning, TEXT("[Client:Startup] Connection rejected"));
}

void CClientPacketHandler::ConnectionSucceeded(CBitStream*, CPlayerSocket const*) {
    UE_LOG(LogTemp, Warning, TEXT("[Client:Startup] Connection succeded!"));
	
	auto const* GameWorld = GEngine->GameViewport->GetWorld();
	auto const* GameLocalCharacter =
		Cast<ACNCharacterLocal>(UGameplayStatics::GetPlayerCharacter(GameWorld, 0));

	SPlacementContext LocalPlacementContext{};
	GameLocalCharacter->GetPlacementContext(LocalPlacementContext);

	CBitStream BitStream{};
	BitStream.Write(LocalPlacementContext);

	auto* pNetClient = ACNGameMode::ClientApp->GetNetClient();
	pNetClient->RPC(RPC_CHARACTER_REQUEST_SPAWN, &BitStream, PRIORITY_MEDIUM, RELIABILITY_RELIABLE_ORDERED);
}

void CClientPacketHandler::ConnectionFailed(CBitStream*, CPlayerSocket const*) {
    UE_LOG(LogTemp, Warning, TEXT("[Client:Startup] Connection failed"));
}

void CClientPacketHandler::AlreadyConnected(CBitStream*, CPlayerSocket const*) {
    UE_LOG(LogTemp, Warning, TEXT("[Client:Startup] You are already connected"));
}

void CClientPacketHandler::ServerFull(CBitStream*, CPlayerSocket const*) {
    UE_LOG(LogTemp, Warning, TEXT("[Client:Startup] There are no available slots to join the server"));
}

void CClientPacketHandler::Disconnected(CBitStream*, CPlayerSocket const*) {
    UE_LOG(LogTemp, Warning, TEXT("[Client:Startup] The server has disconnected you"));
}

void CClientPacketHandler::LostConnection(CBitStream*, CPlayerSocket const*) {
    UE_LOG(LogTemp, Warning, TEXT("[Client:Startup] The connection was interrupted due to technical reasons"));
}

void CClientPacketHandler::Banned(CBitStream*, CPlayerSocket const*) {
    UE_LOG(LogTemp, Warning, TEXT("[Client:Startup] You are on the servers block list"));
}

void CClientPacketHandler::PasswordInvalid(CBitStream*, CPlayerSocket const*) {
    UE_LOG(LogTemp, Warning, TEXT("[Client:Startup] The servers password is incorrect"));
}

void CClientPacketHandler::CharacterOnfootData(CBitStream* pBitStream, CPlayerSocket const*) {
	un::EntityId RemotePlayer{};
	SPlacementContext RemotePlacementContext{};
	SMovementContext RemoteMovementContext{};
	pBitStream->Read(RemotePlayer);
	pBitStream->Read(RemotePlacementContext);
	pBitStream->Read(RemoteMovementContext);

	auto* CharactersStore = ACNGameMode::ClientApp->GetCharactersStore();
	
	auto* RemoteData = CharactersStore->Get(RemotePlayer);
	if (!RemoteData) {
		return;
	}
	
	auto* RemoteWorldCharacter{RemoteData->GetWorldCharacter()};
	if (IsValid(RemoteWorldCharacter)) {
		RemoteWorldCharacter->SetPlacementContext(RemotePlacementContext);
		RemoteWorldCharacter->SetMovementContext(RemoteMovementContext);
	}
}

void CClientPacketHandler::Register() {
	Add(PACKET_CONNECTION_SUCCEEDED, ConnectionSucceeded);
	Add(PACKET_CONNECTION_FAILED, ConnectionFailed);
	Add(PACKET_ALREADY_CONNECTED, AlreadyConnected);
	Add(PACKET_SERVER_FULL, ServerFull);
	Add(PACKET_DISCONNECTED, Disconnected);
	Add(PACKET_LOST_CONNECTION, LostConnection);
	Add(PACKET_BANNED, Banned);
	Add(PACKET_PASSWORD_INVALID, PasswordInvalid);
	
	Add(PACKET_CHARACTER_ONFOOT_DATA, CharacterOnfootData);
}

void CClientPacketHandler::Unregister() {
	Remove(PACKET_CONNECTION_SUCCEEDED);
	Remove(PACKET_CONNECTION_FAILED);
	Remove(PACKET_ALREADY_CONNECTED);
	Remove(PACKET_SERVER_FULL);
	Remove(PACKET_DISCONNECTED);
	Remove(PACKET_LOST_CONNECTION);
	Remove(PACKET_BANNED);
	Remove(PACKET_PASSWORD_INVALID);
	
	Remove(PACKET_CHARACTER_ONFOOT_DATA);
}
