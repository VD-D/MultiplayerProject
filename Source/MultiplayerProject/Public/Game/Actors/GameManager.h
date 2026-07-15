// Copyright Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Shared/Types/SharedDelegates.h"
#include "Shared/Types/SharedEnums.h"
#include "GameManager.generated.h"

class AHunterPropStart;
class APlayerStart;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGamePhaseUpdated, EGamePhase, NewPhase);

/**
 * Actor which is responsible for storing game states which should be replicated to clients and also
 * performs countdowns until game starts and ends.
 */
UCLASS(NotPlaceable, BlueprintType, Blueprintable, Transient, hideCategories = (Info, Rendering, MovementReplication, Actor))
class MULTIPLAYERPROJECT_API AGameManager : public AActor
{
	GENERATED_BODY()
#pragma region Interal
public:
	UPROPERTY(BlueprintAssignable, Category = "Game Manager")
	FOnFloatPropertyChanged OnCountdownTimeTick;

	UPROPERTY(BlueprintAssignable, Category = "Game Manager")
	FOnGamePhaseUpdated OnGamePhaseUpdated;
	
protected:
	/* This variable is updated periodically via the timer bound to CountdownTimeTimerHandle (this is to prevent the GamePhaseTimerHandle timer's time replicating on tick). */
	UPROPERTY(ReplicatedUsing=OnRep_CountdownTime)
	float CountdownTime;

	UPROPERTY(ReplicatedUsing=OnRep_CurrentGamePhase)
	EGamePhase CurrentGamePhase;
	
	FTimerHandle CountdownTimeTimerHandle;
	FTimerHandle GamePhaseTimerHandle;
#pragma endregion Interal
	
#pragma region Construction
public:
	/**
	 * Default constructor.
	 */
	AGameManager();

	/**
	 * Creates a new GameManager instance, then spawns relevant characters and possesses, then begins countdown timer
	 * for game start.
	 * @param WorldContextObject Gets world.
	 * @param GameManagerClass Class to create instance from.
	 * @return The newly created instance.
	 */
	static AGameManager* CreateInstance(const UObject* WorldContextObject, TSubclassOf<AGameManager> GameManagerClass);

	/**
	 * Assigns roles for controllers, possesses new characters and starts begin game countdown timer.
	 */
	void AssignRolesAndPossessControllers();
#pragma endregion Construction

#pragma region Accessors
	UFUNCTION(BlueprintPure, Category = "Game Manager")
	float GetCountdownTime() const { return CountdownTime; }

	UFUNCTION(BlueprintPure, Category = "Game Manager")
	EGamePhase GetCurrentGamePhase() const { return CurrentGamePhase; }
#pragma endregion Accessors

#pragma region Utility
	/**
	 * @param WorldContextObject Gets world.
	 * @return Current instance.
	 */
	UFUNCTION(BlueprintCallable, Category = "Game Manager", meta = (WorldContext = "WorldContextObject"))
	static AGameManager* GetGameManager(const UObject* WorldContextObject);
	
private:
	/**
	 * @param PlayerStarts (OUT) All player start actors.
	 */
	void GetPlayerStarts(TArray<APlayerStart*>& PlayerStarts) const;

	/**
	 * @param RoleType Of what role type to get starts for.
	 * @param HunterPropStarts Out array of hunter prop start actors.
	 */
	void GetHunterPropStartsOfType(ERoleType RoleType, TArray<AHunterPropStart*>& HunterPropStarts) const;

	/**
	 * @param PlayerStarts (IN-OUT) Player starts to get transforms from.
	 * @return The found transform. This may be FTransform::Identity if no player starts exist.
	 */
	FTransform GetSpawnTransformFromPlayerStart(TArray<APlayerStart*>& PlayerStarts) const;

	/**
	 * @param RoleType Role type of player start to search.
	 * @param HunterPropStarts (IN-OUT) Hunter-prop starts to get transforms from.
	 * @return The found transform. This may be FTransform::Identity if no player starts exist.
	 */
	FTransform GetSpawnTransformFromHunterPropStart(ERoleType RoleType, TArray<AHunterPropStart*>& HunterPropStarts) const;
#pragma endregion Utility

#pragma region Timer
protected:
	/**
	 * Starts the timer for the current phase.
	 */
	void BeginTimerForPhase();

	/**
	 * Sets countdown time to server time.
	 */
	UFUNCTION()
	void UpdateCountdownTime();

	/**
	 * Advances phase and re-initialises time.
	 */
	UFUNCTION()
	void OnTimerForPhaseEnded();

	/**
	 * Disconnects local client - because this is only called on the host, this will disconnect everyone.
	 */
	UFUNCTION(Client, Reliable)
	void EndGameSession();
#pragma endregion Timer

#pragma region Replication
public:
	/**
	 * Replicates properties.
	 * @param OutLifetimeProps Unused.
	 */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UFUNCTION()
	void OnRep_CountdownTime();

	UFUNCTION()
	void OnRep_CurrentGamePhase();
#pragma endregion Replication
};
