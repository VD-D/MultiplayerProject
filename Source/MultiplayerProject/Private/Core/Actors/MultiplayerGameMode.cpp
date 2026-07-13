// Copyright Robert Uszynski

/* Class header. */
#include "Core/Actors/MultiplayerGameMode.h"

/* Project includes. */
#include "Core/Actors/MultiplayerGameState.h"
#include "Core/Settings/MultiplayerSettings.h"
#include "Shared/Libraries/Logging.h"
#include "Shared/Subsystems/SessionSubsystem.h"

/* Engine includes. */
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

void AMultiplayerGameMode::BeginGame() const
{
	if (IsValid(GetWorld()))
	{
		const FString URL = FPackageName::ObjectPathToPackageName(UMultiplayerSettings::GetGameWorld().ToString());
		GetWorld()->ServerTravel(URL);
	}
}

void AMultiplayerGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (IsValid(GetWorld()))
	{
		if (const AMultiplayerGameState* MultiplayerGameState = GetWorld()->GetGameState<AMultiplayerGameState>(); IsValid(MultiplayerGameState))
		{
			const FOnlineSessionSettings Settings = USessionSubsystem::GetSessionSettings(this);
			const int32 MaxNumPlayers = Settings.NumPublicConnections;
			const int32 CurrentNumPlayers = UMultiplayerLibrary::GetNumPlayers(this);

			if (UMultiplayerSettings::GetEnableOptionalLogging())
			{
				ULogging::LogMessageToConsole(FString::Printf(TEXT("A new player logged in. There are now %d players out of %d players."), CurrentNumPlayers, MaxNumPlayers));
			}
			
			MultiplayerGameState->BroadcastPlayerConnected(NewPlayer, CurrentNumPlayers, MaxNumPlayers);
		}
		else
		{
			ULogging::LogVerboseError(GetName(), "AMultiplayerGameMode::PostLogin", "Game state is not AMultiplayerGameState!");
		}
	}
}
