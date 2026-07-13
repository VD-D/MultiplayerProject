// Copyright Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Shared/Types/SharedEnums.h"
#include "Shared/Types/SharedStructs.h"
#include "MultiplayerSettings.generated.h"

USTRUCT(BlueprintType)
struct FPlayerRatio
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "1"))
	int32 NumProps;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "1"))
	int32 NumHunters;

	FPlayerRatio()
	{
		NumProps = 1;
		NumHunters = 1;
	}

	int32 GetTotal() const { return NumProps + NumHunters; }
};

/**
 * Settings shared across classes.
 */
UCLASS(Config = "Game", defaultconfig, meta = (DisplayName = "Multiplayer Game Settings"))
class MULTIPLAYERPROJECT_API UMultiplayerSettings : public UDeveloperSettings
{
	GENERATED_BODY()
#pragma region Statics
public:
	/* The number of session to search for can never exceed this number. */
	static int32 MaxSessionToFindPerSearch;
#pragma endregion Statics
	
#pragma region Logging
protected:
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Logging")
	bool bNewScreenMessagesOnTop = true;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Logging")
	FVector2D ScreenMessageScale = FVector2D(1.0f);

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Logging")
	FColor MessageColor = FColor::Cyan;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Logging")
	FColor WarningColor = FColor::Yellow;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Logging")
	FColor ErrorColor = FColor::Red;

	/* This logs generic information. Useful for observing game flow. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Logging")
	bool bEnableOptionalLogging = false;
#pragma endregion Logging

#pragma region WidgetSettings
	/* Classes of widgets which should be directly added to the viewport. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Widget Settings")
	TMap<EViewportWidget, FViewportWidgetSettings> ViewportWidgetSettings;

	/* How to handle mouse locking when in UI screens. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Game")
	EMouseLockMode MouseLockPreference = EMouseLockMode::DoNotLock;
#pragma endregion WidgetSettings

#pragma region Session
	/* This is the world we travel to locally which acts as a "lobby" (pre-game level). */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Session")
	TSoftObjectPtr<UWorld> LobbyWorld;

	/* Any additional options to use when travelling to lobby ("listen" is included by default). */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Session")
	FString AdditionalLobbyTravelOptions;
	
	/* How frequently we search for new sessions. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Session", meta = (ClampMin = "1"))
	float SessionRefreshTime = 5.0f;

	/* How many session to find every time we search for new sessions. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Session", meta = (ClampMin = "1"))
	int32 NumSessionsToFindPerSearch = 10;

	/* Max number of characters in session length. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Session", meta = (ClampMin = "1"))
	int32 MaxSessionNameLength = 12;
#pragma endregion Session

#pragma region Game
	/* This is the world we server-travel to and acts as the main game world. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Game")
	TSoftObjectPtr<UWorld> GameWorld;
		
	/* How many props there should be per hunter. */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Game")
	FPlayerRatio HunterToPropRatio;
#pragma endregion Game

#pragma region Accessors
public:
	/**
	 * @return Settings object.
	 */
	static const UMultiplayerSettings* GetGenericSettings();

	/**
	 * @return Whether logged messages on screen should appear on top of old ones.
	 */
	UFUNCTION(BlueprintPure, Category = "Multiplayer Settings")
	static bool GetNewScreenMessagesOnTop();

	/**
	 * @return Text scale of logged messages on screen.
	 */
	UFUNCTION(BlueprintPure, Category = "Multiplayer Settings")
	static FVector2D GetOnScreenMessageTextScale();

	/**
	 * @return Color of EVerbosity = Log log messages
	 */
	UFUNCTION(BlueprintPure, Category = "Multiplayer Settings")
	static FColor GetOnScreenMessageLogColor();

	/**
	 * @return Color of EVerbosity = Warning log messages
	 */
	UFUNCTION(BlueprintPure, Category = "Multiplayer Settings")
	static FColor GetOnScreenWarningLogColor();

	/**
	 * @return Color of EVerbosity = Error log messages
	 */
	UFUNCTION(BlueprintPure, Category = "Multiplayer Settings")
	static FColor GetOnScreenErrorLogColor();

	/**
	 * @return True if optional logging should be enabled.
	 */
	UFUNCTION(BlueprintPure, Category = "Multiplayer Settings")
	static bool GetEnableOptionalLogging();

	/**
	 * @param WidgetToFind Which widget to get settings for
	 * @return Setting for a widget to add to viewport.
	 */
	static const FViewportWidgetSettings* GetViewportSetting(EViewportWidget WidgetToFind);

	/**
	 * @return How to handle mouse locking when in UI screens.
	 */
	UFUNCTION(BlueprintPure, Category = "Multiplayer Settings")
	static EMouseLockMode GetMouseLockMode();

	/**
	 * @return Level which acts as a lobby.
	 */
	UFUNCTION(BlueprintPure, Category = "Multiplayer Settings")
	static TSoftObjectPtr<UWorld> GetLobbyLevel();

	/**
	 * @return Any additional options when travelling to lobby.
	 */
	UFUNCTION(BlueprintPure, Category = "Multiplayer Settings")
	static FString GetAdditionalLobbyTravelOptions();

	/**
	 * @return How often session should refresh.
	 */
	UFUNCTION(BlueprintPure, Category = "Multiplayer Settings")
	static float GetSessionRefreshTime();

	/**
	 * @return How many sessions to search for.
	 */
	UFUNCTION(BlueprintPure, Category = "Multiplayer Settings")
	static int32 GetNumSessionsToFindPerSearch();

	/**
	 * @return Max Length in characters of session names.
	 */
	UFUNCTION(BlueprintPure, Category = "Multiplayer Settings")
	static int32 GetMaxSessionNameLength();

	/**
	 * @return Level which acts as the main world.
	 */
	UFUNCTION(BlueprintPure, Category = "Multiplayer Settings")
	static TSoftObjectPtr<UWorld> GetGameWorld();

	/**
	 * @return How many hunters.
	 */
	UFUNCTION(BlueprintPure, Category = "Multiplayer Settings")
	static int32 GetNumHunters();

	/**
	 * @return How many props.
	 */
	UFUNCTION(BlueprintPure, Category = "Multiplayer Settings")
	static int32 GetNumProps();

	/**
	 * @return How many players total per game.
	 */
	UFUNCTION(BlueprintPure, Category = "Multiplayer Settings")
	static int32 GetTotalPlayerNum();
#pragma endregion Accessors
};
