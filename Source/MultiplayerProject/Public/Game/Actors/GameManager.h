// Copyright Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Shared/Types/SharedEnums.h"
#include "GameManager.generated.h"

class AHunterPropStart;
class APlayerStart;

/**
 * Actor which is responsible for storing game states which should be replicated to clients and also
 * performs countdowns until game starts and ends.
 */
UCLASS(NotPlaceable, BlueprintType, Blueprintable, Transient, hideCategories = (Info, Rendering, MovementReplication, Actor))
class MULTIPLAYERPROJECT_API AGameManager : public AActor
{
	GENERATED_BODY()
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
};
