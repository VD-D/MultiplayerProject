// Copyright Robert Uszynski


#include "Core/Actors/MultiplayerGameState.h"

#include "Core/Settings/MultiplayerSettings.h"
#include "Net/UnrealNetwork.h"
#include "Shared/Interfaces/SessionConnectionListener.h"
#include "Shared/Libraries/Logging.h"

AMultiplayerGameState* AMultiplayerGameState::GetMultiplayerGameState(const UObject* WorldContextObject)
{
	if (GEngine == nullptr) return nullptr;

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull); IsValid(World))
	{
		return World->GetGameState<AMultiplayerGameState>();
	}

	return nullptr;
}

int32 AMultiplayerGameState::GetCurrentNumberPlayers(const UObject* WorldContextObject)
{
	if (const AMultiplayerGameState* GameState = GetMultiplayerGameState(WorldContextObject))
	{
		return GameState->CurrentNumberPlayers;
	}

	return -1;
}

int32 AMultiplayerGameState::GetMaxNumberPlayers(const UObject* WorldContextObject)
{
	if (const AMultiplayerGameState* GameState = GetMultiplayerGameState(WorldContextObject))
	{
		return GameState->MaximumNumberPlayers;
	}

	return -1;
}

void AMultiplayerGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMultiplayerGameState, CurrentNumberPlayers);
	DOREPLIFETIME(AMultiplayerGameState, MaximumNumberPlayers);
}

void AMultiplayerGameState::OnRep_CurrentNumberPlayers()
{
	OnCurrentNumPlayersChanged.Broadcast(CurrentNumberPlayers);
}

void AMultiplayerGameState::OnRep_MaxNumberPlayers()
{
	OnMaxNumPlayersChanged.Broadcast(MaximumNumberPlayers);
}

void AMultiplayerGameState::BroadcastPlayerConnected(APlayerController* NewPlayer, int32 CurrentNumPlayers, int32 MaxNumPlayers)
{
	if (!HasAuthority()) return;

	CurrentNumberPlayers = CurrentNumPlayers;
	MaximumNumberPlayers = MaxNumPlayers;

	OnCurrentNumPlayersChanged.Broadcast(CurrentNumberPlayers);
	OnMaxNumPlayersChanged.Broadcast(MaximumNumberPlayers);
	
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
