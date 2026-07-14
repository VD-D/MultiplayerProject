// Copyright Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Shared/Types/SharedDelegates.h"
#include "MultiplayerGameState.generated.h"

/**
 * Used to broadcast session listener interface.
 */
UCLASS()
class MULTIPLAYERPROJECT_API AMultiplayerGameState : public AGameState
{
	GENERATED_BODY()
#pragma region StateVariables
public:
	UPROPERTY(BlueprintAssignable)
	FOnIntPropertyChanged OnCurrentNumPlayersChanged;

	UPROPERTY(BlueprintAssignable)
	FOnIntPropertyChanged OnMaxNumPlayersChanged;
	
protected:
	UPROPERTY(ReplicatedUsing=OnRep_CurrentNumberPlayers)
	int32 CurrentNumberPlayers;

	UPROPERTY(ReplicatedUsing=OnRep_MaxNumberPlayers)
	int32 MaximumNumberPlayers;
#pragma endregion StateVariables

#pragma region Accessors
public:
	UFUNCTION(BlueprintPure, Category = "Multiplayer Game State", meta = (WorldContext = "WorldContextObject"))
	static AMultiplayerGameState* GetMultiplayerGameState(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintPure, Category = "Multiplayer Game State", meta = (WorldContext = "WorldContextObject"))
	static int32 GetCurrentNumberPlayers(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "Multiplayer Game State", meta = (WorldContext = "WorldContextObject"))
	static int32 GetMaxNumberPlayers(const UObject* WorldContextObject);
#pragma endregion Accessors

#pragma region Replication
	/**
	 * For replicating Current and Max players.
	 * @param OutLifetimeProps Unused.
	 */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UFUNCTION()
	void OnRep_CurrentNumberPlayers();

	UFUNCTION()
	void OnRep_MaxNumberPlayers();
#pragma endregion Replication
	
#pragma region SessionInterface
public:
	/**
	 * Broadcast each time a new player connects.
	 * @param NewPlayer The player which just connected.
	 * @param CurrentNumPlayers How many players are connected to the session.
	 * @param MaxNumPlayers The total number of player which can connect to the session.
	 */
	void BroadcastPlayerConnected(APlayerController* NewPlayer, int32 CurrentNumPlayers, int32 MaxNumPlayers);
#pragma endregion SessionInterface
};
