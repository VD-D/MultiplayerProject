// Copyright Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "MultiplayerGameCharacter.generated.h"

class UCameraComponent;
class UGameplayAbility;
class UInputAction;
class UInputComponent;
class UInputMappingContext;
class USpringArmComponent;

struct FInputActionInstance;

/* Defines a pair of an input action and an ability. */
USTRUCT(BlueprintType)
struct FAbilityInputMapping
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UInputAction> InputAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGameplayAbility> Ability;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 InputID;
};

/**
 * Base class for in game character - configures shared functionality.
 * Should never be instantiated - use subclasses instead.
 */
UCLASS(Abstract, Blueprintable)
class MULTIPLAYERPROJECT_API AMultiplayerGameCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()
#pragma region Components
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USpringArmComponent> SpringArm;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCameraComponent> Camera;

	/* Manages abilities for this player. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAbilitySystemComponent* AbilitySystemComponent;
#pragma endregion Components

#pragma region Config
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
#pragma endregion Config

#pragma region AbilitySystemInterface
public:
	/**
	 * Boilerplate API for ability system.
	 * @return Default ability system component.
	 */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; }
#pragma endregion AbilitySystemInterface

#pragma region Construction 
	/**
	 * Default constructor.
	 */
	AMultiplayerGameCharacter();

protected:
	/**
	 * TODO: Is BeginPlay needed?
	 */
	virtual void BeginPlay() override;

	/**
	 * Grants abilities and configures UI on owning client.
	 * @param NewController Unused.
	 */
	virtual void PossessedBy(AController* NewController) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/**
	 * Configures player input component for player.
	 * @param PlayerInputComponent Adds input bindings.
	 */
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
#pragma endregion Construction

#pragma region Input
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
