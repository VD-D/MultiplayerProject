// Copyright Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MultiplayerGameMode.generated.h"

class AGameManager;
class AHunterCharacter;
class APropCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnPlayerSessionEvent, APlayerController*, PlayerController, int32, CurrentNumPlayers, int32, MaxNumPlayers);

/**
 * Used to enable seamless server travel.
 */
UCLASS()
class MULTIPLAYERPROJECT_API AMultiplayerGameMode : public AGameMode
{
	GENERATED_BODY()
#pragma region Delegates
public:
	UPROPERTY(BlueprintAssignable, Category = "Multiplayer Game Mode")
	FOnPlayerSessionEvent OnPlayerConnected;

	UPROPERTY(BlueprintAssignable, Category = "Multiplayer Game Mode")
	FOnPlayerSessionEvent OnPlayerDisconnected;
#pragma endregion Delegates

#pragma region Internal
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Classes")
	TSubclassOf<AGameManager> GameManagerClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Classes")
	TSubclassOf<AHunterCharacter> HunterCharacterClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Classes")
	TSubclassOf<APropCharacter> PropCharacterClass;
	
private:
	friend class AGameManager;
	
	/* Game manager. If this is valid, the game is in progress. */
	UPROPERTY()
	TObjectPtr<AGameManager> GameManagerInstance;
#pragma endregion Internal
	
#pragma region Construction
public:
	/**
	 * Default constructor.
	 */
	AMultiplayerGameMode();

	/**
	 * Gets appropriate class for pawn.
	 * @param Controller Cast to multiplayer game controller.
	 * @return Hunter or prop class, or default if the role is neither.
	 */
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* Controller) override;
#pragma endregion Construction

#pragma region Accessors
	/**
	 * @param WorldContextObject Gets world.
	 * @return Current game mode cast to multiplayer game mode. Only valid on server.
	 */
	UFUNCTION(BlueprintPure, Category = "MultiplayerGameMode", meta = (WorldContext = "WorldContextObject"))
	static AMultiplayerGameMode* GetMultiplayerGameMode(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "MultiplayerGameMode", meta = (WorldContext = "WorldContextObject"))
	static TSubclassOf<AHunterCharacter> GetHunterCharacterClass(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "MultiplayerGameMode", meta = (WorldContext = "WorldContextObject"))
	static TSubclassOf<APropCharacter> GetPropCharacterClass(const UObject* WorldContextObject);
#pragma endregion Accessors

#pragma region Connections
	/**
	 * Server travels to the main game.
	 */
	void SeverTravelToGameLevel() const;

	/**
	 * Checks if all players are connected. Spawns the game manager actor, which itself will assign player roles and
	 * begin the game timer.
	 * @return True if we actually started the game.
	 */
	bool TryToStartGame();
	
	/**
	 * Broadcasts OnPlayerConnected. This is ONLY broadcast on server.
	 * @param NewPlayer The player who just connected.
	 */
	virtual void PostLogin(APlayerController* NewPlayer) override;

	/**
	 * Broadcasts OnPlayerDisconnected if exiting controller is a player controller. This is ONLY broadcast on server.
	 * @param Exiting The controller which disconnected.
	 */
	virtual void Logout(AController* Exiting) override;
#pragma endregion Connections
};
