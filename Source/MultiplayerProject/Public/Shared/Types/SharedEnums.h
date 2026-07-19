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
	LoadingScreen UMETA(DisplayName = "Loading Screen"),
	Scoreboard    UMETA(DisplayName = "Scoreboard")
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

UENUM(BlueprintType)
enum class EGamePhase : uint8
{
	GameCountdown = 0 UMETA(DisplayName = "Game Countdown"),
	InGame        = 1 UMETA(DisplayName = "In Game"),
	Scoreboard    = 2 UMETA(DisplayName = "Scoreboard")
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
