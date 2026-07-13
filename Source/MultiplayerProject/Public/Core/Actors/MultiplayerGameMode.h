// Copyright Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MultiplayerGameMode.generated.h"

/**
 * Used to enable seamless server travel.
 */
UCLASS()
class MULTIPLAYERPROJECT_API AMultiplayerGameMode : public AGameMode
{
	GENERATED_BODY()
#pragma region Construction
public:
	/**
	 * Default constructor.
	 */
	AMultiplayerGameMode();
#pragma endregion Construction

#pragma region Accessors
	/**
	 * @param WorldContextObject Gets world.
	 * @return Current game mode cast to multiplayer game mode. Only valid on server.
	 */
	UFUNCTION(BlueprintPure, Category = "MultiplayerGameMode", meta = (WorldContext = "WorldContextObject"))
	static AMultiplayerGameMode* GetMultiplayerGameMode(const UObject* WorldContextObject);
#pragma endregion Accessors

#pragma region Connections
	/**
	 * Server travels to the main game.
	 */
	void BeginGame() const;
	
	/**
	 * Broadcasts SessionConnectionListener interface to player controllers and pawns. This is ONLY broadcast on server.
	 * @param NewPlayer The player who just connected.
	 */
	virtual void PostLogin(APlayerController* NewPlayer) override;
#pragma endregion Connections
};
