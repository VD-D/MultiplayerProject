// Copyright Robert Uszynski

/* Class header. */
#include "Game/Actors/HunterPropStart.h"

/* Engine includes. */
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"

AHunterPropStart::AHunterPropStart()
{
 	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = Capsule;

	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	Arrow->SetupAttachment(RootComponent);

	StartRoleType = ERoleType::Unknown;
	HunterStartColor = FColor::Blue;
	PropStartColor = FColor::Green;
	UnknownStartColor = FColor::Red;
}

void AHunterPropStart::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName PropertyName = PropertyChangedEvent.Property != nullptr ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if (PropertyName == GET_MEMBER_NAME_CHECKED(AHunterPropStart, StartRoleType))
	{
		if (IsValid(Capsule))
		{
			switch (StartRoleType) {
			case ERoleType::Hunter:
				Capsule->ShapeColor = HunterStartColor;
				break;
			case ERoleType::Prop:
				Capsule->ShapeColor = PropStartColor;
				break;
			case ERoleType::Unknown:
				Capsule->ShapeColor = UnknownStartColor;
				break;
			}
		}
	}

	if (PropertyName == GET_MEMBER_NAME_CHECKED(AHunterPropStart, HunterStartColor) && StartRoleType == ERoleType::Hunter)
	{
		if (IsValid(Capsule)) Capsule->ShapeColor = HunterStartColor;
	}

	if (PropertyName == GET_MEMBER_NAME_CHECKED(AHunterPropStart, PropStartColor) && StartRoleType == ERoleType::Prop)
	{
		if (IsValid(Capsule)) Capsule->ShapeColor = PropStartColor;
	}

	if (PropertyName == GET_MEMBER_NAME_CHECKED(AHunterPropStart, UnknownStartColor) && StartRoleType == ERoleType::Unknown)
	{
		if (IsValid(Capsule)) Capsule->ShapeColor = UnknownStartColor;
	}
}
