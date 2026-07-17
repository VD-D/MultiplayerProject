// Copyright Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Shared/Types/SharedEnums.h"
#include "MultiplayerGameController.generated.h"

struct FInputActionInstance;
class UInputAction;
class UInputMappingContext;

USTRUCT(BlueprintType)
struct FAbilityInputMapping
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UInputAction> InputAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 InputID;
};

/**
 * Controller which exists to set character class.
 */
UCLASS()
class MULTIPLAYERPROJECT_API AMultiplayerGameController : public APlayerController
{
	GENERATED_BODY()
#pragma region Config
protected:
	/* Mapping Context for player input. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Multiplayer Character")
	TObjectPtr<UInputMappingContext> InputMappingContext;

	/* Input actions enabling usage of gameplay abilities. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Multiplayer Character")
	TArray<FAbilityInputMapping> AbilityActions;
	
	ERoleType CurrentRole = ERoleType::Unknown;
#pragma endregion Config

#pragma region Construction
public:
	/**
	 * Configures player inputs.
	 */
	virtual void BeginPlay() override;
#pragma endregion Construction
	
#pragma region Accessors
	UFUNCTION(BlueprintPure, Category = "Multiplayer Game Controller")
	ERoleType GetRoleType() const { return CurrentRole; }

	UFUNCTION(BlueprintCallable, Category = "Multiplayer Game Controller")
	void SetRoleType(ERoleType NewRole) { if (HasAuthority()) CurrentRole = NewRole; }
#pragma endregion Accessors
};
