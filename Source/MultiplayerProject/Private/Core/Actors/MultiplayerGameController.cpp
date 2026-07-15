// Copyright Robert Uszynski

/* Class header. */
#include "Core/Actors/MultiplayerGameController.h"

/* Project includes. */
#include "Shared/Libraries/Logging.h"

/* Engine includes. */
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

void AMultiplayerGameController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalController()) return;

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(InputMappingContext, 0);

		if (UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(InputComponent))
		{
			Input->BindAction(MovementInputAction, ETriggerEvent::Triggered, this, &AMultiplayerGameController::MovementInput);
			Input->BindAction(CameraInputAction, ETriggerEvent::Triggered, this, &AMultiplayerGameController::CameraLook);

			for (const auto& Mapping : AbilityActions)
			{
				Input->BindAction(Mapping.InputAction, ETriggerEvent::Started, this, &AMultiplayerGameController::OnAbilityInputPressed, Mapping.InputID);
				Input->BindAction(Mapping.InputAction, ETriggerEvent::Completed, this, &AMultiplayerGameController::OnAbilityInputReleased, Mapping.InputID);
			}
		}
		else
		{
			ULogging::LogVerboseError(GetName(), "AMultiplayerGameController::SetupInputComponent", "Failed to cast input component to UEnhancedInputComponent!");
		}
	}
	else
	{
		ULogging::LogVerboseError(GetName(), "AMultiplayerGameController::SetupInputComponent", "Failed to get EnhancedInputLocalPlayerSubsystem!");
	}
}

void AMultiplayerGameController::MovementInput(const FInputActionInstance& Instance)
{
	if (APawn* ControlledPawn = GetPawn())
	{
		const FRotator ThisControlRotation = GetControlRotation();
		const FVector2D AxisValue = Instance.GetValue().Get<FVector2D>();
	
		ControlledPawn->AddMovementInput(FRotationMatrix(FRotator(0.0f, ThisControlRotation.Yaw, ThisControlRotation.Roll)).GetScaledAxis(EAxis::Y), AxisValue.X);
		ControlledPawn->AddMovementInput(FRotationMatrix(FRotator(0.0f, ThisControlRotation.Yaw, 0.0f)).GetScaledAxis(EAxis::X), AxisValue.Y);
	}
}

void AMultiplayerGameController::CameraLook(const FInputActionInstance& Instance)
{
	if (APawn* ControlledPawn = GetPawn())
	{
		const FVector2D AxisValue = Instance.GetValue().Get<FVector2D>();
		ControlledPawn->AddControllerYawInput(AxisValue.X);
		ControlledPawn->AddControllerPitchInput(AxisValue.Y);
	}
}

void AMultiplayerGameController::OnAbilityInputPressed(int32 InputID)
{
	if (UAbilitySystemComponent* AbilityComp = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetPawn()); IsValid(AbilityComp))
	{
		AbilityComp->AbilityLocalInputPressed(InputID);
	}
}

void AMultiplayerGameController::OnAbilityInputReleased(int32 InputID)
{
	if (UAbilitySystemComponent* AbilityComp = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetPawn()); IsValid(AbilityComp))
	{
		AbilityComp->AbilityLocalInputReleased(InputID);
	}
}
