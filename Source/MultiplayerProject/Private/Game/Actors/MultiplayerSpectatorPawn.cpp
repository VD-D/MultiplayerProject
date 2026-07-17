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
			OnClientFollowNewCharacter(GameCharacters[FMath::RandRange(0, GameCharacters.Num() - 1)]);
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
		PlayerController->SetViewTarget(CharacterToFollow);
	}
}
