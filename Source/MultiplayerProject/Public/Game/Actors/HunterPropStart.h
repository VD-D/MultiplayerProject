// Copyright Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Shared/Types/SharedEnums.h"
#include "HunterPropStart.generated.h"

class UArrowComponent;
class UCapsuleComponent;

struct FPropertyChangedEvent;

/**
 * Similar to a player start, defines locations where hunters and props can start.
 */
UCLASS()
class MULTIPLAYERPROJECT_API AHunterPropStart : public AActor
{
	GENERATED_BODY()
#pragma region Components
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCapsuleComponent> Capsule;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UArrowComponent> Arrow;
#pragma endregion Components

#pragma region Config
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Hunter-Prop Start Location")
	ERoleType StartRoleType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hunter-Prop Start Location")
	FColor HunterStartColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hunter-Prop Start Location")
	FColor PropStartColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hunter-Prop Start Location")
	FColor UnknownStartColor;
#pragma endregion Config

#pragma region Construction
public:
	/**
	 * Default constructor.
	 */
	AHunterPropStart();

	/**
	 * Updates capsule color.
	 * @param PropertyChangedEvent Unused.
	 */
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#pragma endregion Construction

#pragma region Accessors
	/**
	 * @return Role type of this start.
	 */
	UFUNCTION(BlueprintPure, Category = "Hunter-Prop Start")
	ERoleType GetStartRoleType() const { return StartRoleType; }
#pragma endregion Accessors
};
