#pragma once

#include "GameFramework/Character.h"

#include "CNCharacterBase.generated.h"

struct SMovementContext;
struct SPlacementContext;

UCLASS(Abstract)
class ACNCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	ACNCharacterBase(FObjectInitializer const& ObjectInitializer);
};
