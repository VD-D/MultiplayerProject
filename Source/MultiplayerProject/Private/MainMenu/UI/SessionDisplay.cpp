// Copyright Robert Uszynski

/* Class header. */
#include "MainMenu/UI/SessionDisplay.h"

/* Project includes. */
#include "Shared/Libraries/Logging.h"
#include "Shared/Libraries/MultiplayerLibrary.h"
#include "Shared/Subsystems/SessionSubsystem.h"

/* Engine includes. */
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Core/Settings/MultiplayerSettings.h"

USessionDisplay* USessionDisplay::CreateSessionDisplay(UUserWidget* OwningWidget, TSubclassOf<USessionDisplay> SessionDisplayClass, const FOnlineSessionSearchResult& SearchResult)
{
	if (SessionDisplayClass.Get() == nullptr)
	{
		ULogging::LogVerboseError("USessionDisplay", "USessionDisplay::CreateSessionDisplay", "Tried creating session display with null class!");
		return nullptr;
	}

	if (!SearchResult.IsValid())
	{
		ULogging::LogVerboseError("USessionDisplay", "USessionDisplay::CreateSessionDisplay", "Tried creating session display with invalid search results!");
		return nullptr;
	}

	if (USessionDisplay* NewSessionDisplay = CreateWidget<USessionDisplay>(OwningWidget, SessionDisplayClass); IsValid(NewSessionDisplay))
	{
		NewSessionDisplay->SessionSearchResult = SearchResult;
		NewSessionDisplay->RefreshVisuals();
		return NewSessionDisplay;
	}

	return nullptr;
}

void USessionDisplay::RefreshVisuals() const
{
	if (!SessionSearchResult.IsValid()) return;

	if (IsValid(SessionNameText))
	{
		FString SessionOwnerName = SessionSearchResult.Session.OwningUserName;
		if (SessionOwnerName.Len() > UMultiplayerSettings::GetMaxSessionNameLength())
		{
			SessionOwnerName = SessionOwnerName.Left(UMultiplayerSettings::GetMaxSessionNameLength()) + "(...)";
		}
		
		SessionNameText->SetText(FText::FromString(SessionOwnerName));
	}

	if (IsValid(MaxPlayersText))
	{
		const int32 MaxPlayers = SessionSearchResult.Session.SessionSettings.NumPublicConnections;
		MaxPlayersText->SetText(FText::AsNumber(MaxPlayers));
	}

	if (IsValid(CurrentPlayersText))
	{
		const int32 CurrentPlayers = SessionSearchResult.Session.SessionSettings.NumPublicConnections - SessionSearchResult.Session.NumOpenPublicConnections;
		CurrentPlayersText->SetText(FText::AsNumber(CurrentPlayers));
	}

	if (IsValid(PingText))
	{
		const int32 Ping =  SessionSearchResult.PingInMs;
		PingText->SetText(FText::AsNumber(Ping));
	}
}

void USessionDisplay::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(ConnectToSessionButton))
	{
		ConnectToSessionButton->OnClicked.AddDynamic(this, &USessionDisplay::OnConnectToSessionClicked);
	}
}

void USessionDisplay::OnConnectToSessionClicked()
{
	USessionSubsystem::JoinSession(UMultiplayerLibrary::GetLocalPlayerController(this), &SessionSearchResult);
}
