// Copyright Robert Uszynski

/* Class header. */
#include "MainMenu/UI/LobbyWidget.h"

/* Project includes. */
#include "Core/Actors/MultiplayerGameMode.h"
#include "Core/Settings/MultiplayerSettings.h"
#include "MainMenu/UI/LobbyPlayerDisplay.h"
#include "Shared/Libraries/Logging.h"
#include "Shared/Libraries/MultiplayerLibrary.h"

/* Engine includes. */
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"

void ULobbyWidget::RefreshLobbyWidgetDisplay(int32 CurrentNumPlayers, int32 MaxNumPlayers)
{
	if (IsValid(GetWorld()))
	{
		if (AGameStateBase* GameState = GetWorld()->GetGameState<AGameStateBase>(); IsValid(GameState))
		{
			CurrentNumPlayers = GameState->PlayerArray.Num();

			if (IsValid(PlayerNamesVerticalBox) && LobbyPlayerDisplayClass.Get() != nullptr)
			{
				PlayerNamesVerticalBox->ClearChildren();
				for (const APlayerState* PlayerState : GameState->PlayerArray)
				{
					FString PlayerNameString = PlayerState->GetPlayerName();
					if (PlayerNameString.Len() > UMultiplayerSettings::GetMaxSessionNameLength())
					{
						PlayerNameString = PlayerNameString.Left(UMultiplayerSettings::GetMaxSessionNameLength()) + "(...)";
					}
					
					const FText PlayerName = FText::FromString(PlayerNameString);
					if (ULobbyPlayerDisplay* NewPlayerDisplay = ULobbyPlayerDisplay::CreateLobbyPlayerDisplay(this, LobbyPlayerDisplayClass, PlayerName); IsValid(NewPlayerDisplay))
					{
						PlayerNamesVerticalBox->AddChildToVerticalBox(NewPlayerDisplay);
					}
				}
			}
		}
	}

	if (IsValid(CurrentNumPlayersText))
	{
		CurrentNumPlayersText->SetText(FText::AsNumber(CurrentNumPlayers));
	}

	if (IsValid(MaxNumPlayersText))
	{
		MaxNumPlayersText->SetText(FText::AsNumber(MaxNumPlayers));
	}

	if (const APlayerController* LocalController = UMultiplayerLibrary::GetLocalPlayerController(this); IsValid(LocalController) && LocalController->HasAuthority())
	{
		if (CurrentNumPlayers == MaxNumPlayers && IsValid(StartGameButton))
		{
			StartGameButton->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void ULobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(StartGameButton))
	{
		StartGameButton->SetVisibility(ESlateVisibility::Hidden);
		StartGameButton->OnClicked.AddDynamic(this, &ULobbyWidget::OnStartGameButtonClicked);
	}
}

void ULobbyWidget::OnStartGameButtonClicked()
{
	if (const APlayerController* LocalController = UMultiplayerLibrary::GetLocalPlayerController(this); IsValid(LocalController) && LocalController->HasAuthority())
	{
		if (const AMultiplayerGameMode* MultiplayerGameMode = AMultiplayerGameMode::GetMultiplayerGameMode(this))
		{
			MultiplayerGameMode->BeginGame();
		}
		else
		{
			ULogging::LogVerboseError(GetName(), "ULobbyWidget::OnStartGameButtonClicked", "Game Mode is NOT of type AMultiplayerGameMode. Cannot start game!");
		}
	}
}
