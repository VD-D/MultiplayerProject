// Copyright Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MultiplayerLibrary.generated.h"

/**
 * Generic multiplayer utilities.
 */
UCLASS()
class MULTIPLAYERPROJECT_API UMultiplayerLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
#pragma region Library
public:
	/**
	 * Utility to get the player's local (client-side) player controller.
	 * @param WorldContextObject Gets world.
	 * @return The first LOCAL player controller
	 */
	UFUNCTION(BlueprintPure, Category = "Multiplayer", meta = (WorldContext = "WorldContextObject"))
	static APlayerController* GetLocalPlayerController(const UObject* WorldContextObject);

	/**
	 * Number of player controllers. On clients, this will ALWAYS be 1, on the server this will be the actual number.
	 * @param WorldContextObject Gets world.
	 * @return How many player controllers are present.
	 */
	UFUNCTION(BlueprintPure, Category = "Multiplayer", meta = (WorldContext = "WorldContextObject"))
	static int32 GetNumPlayers(const UObject* WorldContextObject);

	/**
	 * Forces local player to disconnect.
	 * @param WorldContextObject Gets world.
	 */
	static void DisconnectLocalPlayer(const UObject* WorldContextObject);
#pragma endregion Library
};
