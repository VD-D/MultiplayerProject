// Copyright Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SharedStructs.generated.h"

class FOnlineSessionSearchResult;

#pragma region Multiplayer
USTRUCT()
struct FSessionResult
{
	GENERATED_BODY()

	FOnlineSessionSearchResult* OnlineResult;

	FSessionResult()
	{
		OnlineResult = nullptr;
	}

	explicit FSessionResult(FOnlineSessionSearchResult* InOnlineResult)
	{
		OnlineResult = InOnlineResult;
	}

	/**
	 * @return Ping of connection. 
	 */
	int32 GetPing() const;
};
#pragma endregion Multiplayer

#pragma region UI
/* Settings for a widget to be added to the viewport. */
USTRUCT(BlueprintType)
struct FViewportWidgetSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftClassPtr<UUserWidget> ViewportWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ZOrder;

	FViewportWidgetSettings()
	{
		ViewportWidget = nullptr;
		ZOrder = 0;
	}
};
#pragma endregion UI

/**
 * Structs shared across classes.
 */
UCLASS()
class MULTIPLAYERPROJECT_API USharedStructs : public UObject
{
	GENERATED_BODY()
};
