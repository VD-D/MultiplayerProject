// Copyright Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Shared/Interfaces/SessionConnectionListener.h"
#include "LobbyPawn.generated.h"

class UCameraComponent;

/**
 * Pawn which acts as the intermediary between the lobby and the actual game.
 */
UCLASS()
class MULTIPLAYERPROJECT_API ALobbyPawn : public APawn, public ISessionConnectionListener
{
	GENERATED_BODY()
#pragma region Components
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* Camera;
#pragma endregion Components

#pragma region Construction
public:
	/**
	 * Default constructor.
	 */
	ALobbyPawn();

protected:
	/**
	 * Adds lobby UI to viewport.
	 */
	virtual void BeginPlay() override;

	/**
	 * Requests the lobby widget to be loaded on the owning client.
	 */
	UFUNCTION(Client, Reliable)
	void RequestLoadLobbyWidget(int32 CurrentNumPlayers, int32 MaxNumPlayers);
#pragma endregion Construction

#pragma region SessionConnectionListener
	/**
	 * Requests an update of the lobby widget on owning client.
	 * @param NewController Unused.
	 * @param CurrentNumPlayers Unused.
	 * @param MaxNumPlayers Unused.
	 */
	virtual void OnPlayerConnected_Implementation(APlayerController* NewController, int32 CurrentNumPlayers, int32 MaxNumPlayers) override;

	/**
	 * Requests an update on the lobby widget.
	 */
	UFUNCTION(Client, Reliable)
	void RequestLobbyWidgetUpdate(int32 CurrentNumPlayers, int32 MaxNumPlayers);
#pragma endregion SessionConnectionListener
};
