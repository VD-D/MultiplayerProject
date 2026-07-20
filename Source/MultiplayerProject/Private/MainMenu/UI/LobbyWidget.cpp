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
#include "MainMenu/Actors/LobbyPawn.h"

void ULobbyWidget::RefreshPlayerList(const TArray<FString>& PlayerNames)
{
	if (IsValid(PlayerNamesVerticalBox) && LobbyPlayerDisplayClass.Get() != nullptr)
	{
		PlayerNamesVerticalBox->ClearChildren();
		for (const auto& PlayerName : PlayerNames)
		{
			FString PlayerNameString = PlayerName;
			if (PlayerNameString.Len() > UMultiplayerSettings::GetMaxSessionNameLength())
			{
				PlayerNameString = PlayerNameString.Left(UMultiplayerSettings::GetMaxSessionNameLength()) + "(...)";
			}
					
			const FText PlayerNameText = FText::FromString(PlayerNameString);
			if (ULobbyPlayerDisplay* NewPlayerDisplay = ULobbyPlayerDisplay::CreateLobbyPlayerDisplay(this, LobbyPlayerDisplayClass, PlayerNameText); IsValid(NewPlayerDisplay))
			{
				PlayerNamesVerticalBox->AddChildToVerticalBox(NewPlayerDisplay);
			}
		}
	}
}

void ULobbyWidget::RefreshCurrentNumPlayersDisplay(int32 CurrentNumPlayers)
{
	if (IsValid(CurrentNumPlayersText))
	{
		CurrentNumPlayersText->SetText(FText::AsNumber(CurrentNumPlayers));
	}
}

void ULobbyWidget::RefreshMaxNumPlayersDisplay(int32 MaxNumPlayers)
{
	if (IsValid(MaxNumPlayersText))
	{
		MaxNumPlayersText->SetText(FText::AsNumber(MaxNumPlayers));
	}
}

void ULobbyWidget::SetStartGameButtonVisibility(bool bShouldBeVisible)
{
	if (IsValid(StartGameButton))
	{
		StartGameButton->SetVisibility(bShouldBeVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
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

	if (IsValid(LeaveGameButton))
	{
		LeaveGameButton->OnClicked.AddDynamic(this, &ULobbyWidget::OnLeaveGameButtonClicked);
	}

	if (IsValid(PlayerNamesVerticalBox)) // This is to clear anything which may have been added in the editor.
	{
		PlayerNamesVerticalBox->ClearChildren();
	}
}

void ULobbyWidget::OnStartGameButtonClicked()
{
	if (const APlayerController* LocalController = UMultiplayerLibrary::GetLocalPlayerController(this); IsValid(LocalController) && LocalController->HasAuthority())
	{
		if (const AMultiplayerGameMode* MultiplayerGameMode = AMultiplayerGameMode::GetMultiplayerGameMode(this))
		{
			// Noting if game mode is valid, this is the server
			for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
			{
				const APlayerController* PlayerController = Iterator->Get();
				if (!IsValid(PlayerController)) continue;

				if (ALobbyPawn* LobbyPawn = Cast<ALobbyPawn>(PlayerController->GetPawn()); IsValid(LobbyPawn))
				{
					LobbyPawn->RemoveUILocal();
				}
			}
			
			MultiplayerGameMode->SeverTravelToGameLevel();
		}
		else
		{
			ULogging::LogVerboseError(GetName(), "ULobbyWidget::OnStartGameButtonClicked", "Game Mode is NOT of type AMultiplayerGameMode. Cannot start game!");
		}
	}
}

void ULobbyWidget::OnLeaveGameButtonClicked()
{
	// Noting here that if the host disconnects, Unreal Engine's default disconnection handling kicks in and all clients are booted to the main menu.
	// Otherwise, the client just disconnects locally.
	UMultiplayerLibrary::DisconnectLocalPlayer(this);
}
