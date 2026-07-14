// Copyright Robert Uszynski

/* Class header. */
#include "MainMenu/Actors/LobbyPawn.h"

/* Project includes. */
#include "Core/Actors/MultiplayerGameMode.h"
#include "Core/Settings/MultiplayerSettings.h"
#include "MainMenu/UI/LobbyWidget.h"
#include "Shared/Libraries/Logging.h"
#include "Shared/Libraries/MultiplayerLibrary.h"
#include "Shared/Subsystems/UIManager.h"
#include "Shared/Subsystems/SessionSubsystem.h"

/* Engine includes. */
#include "EngineUtils.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"

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
		if (AMultiplayerGameMode* MultiplayerGameMode = AMultiplayerGameMode::GetMultiplayerGameMode(this); IsValid(MultiplayerGameMode))
		{
			MultiplayerGameMode->OnPlayerConnected.AddDynamic(this, &ALobbyPawn::OnPlayerConnected);
			MultiplayerGameMode->OnPlayerDisconnected.AddDynamic(this, &ALobbyPawn::OnPlayerDisconnected);
		}
	}
}

void ALobbyPawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UUIManager::RemoveViewportWidget(this, EViewportWidget::Lobby);
	Super::EndPlay(EndPlayReason);
}

void ALobbyPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	const FOnlineSessionSettings Settings = USessionSubsystem::GetSessionSettings(this);
	MaximumNumberPlayers = Settings.NumPublicConnections;
	CurrentNumberPlayers = UMultiplayerLibrary::GetNumPlayers(this);

	if (UMultiplayerSettings::GetEnableOptionalLogging())
	{
		ULogging::LogMessageToConsole(FString::Printf(TEXT("ALobby::PossessedBy: found %d/%d players"), CurrentNumberPlayers, MaximumNumberPlayers));
	}
	
	OnPossessedClient();
}

void ALobbyPawn::OnPossessedClient_Implementation()
{
	const int32 CurrentNumPlayers = CurrentNumberPlayers;
	const int32 MaxNumPlayers = MaximumNumberPlayers;
	
	UpdatePlayerNames();
	TArray<FString> CurrentPlayerNames = PlayerNames;
	
	UUIManager::LoadViewportWidget(this, EViewportWidget::Lobby, FOnWidgetLoaded::CreateLambda([CurrentPlayerNames, CurrentNumPlayers, MaxNumPlayers](UUserWidget* UserWidget)
	{
		if (ULobbyWidget* LobbyWidget = Cast<ULobbyWidget>(UserWidget))
		{
			LobbyWidget->RefreshPlayerList(CurrentPlayerNames);
			LobbyWidget->RefreshCurrentNumPlayersDisplay(CurrentNumPlayers);
			LobbyWidget->RefreshMaxNumPlayersDisplay(MaxNumPlayers);
		}
	}));
}

void ALobbyPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALobbyPawn, CurrentNumberPlayers);
	DOREPLIFETIME(ALobbyPawn, MaximumNumberPlayers);
	DOREPLIFETIME(ALobbyPawn, PlayerNames);
}

void ALobbyPawn::OnRep_CurrentNumberPlayers()
{
	if (ULobbyWidget* LobbyWidget = Cast<ULobbyWidget>(UUIManager::GetLoadedWidget(this, EViewportWidget::Lobby)))
	{
		LobbyWidget->RefreshCurrentNumPlayersDisplay(CurrentNumberPlayers);
	}
}

void ALobbyPawn::OnRep_MaxNumberPlayers()
{
	if (ULobbyWidget* LobbyWidget = Cast<ULobbyWidget>(UUIManager::GetLoadedWidget(this, EViewportWidget::Lobby)))
	{
		LobbyWidget->RefreshMaxNumPlayersDisplay(MaximumNumberPlayers);
	}
}

void ALobbyPawn::OnRep_PlayerNames()
{
	if (ULobbyWidget* LobbyWidget = Cast<ULobbyWidget>(UUIManager::GetLoadedWidget(this, EViewportWidget::Lobby)))
	{
		LobbyWidget->RefreshPlayerList(PlayerNames);
	}
}

void ALobbyPawn::UpdatePlayerNames()
{
	TArray<FString> NewPlayerNames;
	for (TActorIterator<ALobbyPawn> ActorIterator(GetWorld()); ActorIterator; ++ActorIterator)
	{
		const ALobbyPawn* LobbyPawn = *ActorIterator;
		if (!IsValid(LobbyPawn)) continue;

		NewPlayerNames.Add(IsValid(LobbyPawn->GetPlayerState()) ? LobbyPawn->GetPlayerState()->GetPlayerName() : FString("INVALID_NAME"));
	}

	PlayerNames = NewPlayerNames;
}

void ALobbyPawn::OnPlayerConnected(APlayerController* NewController, int32 CurrentNumPlayers, int32 MaxNumPlayers)
{
	if (UMultiplayerSettings::GetEnableOptionalLogging())
	{
		ULogging::LogMessageToConsole(FString::Printf(TEXT("%s connected... (%d/%d players)"), *NewController->GetName(), CurrentNumPlayers, MaxNumPlayers));
	}
	
	if (HasAuthority())
	{
		CurrentNumberPlayers = CurrentNumPlayers;
		MaximumNumberPlayers = MaxNumPlayers;
		UpdatePlayerNames();
		OnRep_CurrentNumberPlayers();
		OnRep_MaxNumberPlayers();
		OnRep_PlayerNames();

		if (ULobbyWidget* LobbyWidget = Cast<ULobbyWidget>(UUIManager::GetLoadedWidget(this, EViewportWidget::Lobby)))
		{
			LobbyWidget->SetStartGameButtonVisibility(CurrentNumberPlayers == MaximumNumberPlayers);
		}
	}
}

void ALobbyPawn::OnPlayerDisconnected(APlayerController* OldController, int32 CurrentNumPlayers, int32 MaxNumPlayers)
{
	if (UMultiplayerSettings::GetEnableOptionalLogging())
	{
		ULogging::LogMessageToConsole(FString::Printf(TEXT("%s connected... (%d/%d players)"), *GetNameSafe(OldController), CurrentNumPlayers, MaxNumPlayers));
	}
	
	if (HasAuthority())
	{
		CurrentNumberPlayers = CurrentNumPlayers;
		MaximumNumberPlayers = MaxNumPlayers;
		UpdatePlayerNames();
		OnRep_CurrentNumberPlayers();
		OnRep_MaxNumberPlayers();
		OnRep_PlayerNames();

		if (ULobbyWidget* LobbyWidget = Cast<ULobbyWidget>(UUIManager::GetLoadedWidget(this, EViewportWidget::Lobby)))
		{
			LobbyWidget->SetStartGameButtonVisibility(CurrentNumberPlayers == MaximumNumberPlayers);
		}
	}
}
