// Copyright Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "Shared/Types/SharedEnums.h"
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
struct FAbilityToID
{
	GENERATED_BODY()

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TArray<FAbilityToID> AbilityToIDs;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	float MaxTraceDistance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	TEnumAsByte<ECollisionChannel> TraceCollisionChannel;
private:
	/* The last actor we have targeted. */
	UPROPERTY()
	TObjectPtr<AActor> CurrentTarget;
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

	/**
	 * Does trace and updates current target.
	 * @param DeltaTime Unused.
	 */
	virtual void Tick(float DeltaTime) override;

protected:
	/**
	 * TODO: Is BeginPlay needed?
	 */
	virtual void BeginPlay() override;

	/**
	 * Removes GameHUD from viewport.
	 * @param EndPlayReason Unused.
	 */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/**
	 * Grants abilities and configures UI on owning client.
	 * @param NewController Unused.
	 */
	virtual void PossessedBy(AController* NewController) override;

	UFUNCTION(Client, Reliable)
	void OnCharacterPossessedClient();

	UFUNCTION()
	void TryInitHUDTimeFromGameManager();

	UFUNCTION()
	void OnGameTimeUpdate(float NewTime);

	UFUNCTION()
	void OnGamePhaseUpdated(EGamePhase NewGamePhase);
#pragma endregion Construction

#pragma region Accessors
public:
	/**
	 * @return Target we are looking at.
	 */
	UFUNCTION(BlueprintPure, Category = "Multiplayer Game Character")
	AActor* GetCurrentTarget() const { return CurrentTarget; }
#pragma endregion Accessors

#pragma region Targeting
protected:
	/**
	 * Performs the line trace necessary for targeting.
	 */
	void DoTargeting();

	/**
	 * Sets the target's static and skeletal mesh components highlighted (simply renders them in custom depth).
	 * This is only assuming the target does not exceed the max size, as set in settings.
	 * @param Target Target we want to highlight.
	 * @param bShouldHighlight Whether we want to turn highlights on or off.
	 */
	//UFUNCTION(Client, Reliable)
	void SetHighlightTarget(const AActor* Target, bool bShouldHighlight);
#pragma endregion Targeting
};
