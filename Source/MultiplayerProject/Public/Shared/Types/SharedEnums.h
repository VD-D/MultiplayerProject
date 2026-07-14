// Copyright Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SharedEnums.generated.h"

#pragma region UI
/* Types of widgets addable to the viewport. */
UENUM(BlueprintType)
enum class EViewportWidget : uint8
{
	MainMenu      UMETA(DisplayName = "Main Menu"),
	Lobby         UMETA(DisplayName = "Lobby"),
	GameHUD       UMETA(DisplayName = "GameHUD"),
	LoadingScreen UMETA(DisplayName = "Loading Screen")
};
#pragma endregion UI

#pragma region Game
UENUM(BlueprintType)
enum class ERoleType : uint8
{
	Hunter  UMETA(DisplayName = "Hunter"),
	Prop    UMETA(DisplayName = "Prop"),
	Unknown UMETA(DisplayName = "Unknown")
};
#pragma endregion Game

/**
 * Enums shared across classes.
 */
UCLASS()
class MULTIPLAYERPROJECT_API USharedEnums : public UObject
{
	GENERATED_BODY()
};
