// Copyright Robert Uszynski

/* Class header. */
#include "MainMenu/Actors/MainMenuPawn.h"

/* Project includes. */
#include "Core/Settings/MultiplayerSettings.h"
#include "Shared/Subsystems/UIManager.h"

/* Engine includes. */
#include "Camera/CameraComponent.h"

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
