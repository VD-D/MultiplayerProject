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
#include "Camera/CameraComponent.h"

AMultiplayerSpectatorPawn::AMultiplayerSpectatorPawn()
{
	SpectatorCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Spectator Camera"));
	RootComponent = SpectatorCamera;
}

void AMultiplayerSpectatorPawn::SetFollowNewCharacter()
{
	if (!HasAuthority() || !IsLocallyControlled()) return;
	
	if (const AMultiplayerGameController* GameController = Cast<AMultiplayerGameController>(GetController()))
	{
		UE_LOG(LogTemp, Log, TEXT("SetFollowNewCharacter (current role type is %s)"), *UEnum::GetValueAsString(GameController->GetRoleType()));
		
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
			UE_LOG(LogTemp, Log, TEXT("Did not find character to follow"));
			OnClientFollowNewCharacter(nullptr);
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("Found character to follow"));
			
			AMultiplayerGameCharacter* GameCharacter = GameCharacters[FMath::RandRange(0, GameCharacters.Num() - 1)];
			GameCharacter->OnCharacterDeath.AddDynamic(this, &AMultiplayerSpectatorPawn::SetFollowNewCharacter); // NOTE: If the character we're following dies, follow a new one.
			OnClientFollowNewCharacter(GameCharacter);
		}
	}
	else
	{
		OnClientFollowNewCharacter(nullptr);
	}
}

void AMultiplayerSpectatorPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	SetFollowNewCharacter();
}

void AMultiplayerSpectatorPawn::OnClientFollowNewCharacter_Implementation(AMultiplayerGameCharacter* CharacterToFollow)
{
	if (!IsValid(CharacterToFollow)) return;
	
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		PlayerController->SetViewTargetWithBlend(CharacterToFollow, 0.2f);
	}
}
