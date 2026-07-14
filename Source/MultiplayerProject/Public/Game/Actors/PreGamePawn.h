// Copyright Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PreGamePawn.generated.h"

class UCameraComponent;

/**
 * Pawn possessed by default when players are waiting for others to connect.
 */
UCLASS()
class MULTIPLAYERPROJECT_API APreGamePawn : public APawn
{
	GENERATED_BODY()
#pragma region Components
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCameraComponent> Camera;
#pragma endregion Components

#pragma region Construction
public:
	/**
	 * Default constructor.
	 */
	APreGamePawn();

protected:
	/**
	 * Attempts to start game or waits for other players.
	 * @param NewController Unused.
	 */
	virtual void PossessedBy(AController* NewController) override;

	/**
	 * Tries to start the game.
	 * @param NewController Unused.
	 * @param CurrentNumPlayers Unused.
	 * @param MaxNumPlayers Unused.
	 */
	UFUNCTION()
	void OnPlayerConnected(APlayerController* NewController, int32 CurrentNumPlayers, int32 MaxNumPlayers);

	/**
	 * @return True if we actually managed to start the game.
	 */
	bool TryStartGame() const;
#pragma endregion Construction
};
