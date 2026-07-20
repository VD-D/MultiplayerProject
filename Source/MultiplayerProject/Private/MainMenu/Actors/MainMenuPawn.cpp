// Copyright Robert Uszynski

/* Class header. */
#include "MainMenu/Actors/MainMenuPawn.h"

/* Project includes. */
#include "Core/Settings/MultiplayerSettings.h"
#include "Shared/Subsystems/UIManager.h"

/* Engine includes. */
#include "Camera/CameraComponent.h"
#include "Shared/Subsystems/SessionSubsystem.h"

AMainMenuPawn::AMainMenuPawn()
{
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));;
	RootComponent = Camera;
}

void AMainMenuPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	if (APlayerController* PlayerController = Cast<APlayerController>(NewController))
	{
		UUIManager::LoadViewportWidget(this, EViewportWidget::MainMenu);

		// Note: This is done as a fallback to ensure we have no active session if we were kicked from one (that may have not been destroyed).
		USessionSubsystem::DestroySession(PlayerController);
		
		FInputModeUIOnly InputMode;
		InputMode.SetLockMouseToViewportBehavior(UMultiplayerSettings::GetMouseLockMode());
		
		PlayerController->SetInputMode(InputMode);
		PlayerController->SetShowMouseCursor(true);
	}
}

void AMainMenuPawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UUIManager::RemoveViewportWidget(this, EViewportWidget::MainMenu);
	Super::EndPlay(EndPlayReason);
}
