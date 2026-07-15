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

	/* Input action driving movement. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Multiplayer Character")
	TObjectPtr<UInputAction> MovementInputAction;

	/* Input action camera look. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Multiplayer Character")
	TObjectPtr<UInputAction> CameraInputAction;

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

#pragma region Input
private:
	/**
	 * Enables WASD movement for this combat vehicle.
	 * @param Instance Should be a vector 2D.
	 */
	UFUNCTION()
	void MovementInput(const FInputActionInstance& Instance);

	/**
	 * Attempts to interact with whatever is at the top of the interact stack.
	 * @param Instance Unused.
	 */
	UFUNCTION()
	void CameraLook(const FInputActionInstance& Instance);

	/**
	 * Informs ability system component an ability with a given id has been pressed.
	 * @param InputID Passed to ability system component
	 */
	UFUNCTION()
	void OnAbilityInputPressed(int32 InputID);

	/**
	 * Informs ability system component an ability with a given id has been released.
	 * @param InputID Passed to ability system component
	 */
	UFUNCTION()
	void OnAbilityInputReleased(int32 InputID);
#pragma endregion Input
};
