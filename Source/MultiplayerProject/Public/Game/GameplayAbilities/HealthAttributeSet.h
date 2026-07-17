// Copyright Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "HealthAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * Simple attribute set for health.
 */
UCLASS()
class MULTIPLAYERPROJECT_API UHealthAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
#pragma region Attributes
public:
	UPROPERTY(BlueprintReadOnly, Category = "Combat Vehicle Attributes")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, Health);

	UPROPERTY(BlueprintReadOnly, Category = "Combat Vehicle Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UHealthAttributeSet, MaxHealth);
#pragma endregion Attributes
};
