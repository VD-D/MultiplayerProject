// Copyright Robert Uszynski

/* Class header. */
#include "Game/Actors/MultiplayerSpectatorPawn.h"

/* Project includes. */
#include "Core/Actors/MultiplayerGameController.h"
#include "Game/Actors/HunterCharacter.h"
#include "Game/Actors/MultiplayerGameCharacter.h"
#include "Game/Actors/PropCharacter.h"

/* Engine includes. */
#include "EngineUtils.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

AMultiplayerSpectatorPawn::AMultiplayerSpectatorPawn()
{
	PawnRoot = CreateDefaultSubobject<USceneComponent>("PawnRoot");
	RootComponent = PawnRoot;
	
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(PawnRoot);
	SpringArm->TargetArmLength = 400.0f;
	SpringArm->bUsePawnControlRotation = true;
	
	SpectatorCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Spectator Camera"));
	SpectatorCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
}

void AMultiplayerSpectatorPawn::SetFollowNewCharacter()
{
	if (!HasAuthority()) return;
	
	if (const AMultiplayerGameController* GameController = Cast<AMultiplayerGameController>(GetController()))
	{
		TArray<AMultiplayerGameCharacter*> GameCharacters;
		if (GameController->GetRoleType() == ERoleType::Hunter)
		{
			for (TActorIterator<AHunterCharacter> ActorIterator(GetWorld()); ActorIterator; ++ActorIterator)
			{
				AHunterCharacter* HunterCharacter = *ActorIterator;
				if (!IsValid(HunterCharacter)) continue;

				if (HunterCharacter->GetCurrentHealth() > 0.0f) GameCharacters.Emplace(HunterCharacter);
			}
		}
		else if (GameController->GetRoleType() == ERoleType::Prop)
		{
			for (TActorIterator<APropCharacter> ActorIterator(GetWorld()); ActorIterator; ++ActorIterator)
			{
				APropCharacter* PropCharacter = *ActorIterator;
				if (!IsValid(PropCharacter)) continue;

				if (PropCharacter->GetCurrentHealth() > 0.0f) GameCharacters.Emplace(PropCharacter);
			}
		}

		if (GameCharacters.IsEmpty())
		{
			OnClientFollowNewCharacter(nullptr);
		}
		else
		{
			AMultiplayerGameCharacter* GameCharacter = GameCharacters[FMath::RandRange(0, GameCharacters.Num() - 1)];
			GameCharacter->OnCharacterEndDeath.AddDynamic(this, &AMultiplayerSpectatorPawn::SetFollowNewCharacter); // NOTE: If the character we're following dies, follow a new one.
			OnClientFollowNewCharacter(GameCharacter);
		}
	}
	else
	{
		OnClientFollowNewCharacter(nullptr);
	}
}

void AMultiplayerSpectatorPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		Input->BindAction(MoveCamera, ETriggerEvent::Triggered, this, &AMultiplayerSpectatorPawn::CameraLook);
	}
}

void AMultiplayerSpectatorPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	SetFollowNewCharacter();
}

void AMultiplayerSpectatorPawn::CameraLook(const FInputActionInstance& Instance)
{
	const FVector2D AxisValue = Instance.GetValue().Get<FVector2D>();
	AddControllerYawInput(AxisValue.X);
	AddControllerPitchInput(AxisValue.Y);
}

void AMultiplayerSpectatorPawn::OnClientFollowNewCharacter_Implementation(AMultiplayerGameCharacter* CharacterToFollow)
{
	if (!IsValid(CharacterToFollow)) return;
	AttachToActor(CharacterToFollow, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
}
