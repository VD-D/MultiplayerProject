// Copyright Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpectatorPawn.h"
#include "MultiplayerSpectatorPawn.generated.h"

class AMultiplayerGameCharacter;
class UCameraComponent;

UCLASS()
class MULTIPLAYERPROJECT_API AMultiplayerSpectatorPawn : public ASpectatorPawn
{
	GENERATED_BODY()
#pragma region Components
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* SpectatorCamera;
#pragma endregion Components

#pragma region Construction
public:
	/**
	 * Default constructor.
	 */
	AMultiplayerSpectatorPawn();

	/**
	 * Finds a new character to follow and follows it.
	 */
	UFUNCTION()
	void SetFollowNewCharacter();

protected:
	/**
	 * Configures which pawn this spectator will follow.
	 * @param NewController Cast to multiplayer game controller to get role.
	 */
	virtual void PossessedBy(AController* NewController) override;

	/**
	 * @param CharacterToFollow Character who we will view.
	 */
	UFUNCTION(Client, Reliable)
	void OnClientFollowNewCharacter(AMultiplayerGameCharacter* CharacterToFollow);
#pragma endregion Construction
};
