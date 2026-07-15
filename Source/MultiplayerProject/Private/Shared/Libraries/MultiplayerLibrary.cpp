// Copyright Robert Uszynski

/* Class header. */
#include "Shared/Libraries/MultiplayerLibrary.h"

#include "GameMapsSettings.h"
#include "Core/Settings/MultiplayerSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Shared/Libraries/Logging.h"
#include "Shared/Subsystems/SessionSubsystem.h"

APlayerController* UMultiplayerLibrary::GetLocalPlayerController(const UObject* WorldContextObject)
{
	if (GEngine == nullptr) return nullptr;

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		return GEngine->GetFirstLocalPlayerController(World);
	}

	return nullptr;
}

int32 UMultiplayerLibrary::GetNumPlayers(const UObject* WorldContextObject)
{
	if (GEngine == nullptr) return 0;

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		TArray<APlayerController*> Controllers;
		for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			APlayerController* PlayerController = Iterator->Get();
			if (!IsValid(PlayerController)) continue;
			Controllers.Emplace(PlayerController);
		}

		return Controllers.Num();	
	}

	return 0;
}

void UMultiplayerLibrary::SetInputEnabledOnAllControllers(const UObject* WorldContextObject, bool bEnable)
{
	if (GEngine == nullptr) return;

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull); IsValid(World))
	{
		for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			APlayerController* PlayerController = Iterator->Get();
			if (!IsValid(PlayerController)) continue;

			if (!PlayerController->HasAuthority()) break;

			if (APawn* ControlledPawn = PlayerController->GetPawn(); IsValid(ControlledPawn))
			{
				if (bEnable) ControlledPawn->EnableInput(PlayerController);
				else ControlledPawn->DisableInput(PlayerController);
			}
		}
	}
}

void UMultiplayerLibrary::DisconnectLocalPlayer(const UObject* WorldContextObject)
{
	if (APlayerController* LocalController = GetLocalPlayerController(WorldContextObject); IsValid(LocalController))
	{
		TWeakObjectPtr WeakLocalPlayer = LocalController;
		USessionSubsystem::DestroySession(LocalController, FOnSessionDestroyed::CreateLambda([WeakLocalPlayer](bool bSuccess, const FName& SessionName)
		{
			const FString& GameDefaultMapName = UGameMapsSettings::GetGameDefaultMap(); 
			if (UMultiplayerSettings::GetEnableOptionalLogging())
			{
				const FString& SuccessString = bSuccess ? "left" : "did not leave";
				ULogging::LogMessageToConsole(FString::Printf(TEXT("Player %s the session called %s"), *SuccessString, *SessionName.ToString()));

				if (bSuccess) ULogging::LogMessageToConsole(FString::Printf(TEXT("Returning to %s"), *GameDefaultMapName));
			}

			if (bSuccess && WeakLocalPlayer.IsValid())
			{
				UGameplayStatics::OpenLevel(WeakLocalPlayer.Get(), FName(*GameDefaultMapName));
			}
		}));
	}
}
