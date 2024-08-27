#include "CNCharacterBase.h"

#include "Components/CapsuleComponent.h"

ACNCharacterBase::ACNCharacterBase(FObjectInitializer const& ObjectInitializer)
	/*: Super(ObjectInitializer.SetDefaultSubobjectClass<UPawnMovementComponent>(CharacterMovementComponentName))*/
{
	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);

	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimBPClass(
		TEXT("/Game/Gameplay/Character/ABP_CNManny.ABP_CNManny_C"));
	if (AnimBPClass.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(AnimBPClass.Class);
	}

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(
		TEXT("/Game/Assets/Manny/Meshes/SKM_Manny.SKM_Manny"));
	if (MeshAsset.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(MeshAsset.Object);
	}

	GetMesh()->SetRelativeLocation({0.0f, 0.0f, -90.0f});
	GetMesh()->SetRelativeRotation({0.0f, -90.0f, 0.0f});
}
