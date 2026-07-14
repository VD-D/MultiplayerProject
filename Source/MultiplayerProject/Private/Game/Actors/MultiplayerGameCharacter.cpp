// Copyright Robert Uszynski

/* Project includes. */
#include "Game/Actors/MultiplayerGameCharacter.h"

/* Project includes. */
#include "Shared/Libraries/Logging.h"

/* Engine includes. */
#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/SpringArmComponent.h"

AMultiplayerGameCharacter::AMultiplayerGameCharacter()
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>("Spring Arm");
	SpringArm->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>("AbilitySystemComponent");
	
	PrimaryActorTick.bCanEverTick = true;
}

void AMultiplayerGameCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AMultiplayerGameCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (IsValid(AbilitySystemComponent))
	{
		for (const auto& GrantedAbility : AbilityActions)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(GrantedAbility.Ability, 1, GrantedAbility.InputID, this));
		}
	}
}

// Called every frame
void AMultiplayerGameCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMultiplayerGameCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (!IsValid(PlayerInputComponent))
	{
		ULogging::LogVerboseError(GetName(), "AMultiplayerGameCharacter::SetupInputComponent", "Player input component is invalid!");
		return;
	}

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		const FInputModeGameOnly InputMode;
		PlayerController->SetInputMode(InputMode);
		
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputMappingContext, 0);

			if (UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent))
			{
				Input->BindAction(MovementInputAction, ETriggerEvent::Triggered, this, &AMultiplayerGameCharacter::MovementInput);
				Input->BindAction(CameraInputAction, ETriggerEvent::Triggered, this, &AMultiplayerGameCharacter::CameraLook);

				for (const auto& Mapping : AbilityActions)
				{
					Input->BindAction(Mapping.InputAction, ETriggerEvent::Started, this, &AMultiplayerGameCharacter::OnAbilityInputPressed, Mapping.InputID);
					Input->BindAction(Mapping.InputAction, ETriggerEvent::Completed, this, &AMultiplayerGameCharacter::OnAbilityInputReleased, Mapping.InputID);
				}
			}
			else
			{
				ULogging::LogVerboseError(GetName(), "AMultiplayerGameCharacter::SetupInputComponent", "Failed to cast input component to UEnhancedInputComponent!");
			}
		}
		else
		{
			ULogging::LogVerboseError(GetName(), "AMultiplayerGameCharacter::SetupInputComponent", "Failed to get EnhancedInputLocalPlayerSubsystem!");
		}
	}
	else
	{
		ULogging::LogVerboseError(GetName(), "AMultiplayerGameCharacter::SetupInputComponent", "Controller was not a player controller!");
	}
}

void AMultiplayerGameCharacter::MovementInput(const FInputActionInstance& Instance)
{
	const FRotator ControlRotation = IsValid(GetController()) ? GetController()->GetControlRotation() : FRotator::ZeroRotator;
	const FVector2D AxisValue = Instance.GetValue().Get<FVector2D>();
	
	AddMovementInput(FRotationMatrix(FRotator(0.0f, ControlRotation.Yaw, ControlRotation.Roll)).GetScaledAxis(EAxis::Y), AxisValue.X);
	AddMovementInput(FRotationMatrix(FRotator(0.0f, ControlRotation.Yaw, 0.0f)).GetScaledAxis(EAxis::X), AxisValue.Y);
}

void AMultiplayerGameCharacter::CameraLook(const FInputActionInstance& Instance)
{
	const FVector2D AxisValue = Instance.GetValue().Get<FVector2D>();
	AddControllerYawInput(AxisValue.X);
	AddControllerPitchInput(AxisValue.Y);
}

void AMultiplayerGameCharacter::OnAbilityInputPressed(int32 InputID)
{
	if (IsValid(AbilitySystemComponent))
	{
		AbilitySystemComponent->AbilityLocalInputPressed(InputID);
	}
}

void AMultiplayerGameCharacter::OnAbilityInputReleased(int32 InputID)
{
	if (IsValid(AbilitySystemComponent))
	{
		AbilitySystemComponent->AbilityLocalInputReleased(InputID);
	}
}
