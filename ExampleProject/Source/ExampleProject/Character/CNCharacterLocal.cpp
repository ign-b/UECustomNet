#include "CNCharacterLocal.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "ExampleProject/Character/CNCharacterAnimInstance.h"

#include "UECustomNet/NetTypes.h"

ACNCharacterLocal::ACNCharacterLocal(FObjectInitializer const& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCharacterMovementComponent>(CharacterMovementComponentName))
{
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 150.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> MappingContextObject(
		TEXT("/Game/Input/IMC_Default.IMC_Default"));
	if (MappingContextObject.Succeeded())
	{
		DefaultMappingContext = MappingContextObject.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction>
		JumpActionObject(TEXT("/Game/Input/Actions/IA_Jump.IA_Jump"));
	if (JumpActionObject.Succeeded())
	{
		JumpAction = JumpActionObject.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction>
		MoveActionObject(TEXT("/Game/Input/Actions/IA_Move.IA_Move"));
	if (MoveActionObject.Succeeded())
	{
		MoveAction = MoveActionObject.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction>
		LookActionObject(TEXT("/Game/Input/Actions/IA_Look.IA_Look"));
	if (LookActionObject.Succeeded())
	{
		LookAction = LookActionObject.Object;
	}
}

void ACNCharacterLocal::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACNCharacterLocal::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACNCharacterLocal::Look);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT( "'%s' Failed to find an Enhanced Input component!"), *GetNameSafe(this));
	}
}

void ACNCharacterLocal::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem
			<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void ACNCharacterLocal::Move(const FInputActionValue& Value)
{
	FVector2D const MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ACNCharacterLocal::Look(const FInputActionValue& Value)
{
	FVector2D const LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ACNCharacterLocal::GetPlacementContext(SPlacementContext& OutPlacementContext) const
{
	auto const& GameLocalTransform{GetTransform()};

	auto const& GameLocalPosition{GameLocalTransform.GetLocation()};
	OutPlacementContext.vecPosition.Set(
		static_cast<float>(GameLocalPosition.X), static_cast<float>(GameLocalPosition.Y),
		static_cast<float>(GameLocalPosition.Z)
	);

	auto const& GameLocalRotation{GameLocalTransform.GetRotation()};
	OutPlacementContext.quatRotation.Set(
		static_cast<float>(GameLocalRotation.W), static_cast<float>(GameLocalRotation.X),
		static_cast<float>(GameLocalRotation.Y), static_cast<float>(GameLocalRotation.Z)
	);
}

void ACNCharacterLocal::GetMovementContext(SMovementContext& OutMovementContext) const
{
	auto* AnimInstance = Cast<UCNCharacterAnimInstance>(GetMesh()->GetAnimInstance());
	if (IsValid(AnimInstance))
	{
		AnimInstance->GetMovementContext(OutMovementContext);
	}
}
