// Copyright Robert Uszynski

/* Project includes. */
#include "Game/Actors/MultiplayerGameCharacter.h"

/* Project includes. */
#include "Game/Actors/GameManager.h"
#include "Game/UI/InGameHUD.h"
#include "Shared/Subsystems/UIManager.h"

/* Engine includes. */
#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Core/Settings/MultiplayerSettings.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

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

	MaxTraceDistance = 6000.0f;
	TraceCollisionChannel = TEnumAsByte(ECC_Visibility);

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

void AMultiplayerGameCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IsLocallyControlled())
	{
		DoTargeting();
	}
}

void AMultiplayerGameCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AMultiplayerGameCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UUIManager::RemoveViewportWidget(this, EViewportWidget::GameHUD);
	Super::EndPlay(EndPlayReason);
}

void AMultiplayerGameCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	if (IsValid(AbilitySystemComponent))
	{
		for (const auto& GrantedAbility : AbilityToIDs)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(GrantedAbility.Ability, 1, GrantedAbility.InputID, this));
		}
	}
	
	OnCharacterPossessedClient();
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
			// TODO: Update current/max health.
		}
		
		if (WeakThis.IsValid())
		{
			WeakThis.Get()->TryInitHUDTimeFromGameManager();
		}
	}));
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

void AMultiplayerGameCharacter::DoTargeting()
{
	if (!IsValid(GetWorld())) return;
	
	const FVector StartLocation = IsValid(Camera) ? Camera->GetComponentLocation() : GetActorLocation();
	const FVector EndLocation = IsValid(Camera) ? Camera->GetForwardVector() * MaxTraceDistance + StartLocation : GetActorLocation();

	FCollisionQueryParams CollisionParams;
	CollisionParams.bTraceComplex = false;
	CollisionParams.AddIgnoredActor(this);
	
	if (FHitResult Hit; GetWorld()->LineTraceSingleByChannel(
		Hit,
		StartLocation,
		EndLocation,
		TraceCollisionChannel,
		CollisionParams
		))
	{
		if (Hit.GetActor() != CurrentTarget)
		{
			SetHighlightTarget(Hit.GetActor(), true);
			SetHighlightTarget(CurrentTarget, false);

			CurrentTarget = Hit.GetActor();
		}
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
		StaticMeshComponent->SetRenderCustomDepth(bShouldHighlight); 
	}

	TArray<USkeletalMeshComponent*> SkeletalMeshComponents;
	Target->GetComponents<USkeletalMeshComponent>(SkeletalMeshComponents);

	for (const auto SkeletalMeshComponent : SkeletalMeshComponents)
	{
		SkeletalMeshComponent->SetRenderCustomDepth(bShouldHighlight);
	}
}

