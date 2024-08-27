#include "CNCharacterAnimInstance.h"

#include "CNCharacterLocal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "UECustomNet/NetTypes.h"

void UCNCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Character = nullptr;
	MovementComponent = nullptr;

	AActor* OwningActor = GetOwningActor();
	if (!OwningActor)
	{
		return;
	}

	IsLocal = OwningActor->IsA<ACNCharacterLocal>();

	if (IsLocal)
	{
		ACharacter* OwningCharacter = Cast<ACNCharacterLocal>(OwningActor);
		Character = OwningCharacter;
		MovementComponent = OwningCharacter->GetCharacterMovement();
	}
}

void UCNCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (IsLocal)
	{
		if (IsValid(Character) &&
			IsValid(MovementComponent))
		{
			Velocity = MovementComponent->Velocity;
			GroundSpeed = UKismetMathLibrary::VSizeXY(Velocity);
			ShouldMove = GroundSpeed > 3.0f && !MovementComponent->GetCurrentAcceleration().IsZero();
			IsFalling = MovementComponent->IsFalling();
		}
	}
}

void UCNCharacterAnimInstance::GetMovementContext(SMovementContext& OutMovementContext) const
{
	OutMovementContext.vecVelocity.Set(Velocity.X, Velocity.Y, Velocity.Z);
	OutMovementContext.fGroundSpeed = GroundSpeed;
	OutMovementContext.bShouldMove = ShouldMove;
	OutMovementContext.bIsFalling = IsFalling;
}

void UCNCharacterAnimInstance::SetMovementContext(SMovementContext const& MovementContext)
{
	Velocity.Set(
		MovementContext.vecVelocity.GetX(),
		MovementContext.vecVelocity.GetY(),
		MovementContext.vecVelocity.GetZ()
	);
	GroundSpeed = MovementContext.fGroundSpeed;
	ShouldMove = MovementContext.bShouldMove;
	IsFalling = MovementContext.bIsFalling;
}
