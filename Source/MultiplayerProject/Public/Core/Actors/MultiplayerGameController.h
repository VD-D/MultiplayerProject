// Copyright Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Shared/Types/SharedEnums.h"
#include "MultiplayerGameController.generated.h"

struct FInputActionInstance;
class UInputAction;
class UInputMappingContext;

/**
 * Controller which exists to set character class.
 */
UCLASS()
class MULTIPLAYERPROJECT_API AMultiplayerGameController : public APlayerController
{
	GENERATED_BODY()
#pragma region Config
protected:
	/* Mapping Context for player input. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Multiplayer Character")
	TObjectPtr<UInputMappingContext> InputMappingContext;
	
	ERoleType CurrentRole = ERoleType::Unknown;
#pragma endregion Config

#pragma region Construction
public:
	/**
	 * Configures player inputs.
	 */
	virtual void BeginPlay() override;

	/**
	 * Sets this player controller as a spectator.
	 */
	void SetSpectatorState();
#pragma endregion Construction
	
#pragma region Accessors
	UFUNCTION(BlueprintPure, Category = "Multiplayer Game Controller")
	ERoleType GetRoleType() const { return CurrentRole; }

	UFUNCTION(BlueprintCallable, Category = "Multiplayer Game Controller")
	void SetRoleType(ERoleType NewRole); // {  }
#pragma endregion Accessors

#pragma region Scoreboard
	/**
	 * Because the pawn possessed by the controller is nondeterministic at game end (some players may still be playing and alive,
	 * others might be spectators) we handle showing the scoreboard here.
	 * @param WinningSide Side which won the game.
	 * @param Duration How long to display scoreboard for.
	 */
	UFUNCTION(BlueprintCallable, Client, Reliable, Category = "Multiplayer Game Controller")
	void DisplayScoreboardForDuration(ERoleType WinningSide, float Duration);
#pragma endregion Scoreboard
};
