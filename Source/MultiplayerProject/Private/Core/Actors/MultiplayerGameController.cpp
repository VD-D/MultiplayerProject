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
	}
	else
	{
		ULogging::LogVerboseError(GetName(), "AMultiplayerGameController::SetupInputComponent", "Failed to get EnhancedInputLocalPlayerSubsystem!");
	}
}
