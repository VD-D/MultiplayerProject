// Copyright Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "MultiplayerGameState.generated.h"

/**
 * Used to broadcast session listener interface.
 */
UCLASS()
class MULTIPLAYERPROJECT_API AMultiplayerGameState : public AGameState
{
	GENERATED_BODY()

	/*
#pragma region StateVariables
protected:
	UPROPERTY(Replicated)
	int32 CurrentNumberPlayers;

	UPROPERTY(Replicated)
	int32 MaxNumberPlayers;
#pragma endregion StateVariables

#pragma region Accessors

#pragma endregion Accessors
*/
	
#pragma region SessionInterface
public:
	/**
	 * Broadcast each time a new player connects.
	 * @param NewPlayer The player which just connected.
	 * @param CurrentNumPlayers How many players are connected to the session.
	 * @param MaxNumPlayers The total number of player which can connect to the session.
	 */
	void BroadcastPlayerConnected(APlayerController* NewPlayer, int32 CurrentNumPlayers, int32 MaxNumPlayers) const;
#pragma endregion SessionInterface
};
