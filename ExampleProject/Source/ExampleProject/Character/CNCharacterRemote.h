#pragma once

#include "CNCharacterBase.h"

#include "CNCharacterRemote.generated.h"

UCLASS(Blueprintable)
class ACNCharacterRemote : public ACNCharacterBase
{
	GENERATED_BODY()

protected:
	FVector TargetLocation;
	FVector TargetVelocity;
	FQuat TargetRotation;

public:
	ACNCharacterRemote(FObjectInitializer const& ObjectInitializer);

protected:
	virtual void Tick(float DeltaSeconds) override;

public:
	void SetPlacementContext(SPlacementContext const& PlacementContext);
	void SetMovementContext(SMovementContext const& MovementContext);
};
