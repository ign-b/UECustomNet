#include "RemoteCharacter.h"

#include "ExampleProject/Character/CNCharacterRemote.h"

CRemotePlayer::CRemotePlayer(SPlacementContext const& PlacementContext) {
	auto* GameWorld = GEngine->GameViewport->GetWorld();

	FVector const RemoteGamePosition{
		PlacementContext.vecPosition.GetX(),
		PlacementContext.vecPosition.GetY(),
		PlacementContext.vecPosition.GetZ()
	};

	FQuat const RemoteGameQuaternion{
		PlacementContext.quatRotation.GetX(),
		PlacementContext.quatRotation.GetY(),
		PlacementContext.quatRotation.GetZ(),
		PlacementContext.quatRotation.GetW()
	};
	
	WorldCharacter = GameWorld->SpawnActor<ACNCharacterRemote>(
		ACNCharacterRemote::StaticClass(), RemoteGamePosition, RemoteGameQuaternion.Rotator(), {});
	
	UE_LOG(LogTemp, Warning, TEXT("[Client:WorldAdd] Character %p created at %s, %s"), WorldCharacter,
		*RemoteGamePosition.ToString(), *RemoteGameQuaternion.ToString());
}

CRemotePlayer::~CRemotePlayer() {
	if (IsValid(WorldCharacter)) {
		WorldCharacter->Destroy();
	}

	UE_LOG(LogTemp, Warning, TEXT("[Client:WorldRemove] Character %p removed"), WorldCharacter);
}
