// Copyright Robert Uszynski

/* Class header. */
#include "Game/Actors/PropCharacter.h"

/* Project includes. */
#include "Core/Settings/MultiplayerSettings.h"
#include "Game/Actors/GameManager.h"
#include "Game/Actors/HunterPropStart.h"

/* Engine includes. */
#include "GameFramework/PlayerStart.h"

APropCharacter::APropCharacter()
{
	PropMesh = CreateDefaultSubobject<UStaticMeshComponent>("PropMesh");
	PropMesh->SetupAttachment(RootComponent);
	PropMesh->SetVisibility(false);
	PropMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void APropCharacter::OnHit_Implementation()
{
	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		MeshComp->SetVisibility(true, true);
		MeshComp->SetEnableAnimation(true);
	}

	if (IsValid(PropMesh))
	{
		PropMesh->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
		PropMesh->SetVisibility(false, true);
		PropMesh->SetStaticMesh(nullptr);
	}
}

void APropCharacter::OnCurrentHealthChanged(float NewValue, EHealthChangeType ChangeType)
{
	if (NewValue > 0.0f && ChangeType == EHealthChangeType::Lost && HasAuthority())
	{
		CharacterAnimationState = ECharacterAnimationState::Dead;
		SetEnableCharacterInput(false);
		OnHit();

		TWeakObjectPtr WeakThis = this;
		
		FTimerHandle Handle;
		FTimerDelegate Delegate;
		Delegate.BindLambda([WeakThis]()
		{
			if (WeakThis.IsValid())
			{
				WeakThis.Get()->CharacterAnimationState = ECharacterAnimationState::Alive;
				WeakThis.Get()->SetEnableCharacterInput(true);

				if (UMultiplayerSettings::GetPlayerStartPreference() == EStartPreference::CustomStarts)
				{
					TArray<AHunterPropStart*> HunterPropStarts;
					AGameManager::GetHunterPropStartsOfType(WeakThis.Get(), ERoleType::Prop, HunterPropStarts);

					if (!HunterPropStarts.IsEmpty())
					{
						if (const AHunterPropStart* RandomStart = HunterPropStarts[FMath::RandRange(0, HunterPropStarts.Num() - 1)]; IsValid(RandomStart))
						{
							WeakThis.Get()->SetActorLocation(RandomStart->GetActorLocation());
						}
					}
				}
				else
				{
					TArray<APlayerStart*> PlayerStarts;
					AGameManager::GetPlayerStarts(WeakThis.Get(), PlayerStarts);

					if (!PlayerStarts.IsEmpty())
					{
						if (const APlayerStart* RandomStart = PlayerStarts[FMath::RandRange(0, PlayerStarts.Num() - 1)]; IsValid(RandomStart))
						{
							WeakThis.Get()->SetActorLocation(RandomStart->GetActorLocation());
						}
					}
				}
			}
		});
		
		GetWorldTimerManager().SetTimer(Handle, Delegate, DeathHoldTime, false);
	}
}

void APropCharacter::TurnSelfIntoProp_Implementation(UStaticMeshComponent* StaticMeshComp)
{
	if (!IsValid(StaticMeshComp)) return;

	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		MeshComp->SetVisibility(false, true);
		MeshComp->SetEnableAnimation(false);
	}

	if (IsValid(PropMesh))
	{
		PropMesh->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
		PropMesh->SetVisibility(true, true);
		PropMesh->SetWorldScale3D(StaticMeshComp->GetComponentScale());
		PropMesh->SetStaticMesh(StaticMeshComp->GetStaticMesh());
	}
}
