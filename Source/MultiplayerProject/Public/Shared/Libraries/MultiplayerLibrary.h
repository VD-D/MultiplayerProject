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
	 * This function only works on the server. Enables/disables input on all controllers.
	 * @param WorldContextObject Gets world.
	 * @param bEnable True to enable input, false to disable it.
	 */
	UFUNCTION(BlueprintCallable, Category = "Multiplayer", meta = (WorldContext = "WorldContextObject"))
	static void SetInputEnabledOnAllControllers(const UObject* WorldContextObject, bool bEnable);

	static void DisconnectLocalPlayer(const UObject* WorldContextObject);
#pragma endregion Library
};
