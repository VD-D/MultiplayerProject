// Copyright Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SessionConnectionListener.generated.h"

UINTERFACE(MinimalAPI)
class USessionConnectionListener : public UInterface
{
	GENERATED_BODY()
};

/**
 * Broadcasts to player controllers and pawns whenever a player connects or disconnects. 
 */
class MULTIPLAYERPROJECT_API ISessionConnectionListener
{
	GENERATED_BODY()
public:
	/**
	 * Broadcast whenever a new player connects.
	 * @param NewController The controller which just connected.
	 * @param CurrentNumPlayers How many players have connected in total.
	 * @param MaxNumPlayers How many players can connect to the session.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Session Connection Listener")
	void OnPlayerConnected(APlayerController* NewController, int32 CurrentNumPlayers, int32 MaxNumPlayers);
	virtual void OnPlayerConnected_Implementation(APlayerController* NewController, int32 TotalNumPlayers, int32 MaxNumPlayers) {}

	/**
	 * Broadcast whenever a player disconnects
	 * @param TotalNumPlayers How many players remain connected.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Session Connection Listener")
	void OnPlayerDisconnected(int32 TotalNumPlayers);
	virtual void OnPlayerDisconnected_Implementation(int32 TotalNumPlayers) {}
};
