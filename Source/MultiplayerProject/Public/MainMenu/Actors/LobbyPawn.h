// Copyright Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "LobbyPawn.generated.h"

class UCameraComponent;

/**
 * Pawn which acts as the intermediary between the lobby and the actual game.
 */
UCLASS()
class MULTIPLAYERPROJECT_API ALobbyPawn : public APawn
{
	GENERATED_BODY()
#pragma region Components
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCameraComponent> Camera;
#pragma endregion Components

#pragma region Internal
private:
	UPROPERTY(ReplicatedUsing=OnRep_CurrentNumberPlayers)
	int32 CurrentNumberPlayers;

	UPROPERTY(ReplicatedUsing=OnRep_MaxNumberPlayers)
	int32 MaximumNumberPlayers;

	UPROPERTY(ReplicatedUsing=OnRep_PlayerNames)
	TArray<FString> PlayerNames;
#pragma endregion Internal

#pragma region Construction
public:
	/**
	 * Default constructor.
	 */
	ALobbyPawn();

protected:
	/**
	 * Configures delegates.
	 */
	virtual void BeginPlay() override;

	/**
	 * Removes lobby UI from viewport.
	 * @param EndPlayReason Unused.
	 */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/**
	 * Configures UI for local client.
	 * @param NewController Unused.
	 */
	virtual void PossessedBy(AController* NewController) override;

	/**
	 * Configures UI for local client.
	 */
	UFUNCTION(Client, Reliable)
	void OnPossessedClient();
#pragma endregion Construction

#pragma region Replication
public:
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

	UFUNCTION()
	void OnRep_PlayerNames();

	/* Utility to gather player names. */
	void UpdatePlayerNames();
#pragma endregion Replication

#pragma region Sessions
	/**
	 * Requests an update of the lobby widget on owning client.
	 * @param NewController Unused.
	 * @param CurrentNumPlayers Unused.
	 * @param MaxNumPlayers Unused.
	 */
	UFUNCTION()
	void OnPlayerConnected(APlayerController* NewController, int32 CurrentNumPlayers, int32 MaxNumPlayers);

	/**
	 * Updates information on widget.
	 * @param OldController Unused.
	 * @param CurrentNumPlayers Unused.
	 * @param MaxNumPlayers Unused.
	 */
	UFUNCTION()
	void OnPlayerDisconnected(APlayerController* OldController, int32 CurrentNumPlayers, int32 MaxNumPlayers);
#pragma endregion Sessions
};
