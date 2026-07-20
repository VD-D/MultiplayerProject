// Copyright Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpectatorPawn.h"
#include "MultiplayerSpectatorPawn.generated.h"

class AMultiplayerGameCharacter;
class UCameraComponent;
class UInputAction;
class USpringArmComponent;

struct FInputActionInstance;

UCLASS()
class MULTIPLAYERPROJECT_API AMultiplayerSpectatorPawn : public ASpectatorPawn
{
	GENERATED_BODY()
#pragma region Components
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> PawnRoot;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USpringArmComponent> SpringArm;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCameraComponent> SpectatorCamera;
#pragma endregion Components

#pragma region Config
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	TObjectPtr<UInputAction> MoveCamera;
#pragma endregion Config

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

	/**
	 * Configures camera movement.
	 * @param PlayerInputComponent Component to bind to.
	 */
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

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

	/**
	 * Moves camera.
	 * @param Instance Should be a vector 2D.
	 */
	UFUNCTION()
	void CameraLook(const FInputActionInstance& Instance);
#pragma endregion Construction
};
