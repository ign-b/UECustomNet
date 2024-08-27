#pragma once

#include "GameFramework/GameModeBase.h"

#include "CNGameMode.generated.h"

class CClientApp;

UCLASS(minimalapi)
class ACNGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACNGameMode();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaSeconds) override;

public:
	static CClientApp* ClientApp;
};
