// Copyright Robert Uszynski


#include "Core/Actors/MultiplayerGameState.h"

#include "Core/Settings/MultiplayerSettings.h"
#include "Shared/Interfaces/SessionConnectionListener.h"
#include "Shared/Libraries/Logging.h"

void AMultiplayerGameState::BroadcastPlayerConnected(APlayerController* NewPlayer, int32 CurrentNumPlayers, int32 MaxNumPlayers) const
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PlayerController = Iterator->Get();
		if (!IsValid(PlayerController)) continue;
		
		if (IsValid(PlayerController->GetClass()) && PlayerController->GetClass()->ImplementsInterface(USessionConnectionListener::StaticClass()))
		{
			if (UMultiplayerSettings::GetEnableOptionalLogging())
			{
				ULogging::LogMessageToConsole(FString::Printf(TEXT("%s implements ISessionConnectionListener! Executing..."), *PlayerController->GetName()));
			}
			
			ISessionConnectionListener::Execute_OnPlayerConnected(PlayerController, NewPlayer, CurrentNumPlayers, MaxNumPlayers);
		}

		if (APawn* ControlledPawn = PlayerController->GetPawn(); IsValid(PlayerController->GetPawn()))
		{
			if (IsValid(ControlledPawn->GetClass()) && ControlledPawn->GetClass()->ImplementsInterface(USessionConnectionListener::StaticClass()))
			{
				if (UMultiplayerSettings::GetEnableOptionalLogging())
				{
					ULogging::LogMessageToConsole(FString::Printf(TEXT("%s implements ISessionConnectionListener! Executing..."), *ControlledPawn->GetName()));
				}

				ISessionConnectionListener::Execute_OnPlayerConnected(ControlledPawn, NewPlayer, CurrentNumPlayers, MaxNumPlayers);
			}
		}
	}
}
