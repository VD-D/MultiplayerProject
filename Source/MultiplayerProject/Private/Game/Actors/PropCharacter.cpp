// Copyright Robert Uszynski

/* Class header. */
#include "Game/Actors/PropCharacter.h"

APropCharacter::APropCharacter()
{
	PropMesh = CreateDefaultSubobject<UStaticMeshComponent>("PropMesh");
	PropMesh->SetupAttachment(RootComponent);
	PropMesh->SetVisibility(false);
	PropMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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
