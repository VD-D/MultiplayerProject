// Copyright Robert Uszynski

/* Project includes. */
#include "Game/Actors/MultiplayerGameCharacter.h"

/* Project includes. */
#include "Core/Actors/MultiplayerGameController.h"
#include "Core/Settings/MultiplayerSettings.h"
#include "Game/Actors/GameManager.h"
#include "Game/GameplayAbilities/HealthAttributeSet.h"
#include "Game/UI/InGameHUD.h"
#include "Shared/Libraries/Logging.h"
#include "Shared/Subsystems/UIManager.h"

/* Engine includes. */
#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"

class UEnhancedInputLocalPlayerSubsystem;

AMultiplayerGameCharacter::AMultiplayerGameCharacter()
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>("Spring Arm");
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 400.0f;
	SpringArm->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>("AbilitySystemComponent");

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	bEnableCharacterInput = true;
	
	MaxTraceDistance = 6000.0f;
	TraceCollisionChannel = TEnumAsByte(ECC_Visibility);

	DeathHoldTime = 3.0f;
	CharacterAnimationState = ECharacterAnimationState::Alive;

	if (UCapsuleComponent* CharacterCapsule = GetCapsuleComponent(); IsValid(CharacterCapsule))
	{
		CharacterCapsule->SetCapsuleSize(35.0f, 90.0f, false);
	}

	if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement(); IsValid(MovementComponent))
	{
		MovementComponent->MaxAcceleration = 1500.0f;
		MovementComponent->BrakingFrictionFactor = 1.0f;
		MovementComponent->bUseSeparateBrakingFriction = true;
		MovementComponent->MinAnalogWalkSpeed = 20.0f;
		MovementComponent->BrakingDecelerationWalking = 2000.0f;
		MovementComponent->JumpZVelocity = 500.0f;
		MovementComponent->BrakingDecelerationFalling = 1500.0f;
		MovementComponent->AirControl = 0.35f;
		MovementComponent->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
		MovementComponent->bOrientRotationToMovement = true;
	}
	
	PrimaryActorTick.bCanEverTick = true;
}

void AMultiplayerGameCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		Input->BindAction(MovementInputAction, ETriggerEvent::Triggered, this, &AMultiplayerGameCharacter::MovementInput);
		Input->BindAction(CameraInputAction, ETriggerEvent::Triggered, this, &AMultiplayerGameCharacter::CameraLook);
		
		for (const auto& Ability : AbilityToIDs)
		{
			Input->BindAction(Ability.InputAction, ETriggerEvent::Started, this, &AMultiplayerGameCharacter::OnAbilityInputPressed, Ability.InputID);
		}
	}
}

void AMultiplayerGameCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IsLocallyControlled())
	{
		FHitResult OutHit;
		DoTargeting(OutHit);
		SetTargetLocal(OutHit);
	}
}

void AMultiplayerGameCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (IsLocallyControlled())
	{
		UUIManager::RemoveViewportWidget(this, EViewportWidget::GameHUD);
	}
	
	Super::EndPlay(EndPlayReason);
}

void AMultiplayerGameCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	if (IsValid(AbilitySystemComponent))
	{
		AbilitySystemComponent->AddSet<UHealthAttributeSet>();

		TWeakObjectPtr WeakThis = this;
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UHealthAttributeSet::GetHealthAttribute()).AddLambda([WeakThis](const FOnAttributeChangeData& Modifier)
		{
			if (WeakThis.IsValid()) WeakThis.Get()->OnCurrentHealthUpdated(Modifier.NewValue);
		});
		
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UHealthAttributeSet::GetMaxHealthAttribute()).AddLambda([WeakThis](const FOnAttributeChangeData& Modifier)
		{
			if (WeakThis.IsValid()) WeakThis.Get()->OnMaxHealthUpdated(Modifier.NewValue);
		});
		
		for (const auto& GrantedAbility : AbilityToIDs)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(GrantedAbility.Ability, 1, GrantedAbility.InputID, this));
		}
	}

	if (InitialEffectConfig.Get() != nullptr)
	{
		const UGameplayEffect* GameplayEffect = InitialEffectConfig->GetDefaultObject<UGameplayEffect>();
		const FGameplayEffectContextHandle Handle = AbilitySystemComponent->MakeEffectContext();
		AbilitySystemComponent->ApplyGameplayEffectToSelf(GameplayEffect, 1.0f, Handle);
	}
	else
	{
		ULogging::LogVerboseError(GetName(), "AMultiplayerGameCharacter::PossessedBy", "Initial effect config was invalid!");
	}
	
	OnCharacterPossessedClient();
}

void AMultiplayerGameCharacter::UnPossessed()
{
	Super::UnPossessed();
	OnCharacterUnPossessedClient();
}

void AMultiplayerGameCharacter::OnCharacterPossessedClient_Implementation()
{
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		const FInputModeGameOnly InputMode;
		PlayerController->SetInputMode(InputMode);
		PlayerController->bShowMouseCursor = false;
	}
	
	TWeakObjectPtr WeakThis = this;
	UUIManager::LoadViewportWidget(this, EViewportWidget::GameHUD, FOnWidgetLoaded::CreateLambda([WeakThis](UUserWidget* UserWidget)
	{
		if (const UInGameHUD* InGameHUD = Cast<UInGameHUD>(UserWidget))
		{
			InGameHUD->UpdatePlayerRoleText();
		}
		
		if (WeakThis.IsValid())
		{
			WeakThis.Get()->TryInitHUDTimeFromGameManager();
			WeakThis.Get()->OnRep_DisplayCurrentHealth();
			WeakThis.Get()->OnRep_DisplayMaxHealth();
		}
	}));
}

void AMultiplayerGameCharacter::OnCharacterUnPossessedClient_Implementation()
{
	if (IsLocallyControlled())
	{
		UUIManager::RemoveViewportWidget(this, EViewportWidget::GameHUD);
	}
}

void AMultiplayerGameCharacter::TryInitHUDTimeFromGameManager()
{
	bool bSuccess = false;
	if (AGameManager* GameManager = AGameManager::GetGameManager(this))
	{
		if (UInGameHUD* GameHUD = Cast<UInGameHUD>(UUIManager::GetLoadedWidget(this, EViewportWidget::GameHUD)); IsValid(GameHUD))
		{
			GameHUD->UpdateGameTimeText(GameManager->GetCountdownTime());
			GameHUD->OnGamePhaseChanged(GameManager->GetCurrentGamePhase());
			GameManager->OnCountdownTimeTick.AddDynamic(this, &AMultiplayerGameCharacter::OnGameTimeUpdate);
			GameManager->OnGamePhaseUpdated.AddDynamic(this, &AMultiplayerGameCharacter::OnGamePhaseUpdated);
			bSuccess = true;
		}
	}

	// NOTE: Although the GameManager *should* be valid on clients by this point (given it spawns before game characters
	// are spawned), due to lag, it may still not have appeared for client. Therefore, if we fail, we will simply try again
	// after 1 second.
	if (!bSuccess)
	{
		FTimerHandle TimerHandle;
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindUObject(this, &AMultiplayerGameCharacter::TryInitHUDTimeFromGameManager);
		
		GetWorldTimerManager().SetTimer(TimerHandle, TimerDelegate, 1.0f, false);
	}
}

void AMultiplayerGameCharacter::OnGameTimeUpdate(float NewTime)
{
	if (const UInGameHUD* GameHUD = Cast<UInGameHUD>(UUIManager::GetLoadedWidget(this, EViewportWidget::GameHUD)); IsValid(GameHUD))
	{
		GameHUD->UpdateGameTimeText(NewTime);
	}
}

void AMultiplayerGameCharacter::OnGamePhaseUpdated(EGamePhase NewGamePhase)
{
	if (UInGameHUD* GameHUD = Cast<UInGameHUD>(UUIManager::GetLoadedWidget(this, EViewportWidget::GameHUD)); IsValid(GameHUD))
	{
		GameHUD->OnGamePhaseChanged(NewGamePhase);
	}
}

void AMultiplayerGameCharacter::MovementInput(const FInputActionInstance& Instance)
{
	if (!bEnableCharacterInput) return;
	
	const FRotator ThisControlRotation = GetControlRotation();
	const FVector2D AxisValue = Instance.GetValue().Get<FVector2D>();
	
	AddMovementInput(FRotationMatrix(FRotator(0.0f, ThisControlRotation.Yaw, ThisControlRotation.Roll)).GetScaledAxis(EAxis::Y), AxisValue.X);
	AddMovementInput(FRotationMatrix(FRotator(0.0f, ThisControlRotation.Yaw, 0.0f)).GetScaledAxis(EAxis::X), AxisValue.Y);
}

void AMultiplayerGameCharacter::CameraLook(const FInputActionInstance& Instance)
{
	const FVector2D AxisValue = Instance.GetValue().Get<FVector2D>();
	AddControllerYawInput(AxisValue.X);
	AddControllerPitchInput(AxisValue.Y);
}

void AMultiplayerGameCharacter::OnAbilityInputPressed(int32 InputID)
{
	if (!bEnableCharacterInput) return;
	
	if (IsValid(AbilitySystemComponent))
	{
		const FAbilityToID* FoundData = AbilityToIDs.FindByPredicate([InputID](const FAbilityToID& AbilityToID)
		{
			return AbilityToID.InputID == InputID;
		});

		if (FoundData != nullptr)
		{
			AbilitySystemComponent->TryActivateAbilityByClass(FoundData->Ability, true);
		}
	}
}

float AMultiplayerGameCharacter::GetCurrentHealth() const
{
	return UHealthAttributeSet::GetHealthAttribute().GetNumericValue(AbilitySystemComponent->GetAttributeSet(UHealthAttributeSet::StaticClass()));
}

float AMultiplayerGameCharacter::GetMaxHealth() const
{
	return UHealthAttributeSet::GetMaxHealthAttribute().GetNumericValue(AbilitySystemComponent->GetAttributeSet(UHealthAttributeSet::StaticClass()));
}

void AMultiplayerGameCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMultiplayerGameCharacter, bEnableCharacterInput);
	DOREPLIFETIME(AMultiplayerGameCharacter, DisplayCurrentHealth);
	DOREPLIFETIME(AMultiplayerGameCharacter, DisplayMaxHealth);
	DOREPLIFETIME(AMultiplayerGameCharacter, CharacterAnimationState);
}

void AMultiplayerGameCharacter::OnRep_DisplayCurrentHealth()
{
	if (!IsLocallyControlled()) return;
	
	if (const UInGameHUD* GameHUD = Cast<UInGameHUD>(UUIManager::GetLoadedWidget(this, EViewportWidget::GameHUD)); IsValid(GameHUD))
	{
		GameHUD->UpdateCurrentHealthText(FMath::Max(DisplayCurrentHealth, 0.0f)); // Noting that we do not want the display going into negative numbers.
	}
}

void AMultiplayerGameCharacter::OnRep_DisplayMaxHealth()
{
	if (!IsLocallyControlled()) return;
	
	if (const UInGameHUD* GameHUD = Cast<UInGameHUD>(UUIManager::GetLoadedWidget(this, EViewportWidget::GameHUD)); IsValid(GameHUD))
	{
		GameHUD->UpdateMaxHealthText(FMath::Max(DisplayMaxHealth, 0.0f));
	}
}

void AMultiplayerGameCharacter::DoTargeting(FHitResult& OutHit)
{
	if (!IsValid(GetWorld())) return;
	
	const FVector StartLocation = IsValid(Camera) ? Camera->GetComponentLocation() : GetActorLocation();
	const FVector EndLocation = IsValid(Camera) ? Camera->GetForwardVector() * MaxTraceDistance + StartLocation : GetActorLocation();

	FCollisionQueryParams CollisionParams;
	CollisionParams.bTraceComplex = false;
	CollisionParams.AddIgnoredActor(this);
	
	GetWorld()->LineTraceSingleByChannel(
		OutHit,
		StartLocation,
		EndLocation,
		TraceCollisionChannel,
		CollisionParams);
}

void AMultiplayerGameCharacter::SetTargetLocal(const FHitResult& Hit)
{
	if (Hit.GetActor() != CurrentTarget)
	{
		SetHighlightTarget(Hit.GetActor(), true);
		SetHighlightTarget(CurrentTarget, false);

		CurrentTarget = Hit.GetActor();
	}
}

void AMultiplayerGameCharacter::SetHighlightTarget(const AActor* Target, bool bShouldHighlight)
{
	if (!IsValid(Target)) return;

	FVector Origin, Bounds;
	Target->GetActorBounds(true, Origin, Bounds);
	
	if (Bounds.X > UMultiplayerSettings::GetMaxTargetableObjectSize().X ||
		Bounds.Y > UMultiplayerSettings::GetMaxTargetableObjectSize().Y ||
		Bounds.Z > UMultiplayerSettings::GetMaxTargetableObjectSize().Z) return;

	TArray<UStaticMeshComponent*> StaticMeshComponents;
	Target->GetComponents<UStaticMeshComponent>(StaticMeshComponents);

	for (const auto StaticMeshComponent : StaticMeshComponents)
	{
		if (UMultiplayerSettings::GetIsMeshForbidden(StaticMeshComponent->GetStaticMesh()))
		{
			return;
		}
	}

	for (const auto StaticMeshComponent : StaticMeshComponents)
	{
		StaticMeshComponent->SetRenderCustomDepth(bShouldHighlight); 
	}

	TArray<USkeletalMeshComponent*> SkeletalMeshComponents;
	Target->GetComponents<USkeletalMeshComponent>(SkeletalMeshComponents);

	for (const auto SkeletalMeshComponent : SkeletalMeshComponents)
	{
		SkeletalMeshComponent->SetRenderCustomDepth(bShouldHighlight);
	}
}

void AMultiplayerGameCharacter::Die()
{
	if (!HasAuthority()) return;

	CharacterAnimationState = ECharacterAnimationState::Dead;
	OnCharacterBeginDeath.Broadcast();

	FTimerHandle TimerHandle;
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUObject(this, &AMultiplayerGameCharacter::OnDeathDelayFinished);
	
	GetWorldTimerManager().SetTimer(TimerHandle, TimerDelegate, FMath::Max(0.1f, DeathHoldTime), false);
}

void AMultiplayerGameCharacter::OnDeathDelayFinished()
{
	if (!HasAuthority()) return;
	
	OnCharacterEndDeath.Broadcast();
	if (IsLocallyControlled())
	{
		UUIManager::RemoveViewportWidget(this, EViewportWidget::GameHUD);
	}
	
	if (AMultiplayerGameController* GameController = Cast<AMultiplayerGameController>(GetController()); IsValid(GameController))
	{
		GameController->SetSpectatorState();
	}
	
	AGameManager::CheckGameFinished(this);
	Destroy();
}

void AMultiplayerGameCharacter::OnCurrentHealthUpdated(float NewValue)
{
	const EHealthChangeType ChangeType = GetChangeType(NewValue, DisplayCurrentHealth);
	
	DisplayCurrentHealth = NewValue;
	OnRep_DisplayCurrentHealth();
	OnCurrentHealthChanged(NewValue, ChangeType);

	if (NewValue <= 0.0f) Die();
}

void AMultiplayerGameCharacter::OnMaxHealthUpdated(float NewValue)
{
	const EHealthChangeType ChangeType = GetChangeType(NewValue, DisplayMaxHealth);
	
	DisplayMaxHealth = NewValue;
	OnRep_DisplayMaxHealth();
	OnMaxHealthChanged(NewValue, ChangeType);
}

EHealthChangeType AMultiplayerGameCharacter::GetChangeType(float NewValue, float OldValue)
{
	EHealthChangeType ChangeType = EHealthChangeType::Unchanged;
	if (NewValue > OldValue) ChangeType = EHealthChangeType::Increased;
	else if (NewValue < OldValue) ChangeType = EHealthChangeType::Lost;

	return ChangeType;
}

