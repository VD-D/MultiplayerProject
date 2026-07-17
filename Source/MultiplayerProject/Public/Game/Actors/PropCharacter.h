// Copyright Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "Game/Actors/MultiplayerGameCharacter.h"
#include "PropCharacter.generated.h"

/**
 * Functionality specific to the prop character.
 */
UCLASS()
class MULTIPLAYERPROJECT_API APropCharacter : public AMultiplayerGameCharacter
{
	GENERATED_BODY()
#pragma region Components
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* PropMesh;
#pragma endregion Components

#pragma region Construction
public:
	/**
	 * Default constructor.
	 */
	APropCharacter();
#pragma endregion Construction

#pragma region Prop
	/**
	 * Hides player mesh and instead sets prop mesh visible.
	 * @param StaticMeshComp The static mesh comp to copy static mesh from.
	 */
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category = "Prop Character")
	void TurnSelfIntoProp(UStaticMeshComponent* StaticMeshComp);
#pragma endregion Prop
};
