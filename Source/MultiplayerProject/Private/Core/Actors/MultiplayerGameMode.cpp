// Copyright Robert Uszynski

/* Class header. */
#include "Core/Actors/MultiplayerGameMode.h"

/* Project includes. */
#include "Core/Actors/MultiplayerGameState.h"
#include "Core/Settings/MultiplayerSettings.h"
#include "Shared/Libraries/Logging.h"
#include "Shared/Subsystems/SessionSubsystem.h"

/* Engine includes. */
#include "Game/Actors/GameManager.h"
#include "GameFramework/PlayerController.h"
#include "Shared/Libraries/MultiplayerLibrary.h"

AMultiplayerGameMode::AMultiplayerGameMode()
{
	bUseSeamlessTravel = true;
}

AMultiplayerGameMode* AMultiplayerGameMode::GetMultiplayerGameMode(const UObject* WorldContextObject)
{
	if (GEngine == nullptr) return nullptr;

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull); IsValid(World))
	{
		return World->GetAuthGameMode<AMultiplayerGameMode>();
	}

	return nullptr;
}

TSubclassOf<AHunterCharacter> AMultiplayerGameMode::GetHunterCharacterClass(const UObject* WorldContextObject)
{
	if (AMultiplayerGameMode* GameMode = GetMultiplayerGameMode(WorldContextObject))
	{
		return GameMode->HunterCharacterClass;
	}

	return nullptr;
}

TSubclassOf<APropCharacter> AMultiplayerGameMode::GetPropCharacterClass(const UObject* WorldContextObject)
{
	if (AMultiplayerGameMode* GameMode = GetMultiplayerGameMode(WorldContextObject))
	{
		return GameMode->PropCharacterClass;
	}

	return nullptr;
}

void AMultiplayerGameMode::SeverTravelToGameLevel() const
{
	if (IsValid(GetWorld()))
	{
		const FString URL = FPackageName::ObjectPathToPackageName(UMultiplayerSettings::GetGameWorld().ToString());
		GetWorld()->ServerTravel(URL);
	}
}

bool AMultiplayerGameMode::TryToStartGame()
{
	const FOnlineSessionSettings Settings = USessionSubsystem::GetSessionSettings(this);
	const int32 MaxNumPlayers = Settings.NumPublicConnections;
	const int32 CurrentNumPlayers = UMultiplayerLibrary::GetNumPlayers(this);

	if (UMultiplayerSettings::GetEnableOptionalLogging())
	{
		ULogging::LogMessageToConsole(FString::Printf(TEXT("Tried to start game with %d players out of %d players."), CurrentNumPlayers, MaxNumPlayers));
	}

	if (MaxNumPlayers > 0 && CurrentNumPlayers == MaxNumPlayers && !IsValid(GameManagerInstance))
	{
		GameManagerInstance = AGameManager::CreateInstance(this, GameManagerClass);
		return true;
	}
	
	return false;
}

void AMultiplayerGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	const FOnlineSessionSettings Settings = USessionSubsystem::GetSessionSettings(this);
	const int32 MaxNumPlayers = Settings.NumPublicConnections;
	const int32 CurrentNumPlayers = UMultiplayerLibrary::GetNumPlayers(this);

	if (UMultiplayerSettings::GetEnableOptionalLogging())
	{
		ULogging::LogMessageToConsole(FString::Printf(TEXT("A new player connected. There are now %d players out of %d players."), CurrentNumPlayers, MaxNumPlayers));
	}
			
	OnPlayerConnected.Broadcast(NewPlayer, CurrentNumPlayers, MaxNumPlayers);
}

void AMultiplayerGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	if (APlayerController* PlayerController = Cast<APlayerController>(Exiting))
	{
		const FOnlineSessionSettings Settings = USessionSubsystem::GetSessionSettings(this);
		const int32 MaxNumPlayers = Settings.NumPublicConnections;
		const int32 CurrentNumPlayers = UMultiplayerLibrary::GetNumPlayers(this) - 1;
		// Note: we have -1 as Logout is called before the controller is actually destroyed, so there is actually one less player.

		if (UMultiplayerSettings::GetEnableOptionalLogging())
		{
			ULogging::LogMessageToConsole(FString::Printf(TEXT("A player disconnected. There are now %d players out of %d players."), CurrentNumPlayers, MaxNumPlayers));
		}
		
		OnPlayerConnected.Broadcast(PlayerController, CurrentNumPlayers, MaxNumPlayers);
	}
}
