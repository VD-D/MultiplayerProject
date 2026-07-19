// Copyright Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "Shared/Types/SharedDelegates.h"
#include "Shared/Types/SharedEnums.h"
#include "MultiplayerGameCharacter.generated.h"

class UCameraComponent;
class UGameplayAbility;
class UGameplayEffect;
class UInputAction;
class UInputComponent;
class UInputMappingContext;
class USpringArmComponent;

struct FInputActionInstance;

UENUM(BlueprintType)
enum class EHealthChangeType : uint8
{
	Lost      UMETA(DisplayName = "Lost"),
	Unchanged UMETA(DisplayName = "Unchanged"),
	Increased UMETA(DisplayName = "Increased")
};

UENUM(BlueprintType)
enum class ECharacterAnimationState : uint8
{
	Alive UMETA(DisplayName = "Alive"),
	Dead  UMETA(DisplayName = "Dead")
};

/* Defines a pair of an input action and an ability. */
USTRUCT(BlueprintType)
struct FAbilityToID
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
#pragma region Delegates
public:
	UPROPERTY(BlueprintAssignable, Category = "Multiplayer Game Character")
	FGenericDelegate OnCharacterDeath;
#pragma endregion Delegates
	
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
	/* Input action driving movement. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Multiplayer Character")
	TObjectPtr<UInputAction> MovementInputAction;

	/* Input action camera look. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Multiplayer Character")
	TObjectPtr<UInputAction> CameraInputAction;
	
	/* All abilities this character has. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Multiplayer Game Character")
	TArray<FAbilityToID> AbilityToIDs;

	/* Enables/disables movement and ability inputs. */
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Multiplayer Game Character")
	bool bEnableCharacterInput;

	/* This should be used to configure initial attributes. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Multiplayer Game Character")
	TSubclassOf<UGameplayEffect> InitialEffectConfig;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Multiplayer Game Character")
	float MaxTraceDistance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Multiplayer Game Character")
	TEnumAsByte<ECollisionChannel> TraceCollisionChannel;

	/* How long until we reach the end of the "death" state. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Multiplayer Game Character")
	float DeathHoldTime;

	/* This should be used for cosmetic-type effects such as animations. */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Multiplayer Game Character")
	ECharacterAnimationState CharacterAnimationState;
	
private:
	/* The last actor we have targeted. */
	UPROPERTY()
	TObjectPtr<AActor> CurrentTarget;

	UPROPERTY(ReplicatedUsing=OnRep_DisplayCurrentHealth)
	float DisplayCurrentHealth;
	
	UPROPERTY(ReplicatedUsing=OnRep_DisplayMaxHealth)
	float DisplayMaxHealth;
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
	 * Configures input actions.
	 * @param PlayerInputComponent Component to bind input to.
	 */
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

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

	UFUNCTION()
	void OnAbilityInputPressed(int32 InputID);
#pragma endregion Input

#pragma region Accessors
public:
	/**
	 * @return Target we are looking at. This is only valid on the local client.
	 */
	UFUNCTION(BlueprintPure, Category = "Multiplayer Game Character")
	AActor* GetCurrentTarget() const { return CurrentTarget; }

	/**
	 * @return Whether character input (movement and abilities) are enabled.
	 */
	UFUNCTION(BlueprintPure, Category = "Multiplayer Game Character")
	bool GetEnableCharacterInput() const { return bEnableCharacterInput; }

	/**
	 * This value is only valid on the server.
	 * @return Character's current health.
	 */
	UFUNCTION(BlueprintPure, Category = "Multiplayer Game Character")
	float GetCurrentHealth() const;

	/**
	 * This value is only valid on the server.
	 * @return Character's max health.
	 */
	UFUNCTION(BlueprintPure, Category = "Multiplayer Game Character")
	float GetMaxHealth() const;

	/**
	 * Server only. Sets character input as enabled or disabled.
	 * @param NewValue True to enable, false to disable.
	 */
	UFUNCTION(BlueprintCallable, Category = "Multiplayer Game Character")
	void SetEnableCharacterInput(bool NewValue) { if (HasAuthority()) bEnableCharacterInput = NewValue; }
#pragma endregion Accessors

#pragma region Replication
	/**
	 * Sets props for replication.
	 * @param OutLifetimeProps Unused.
	 */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:	
	UFUNCTION()
	void OnRep_DisplayCurrentHealth();

	UFUNCTION()
	void OnRep_DisplayMaxHealth();
#pragma endregion Replication
	
#pragma region Targeting
public:
	/**
	 * Performs the line trace necessary for targeting.
	 */
	UFUNCTION(BlueprintPure, Category = "Multiplayer Game Character")
	void DoTargeting(FHitResult& OutHit);

protected:
	/**
	 * Updates the CurrentTarget if it is differnet from the previous one.
	 * @param Hit Hit result to get actor from.
	 */
	void SetTargetLocal(const FHitResult& Hit);
	
	/**
	 * Sets the target's static and skeletal mesh components highlighted (simply renders them in custom depth).
	 * This is only assuming the target does not exceed the max size, as set in settings.
	 * @param Target Target we want to highlight.
	 * @param bShouldHighlight Whether we want to turn highlights on or off.
	 */
	static void SetHighlightTarget(const AActor* Target, bool bShouldHighlight);
#pragma endregion Targeting

#pragma region GameplayAbility
private:
	/**
	 * Called when the character drops to or below 0 health.
	 */
	void Die();

	UFUNCTION()
	void OnDeathDelayFinished();
	
	UFUNCTION()
	void OnCurrentHealthUpdated(float NewValue);

	UFUNCTION()
	void OnMaxHealthUpdated(float NewValue);

	/**
	 * @param NewValue Value being changed to.
	 * @param OldValue Current value.
	 * @return How new value compares to old value. NewValue == OldValue -> Unchanged, NewValue > OldValue -> Increased, NewValue < OldValue -> Lost.
	 */
	static EHealthChangeType GetChangeType(float NewValue, float OldValue);
#pragma endregion GameplayAbility

#pragma region CharacterAPI
protected:
	/**
	 * Called after the death delay.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Multiplayer Game Character")
	void PostDeath();
	virtual void PostDeath_Implementation() {}
	
	/**
	 * Callback each current health changes.
	 * @param NewValue New health value.
	 * @param ChangeType Whether this value is greater (increased), the same (unchanged) or lower (lost) than previous.
	 */
	// UFUNCTION(BlueprintImplementableEvent, Category = "Multiplayer Game Character")
	virtual void OnCurrentHealthChanged(float NewValue, EHealthChangeType ChangeType) {}
	// virtual void OnCurrentHealthChanged_Implementation(float NewValue, EHealthChangeType ChangeType) {}

	/**
	 * Callback each time max health changes.
	 * @param NewValue New health value.
	 * @param ChangeType Whether this value is greater (increased), the same (unchanged) or lower (lost) than previous.
	 */
	//UFUNCTION(BlueprintImplementableEvent, Category = "Multiplayer Game Character")
	virtual void OnMaxHealthChanged(float NewValue, EHealthChangeType ChangeType) {}
	//virtual void OnMaxHealthChanged_Implementation(float NewValue, EHealthChangeType ChangeType) {}
#pragma endregion CharacterAPI
};
