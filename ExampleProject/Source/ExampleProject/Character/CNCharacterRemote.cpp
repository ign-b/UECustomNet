#include "CNCharacterRemote.h"

#include "ExampleProject/Character/CNCharacterAnimInstance.h"

#include "UECustomNet/NetTypes.h"

ACNCharacterRemote::ACNCharacterRemote(FObjectInitializer const& ObjectInitializer)
	: Super(ObjectInitializer)
{
	TargetLocation = FVector::ZeroVector;
	TargetRotation = FQuat::Identity;
}

void ACNCharacterRemote::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	auto constexpr LocationInterpFactor = 0.3f;
	auto constexpr LocationErrorTolerance = 0.01f;
	auto constexpr LocationDeadzone = 10.0f;

	auto constexpr RotationInterpFactor = 0.2f;
	auto constexpr RotationErrorTolerance = 0.01f;
	auto constexpr RotationDeadzone = 90.0f * (UE_PI / 180.f);

	auto constexpr VelocityErrorTolerance = 0.01f;

	auto const CurrentLocation = GetActorLocation();
	bool const bShouldUpdateLocation = !CurrentLocation.Equals(TargetLocation, LocationErrorTolerance);
	auto const CurrentRotation = GetActorQuat();
	bool const bShouldUpdateRotation = !CurrentRotation.Equals(TargetRotation, RotationErrorTolerance);

	if (bShouldUpdateLocation)
	{
		if (TargetVelocity.Length() > VelocityErrorTolerance)
		{
			auto CalculateMovingLerp = [](
				float const CurrentPosition,
				float const TargetPosition,
				float const Velocity,
				float const DeltaSeconds
			) -> float
			{
				float const DistanceToTarget = FMath::Abs(TargetPosition - CurrentPosition);
				float const ClampedVelocity = FMath::Max(FMath::Abs(Velocity), VelocityErrorTolerance);
				float const TimeToReachTarget = DistanceToTarget / ClampedVelocity;
				float const Alpha = FMath::Clamp(DeltaSeconds / TimeToReachTarget, 0.0f, 1.0f);
				return FMath::Lerp(CurrentPosition, TargetPosition, Alpha);
			};

			SetActorLocation({
				CalculateMovingLerp(CurrentLocation.X, TargetLocation.X, TargetVelocity.X, DeltaSeconds),
				CalculateMovingLerp(CurrentLocation.Y, TargetLocation.Y, TargetVelocity.Y, DeltaSeconds),
				CalculateMovingLerp(CurrentLocation.Z, TargetLocation.Z, TargetVelocity.Z, DeltaSeconds)
			});
		}
		else
		{
			float const DistanceToTarget = FVector::Dist(CurrentLocation, TargetLocation);
			if (DistanceToTarget <= LocationDeadzone)
			{
				auto const InterpSpeed = DistanceToTarget * LocationInterpFactor;
				auto const IntermediateLocation{
					FMath::VInterpTo(CurrentLocation, TargetLocation, DeltaSeconds, InterpSpeed)
				};

				SetActorLocation(IntermediateLocation);
			}
			else
			{
				SetActorLocation(TargetLocation);
			}
		}
	}

	if (bShouldUpdateRotation)
	{
		float const DistanceToTarget = CurrentRotation.AngularDistance(TargetRotation);
		if (DistanceToTarget <= RotationDeadzone)
		{
			auto const IntermediateRotation{FQuat::Slerp(GetActorQuat(), TargetRotation, RotationInterpFactor)};

			SetActorRotation(IntermediateRotation);
		}
		else
		{
			SetActorRotation(TargetRotation);
		}
	}
}

void ACNCharacterRemote::SetPlacementContext(SPlacementContext const& PlacementContext)
{
	TargetLocation = {
		PlacementContext.vecPosition.GetX(),
		PlacementContext.vecPosition.GetY(),
		PlacementContext.vecPosition.GetZ()
	};

	TargetRotation = {
		PlacementContext.quatRotation.GetX(),
		PlacementContext.quatRotation.GetY(),
		PlacementContext.quatRotation.GetZ(),
		PlacementContext.quatRotation.GetW()
	};
}

void ACNCharacterRemote::SetMovementContext(SMovementContext const& MovementContext)
{
	TargetVelocity = {
		MovementContext.vecVelocity.GetX(),
		MovementContext.vecVelocity.GetY(),
		MovementContext.vecVelocity.GetZ()
	};

	auto* AnimInstance = Cast<UCNCharacterAnimInstance>(GetMesh()->GetAnimInstance());
	if (IsValid(AnimInstance))
	{
		AnimInstance->SetMovementContext(MovementContext);
	}
}
