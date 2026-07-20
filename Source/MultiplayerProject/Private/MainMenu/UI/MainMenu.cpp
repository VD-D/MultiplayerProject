// Copyright Robert Uszynski

/* Class header. */
#include "MainMenu/UI/MainMenu.h"

/* Project includes. */
#include "Core/Settings/MultiplayerSettings.h"
#include "MainMenu/UI/SessionDisplay.h"
#include "Shared/Libraries/Logging.h"
#include "Shared/Libraries/MultiplayerLibrary.h"
#include "Shared/Subsystems/SessionSubsystem.h"

/* Engine includes. */
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "OnlineSessionSettings.h"

void UMainMenu::RefreshSessionsDisplayBox()
{
	OnBeginSearchForSessions();
	
	TWeakObjectPtr WeakThis = this;
	FOnSessionsFound OnSessionsFound;
	OnSessionsFound.BindLambda([WeakThis](bool bSuccess, const TArray<FSessionResult>& FoundSessions)
	{
		if (!bSuccess)
		{
			if (UMultiplayerSettings::GetEnableOptionalLogging()) ULogging::LogMessageToConsole("Failed to find any sessions on refresh");
			return;
		}
		
		if (UMainMenu* MainMenu = WeakThis.Get(); WeakThis.IsValid())
		{
			if (UMultiplayerSettings::GetEnableOptionalLogging())
			{
				ULogging::LogMessageToConsole(FString::Printf(TEXT("%s successfully found %d possible sessions to join."), *GetNameSafe(UMultiplayerLibrary::GetLocalPlayerController(WeakThis.Get())), FoundSessions.Num()));
			}

			if (!IsValid(MainMenu->SessionDisplayBox))
			{
				ULogging::LogVerboseError(GetNameSafe(MainMenu), "UMainMenu::RefreshSessionsDisplayBox (lambda->OnSessionsFound)", "Main menu session display box is invalid!");
				return;
			}

			MainMenu->SessionDisplayBox->ClearChildren();
			
			for (const auto& FoundSession : FoundSessions)
			{
				if (FoundSession.OnlineResult == nullptr) continue;
				
				if (USessionDisplay* NewInstance = USessionDisplay::CreateSessionDisplay(MainMenu, MainMenu->SessionDisplayWidgetClass, *FoundSession.OnlineResult); IsValid(NewInstance) && IsValid(MainMenu->SessionDisplayBox))
				{
					MainMenu->SessionDisplayBox->AddChildToVerticalBox(NewInstance);
				}
			}

			MainMenu->OnEndSearchForSessions();
			MainMenu->ConfigureTimer();
		}
	});
	
	// Noting here that in a real-world scenario, we would require functionality to detect if we are not using LAN, e.g. using EOS or Steam. 
	USessionSubsystem::FindSessions(UMultiplayerLibrary::GetLocalPlayerController(this), UMultiplayerSettings::GetNumSessionsToFindPerSearch(), true, true, OnSessionsFound);
}

void UMainMenu::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(HostSessionButton))
	{
		HostSessionButton->OnClicked.AddDynamic(this, &UMainMenu::OnHostSessionClicked);
	}

	if (IsValid(QuitGameButton))
	{
		QuitGameButton->OnClicked.AddDynamic(this, &UMainMenu::OnQuitClicked);
	}

	// This is to clear anything added to the session display box in the editor
	if (IsValid(SessionDisplayBox))
	{
		SessionDisplayBox->ClearChildren();
	}
	
	RefreshSessionsDisplayBox();
}

void UMainMenu::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (const UWorld* World = GetWorld(); IsValid(World) && TimerHandle.IsValid() && IsValid(RefreshSessionsTimeText))
	{
		const float TimeRemaining = World->GetTimerManager().GetTimerRemaining(TimerHandle);
		const int32 ClampedTime = FMath::Max(0, FMath::CeilToInt32(TimeRemaining));
		RefreshSessionsTimeText->SetText(FText::AsNumber(ClampedTime));
	}
}

void UMainMenu::NativeDestruct()
{
	if (const UWorld* World = GetWorld(); IsValid(World))
	{
		World->GetTimerManager().ClearTimer(TimerHandle);
	}
	
	Super::NativeDestruct();
}

void UMainMenu::ConfigureTimer()
{
	if (const UWorld* World = GetWorld(); IsValid(World))
	{
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindUObject(this, &UMainMenu::RefreshSessionsDisplayBox);
		
		World->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, FMath::Max(1.0f, UMultiplayerSettings::GetSessionRefreshTime()), false);
	}
}

void UMainMenu::OnHostSessionClicked()
{
	TWeakObjectPtr WeakThis = this;
	FOnSessionCreated OnSessionCreated;
	OnSessionCreated.BindLambda([WeakThis](bool bSuccess, const FOnlineSessionSettings& Settings)
	{
		if (WeakThis.IsValid())
		{
			if (const APlayerController* PlayerController = UMultiplayerLibrary::GetLocalPlayerController(WeakThis.Get()); IsValid(PlayerController) && UMultiplayerSettings::GetEnableOptionalLogging())
			{
				const FString SuccessString = bSuccess ? "successfully" : "unsuccessfully"; 
				ULogging::LogMessageToConsole(FString::Printf(TEXT("%s %s created a new multiplayer session for %d players"), *PlayerController->GetName(), *SuccessString, Settings.NumPublicConnections));
			}

			USessionSubsystem::TravelToLobby(WeakThis.Get());
		}
	});

	// Noting here that in a real-world scenario, we require functionality to detect if we are not using LAN, e.g. using EOS or Steam.
	USessionSubsystem::CreateSession(UMultiplayerLibrary::GetLocalPlayerController(this), UMultiplayerSettings::GetTotalPlayerNum(), true, true, OnSessionCreated);
}

void UMainMenu::OnQuitClicked()
{
	if (APlayerController* PlayerController = UMultiplayerLibrary::GetLocalPlayerController(this); IsValid(PlayerController))
	{
		PlayerController->ConsoleCommand("quit");
	}
}
