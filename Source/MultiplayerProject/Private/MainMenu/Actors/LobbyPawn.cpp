// Copyright Robert Uszynski

/* Class header. */
#include "MainMenu/Actors/LobbyPawn.h"

/* Project includes. */
#include "Core/Settings/MultiplayerSettings.h"
#include "MainMenu/UI/LobbyWidget.h"
#include "Shared/Libraries/Logging.h"
#include "Shared/Libraries/MultiplayerLibrary.h"
#include "Shared/Subsystems/SessionSubsystem.h"
#include "Shared/Subsystems/UIManager.h"

/* Engine includes. */
#include "Camera/CameraComponent.h"

ALobbyPawn::ALobbyPawn()
{
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	RootComponent = Camera;
}

void ALobbyPawn::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		const FOnlineSessionSettings Settings = USessionSubsystem::GetSessionSettings(this);
		const int32 MaxNumPlayers = Settings.NumPublicConnections;
		const int32 CurrentNumPlayers = UMultiplayerLibrary::GetNumPlayers(this);
		RequestLoadLobbyWidget(CurrentNumPlayers, MaxNumPlayers);
	}
}

void ALobbyPawn::RequestLoadLobbyWidget_Implementation(int32 CurrentNumPlayers, int32 MaxNumPlayers)
{
	UUIManager::LoadViewportWidget(this, EViewportWidget::Lobby, FOnWidgetLoaded::CreateLambda([CurrentNumPlayers, MaxNumPlayers](UUserWidget* UserWidget)
	{
		if (ULobbyWidget* LobbyWidget = Cast<ULobbyWidget>(UserWidget))
		{
			LobbyWidget->RefreshLobbyWidgetDisplay(CurrentNumPlayers, MaxNumPlayers);
		}
	}));
}

void ALobbyPawn::OnPlayerConnected_Implementation(APlayerController* NewController, int32 CurrentNumPlayers, int32 MaxNumPlayers)
{
	/*
	if (HasAuthority())
	{
		if (ULobbyWidget* LobbyWidget = Cast<ULobbyWidget>(UUIManager::GetLoadedWidget(this, EViewportWidget::Lobby)))
		{
			LobbyWidget->RefreshLobbyWidgetDisplay();
		}
	}
	else*/
	if (UMultiplayerSettings::GetEnableOptionalLogging())
	{
		ULogging::LogMessageToConsole(FString::Printf(TEXT("%s connected... (%d/%d players)"), *NewController->GetName(), CurrentNumPlayers, MaxNumPlayers));
	}
	
	if (HasAuthority())
	{
		RequestLobbyWidgetUpdate(CurrentNumPlayers, MaxNumPlayers);
	}
}

void ALobbyPawn::RequestLobbyWidgetUpdate_Implementation(int32 CurrentNumPlayers, int32 MaxNumPlayers)
{
	if (ULobbyWidget* LobbyWidget = Cast<ULobbyWidget>(UUIManager::GetLoadedWidget(this, EViewportWidget::Lobby)))
	{
		LobbyWidget->RefreshLobbyWidgetDisplay(CurrentNumPlayers, MaxNumPlayers);
	}
}
