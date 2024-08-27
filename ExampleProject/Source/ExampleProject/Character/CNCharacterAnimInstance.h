#pragma once

#include "Animation/AnimInstance.h"

#include "CNCharacterAnimInstance.generated.h"

class UCharacterMovementComponent;
struct SMovementContext;

UCLASS()
class UCNCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	UPROPERTY(Transient, BlueprintReadOnly)
	ACharacter* Character;

	UPROPERTY(Transient, BlueprintReadOnly)
	UCharacterMovementComponent* MovementComponent;

	UPROPERTY(Transient, BlueprintReadOnly)
	bool IsLocal;

	UPROPERTY(Transient, BlueprintReadOnly)
	FVector Velocity;

	UPROPERTY(Transient, BlueprintReadOnly)
	float GroundSpeed;

	UPROPERTY(Transient, BlueprintReadOnly)
	bool ShouldMove;

	UPROPERTY(Transient, BlueprintReadOnly)
	bool IsFalling;

protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

public:
	void GetMovementContext(SMovementContext& OutMovementContext) const;
	void SetMovementContext(SMovementContext const& MovementContext);
};
