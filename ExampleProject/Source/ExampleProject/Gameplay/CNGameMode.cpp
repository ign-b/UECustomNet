#include "CNGameMode.h"

#include "ExampleProject/Network/ClientApp.h"

#include "UECustomNet/Containers/String.h"
#include "ExampleProject/Character/CNCharacterLocal.h"

CClientApp* ACNGameMode::ClientApp = nullptr;

ACNGameMode::ACNGameMode()
{
	DefaultPawnClass = ACNCharacterLocal::StaticClass();

	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;
}

void ACNGameMode::BeginPlay()
{
	Super::BeginPlay();

	ClientApp = CClientApp::GetInstance();
	ClientApp->Startup("127.0.0.1", 17778, {});
}

void ACNGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	//ClientApp->Shutdown();
}

void ACNGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (ClientApp)
	{
		ClientApp->Tick();
	}
}
