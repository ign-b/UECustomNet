#include "LocalCharacter.h"

#include "Kismet/GameplayStatics.h"
#include "ExampleProject/Character/CNCharacterLocal.h"
#include "ExampleProject/Gameplay/CNGameMode.h"
#include "ExampleProject/Network/ClientApp.h"

#include "UECustomNet/BitStream.h"
#include "UECustomNet/NetClientInterface.h"
#include "UECustomNet/Packet.h"
#include "UECustomNet/PacketIdentifiers.h"

clk::time_point CLocalCharacter::LastSyncTime{};

void CLocalCharacter::Process() {
	auto* pNetClient = ACNGameMode::ClientApp->GetNetClient();
	if(!pNetClient->IsConnected()) {
		return;
	}

	auto const NowClockTime = clk::now();
	auto const nTimeSinceLastBroadcast = std::chrono::duration_cast<std::chrono::milliseconds>
		(NowClockTime - LastSyncTime).count();

	if (nTimeSinceLastBroadcast < SYNC_FREQUENCY_MAXIMUM_RATE) {
		return;
	}
	
	auto const* GameWorld = GEngine->GameViewport->GetWorld();
	auto const* GameLocalCharacter = Cast<ACNCharacterLocal>(UGameplayStatics::GetPlayerCharacter(GameWorld, 0));

	SPlacementContext LocalPlacementContext{};
	SMovementContext LocalMovementContext{};
	GameLocalCharacter->GetPlacementContext(LocalPlacementContext);
	GameLocalCharacter->GetMovementContext(LocalMovementContext);
	
	CBitStream BitStream{};
	BitStream.Write(static_cast<un::PacketId>(PACKET_CHARACTER_ONFOOT_DATA));
	BitStream.Write(LocalPlacementContext);
	BitStream.Write(LocalMovementContext);
	
	pNetClient->Send(&BitStream, PRIORITY_LOW, RELIABILITY_UNRELIABLE_SEQUENCED);

	LastSyncTime = NowClockTime;
}
