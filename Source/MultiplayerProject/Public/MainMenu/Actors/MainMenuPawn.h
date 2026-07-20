// Copyright Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MainMenuPawn.generated.h"

class UCameraComponent;

/**
 * Pawn which configures main menu.
 */
UCLASS()
class MULTIPLAYERPROJECT_API AMainMenuPawn : public APawn
{
	GENERATED_BODY()
#pragma region Components
protected:
	/* Default camera. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* Camera;
#pragma endregion Components
	
#pragma region Construction
public:
	/**
	 * Default constructor.
	 */
	AMainMenuPawn();

protected:
	/**
	 * Shows main menu.
	 * @param NewController Checked if it is a player controller.
	 */
	virtual void PossessedBy(AController* NewController) override;

	/**
	 * Removes main menu.
	 * @param EndPlayReason Unused.
	 */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
#pragma endregion Construction
};
