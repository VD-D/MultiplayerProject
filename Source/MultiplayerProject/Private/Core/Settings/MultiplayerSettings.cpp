// Copyright Robert Uszynski

/* Class header. */
#include "Core/Settings/MultiplayerSettings.h"

int32 UMultiplayerSettings::MaxSessionToFindPerSearch = 50;

UMultiplayerSettings::UMultiplayerSettings()
{
    GamePhaseDuration.Emplace(EGamePhase::GameCountdown, 5.0f);
    GamePhaseDuration.Emplace(EGamePhase::InGame, 360.0f);
    GamePhaseDuration.Emplace(EGamePhase::Scoreboard, 10.0f);
}

const UMultiplayerSettings* UMultiplayerSettings::GetGenericSettings()
{
    return GetDefault<UMultiplayerSettings>();
}

bool UMultiplayerSettings::GetNewScreenMessagesOnTop()
{
    if (const UMultiplayerSettings* Settings = GetGenericSettings()) return Settings->bNewScreenMessagesOnTop;
    return true;
}

FVector2D UMultiplayerSettings::GetOnScreenMessageTextScale()
{
    if (const UMultiplayerSettings* Settings = GetGenericSettings()) return Settings->ScreenMessageScale;
    return FVector2D(1.0f);
}

FColor UMultiplayerSettings::GetOnScreenMessageLogColor()
{
    if (const UMultiplayerSettings* Settings = GetGenericSettings()) return Settings->MessageColor;
    return FColor::Cyan;
}

FColor UMultiplayerSettings::GetOnScreenWarningLogColor()
{
    if (const UMultiplayerSettings* Settings = GetGenericSettings()) return Settings->WarningColor;
    return FColor::Yellow;
}

FColor UMultiplayerSettings::GetOnScreenErrorLogColor()
{
    if (const UMultiplayerSettings* Settings = GetGenericSettings()) return Settings->ErrorColor;
    return FColor::Red;
}

bool UMultiplayerSettings::GetEnableOptionalLogging()
{
    if (const UMultiplayerSettings* Settings = GetGenericSettings()) return Settings->bEnableOptionalLogging;
    return false;
}

const FViewportWidgetSettings* UMultiplayerSettings::GetViewportSetting(EViewportWidget WidgetToFind)
{
    if (const UMultiplayerSettings* Settings = GetGenericSettings()) 
    {
        return Settings->ViewportWidgetSettings.Find(WidgetToFind);
    }

    return nullptr;
}

EMouseLockMode UMultiplayerSettings::GetMouseLockMode()
{
    if (const UMultiplayerSettings* Settings = GetGenericSettings()) return Settings->MouseLockPreference;
    return EMouseLockMode::DoNotLock;
}

TSoftObjectPtr<UWorld> UMultiplayerSettings::GetLobbyLevel()
{
    if (const UMultiplayerSettings* Settings = GetGenericSettings()) return Settings->LobbyWorld;
    return TSoftObjectPtr<UWorld>(nullptr);
}

FString UMultiplayerSettings::GetAdditionalLobbyTravelOptions()
{
    if (const UMultiplayerSettings* Settings = GetGenericSettings()) return Settings->AdditionalLobbyTravelOptions;
    return "";
}

float UMultiplayerSettings::GetSessionRefreshTime()
{
    if (const UMultiplayerSettings* Settings = GetGenericSettings()) return Settings->SessionRefreshTime;
    return 1.0f;
}

int32 UMultiplayerSettings::GetNumSessionsToFindPerSearch()
{
    if (const UMultiplayerSettings* Settings = GetGenericSettings()) return FMath::Clamp(Settings->NumSessionsToFindPerSearch, 1, MaxSessionToFindPerSearch);
    return 10;
}

int32 UMultiplayerSettings::GetMaxSessionNameLength()
{
    if (const UMultiplayerSettings* Settings = GetGenericSettings()) return FMath::Max(1, Settings->MaxSessionNameLength);
    return 12;
}

TSoftObjectPtr<UWorld> UMultiplayerSettings::GetGameWorld()
{
    if (const UMultiplayerSettings* Settings = GetGenericSettings()) return Settings->GameWorld;
    return TSoftObjectPtr<UWorld>(nullptr);
}

int32 UMultiplayerSettings::GetNumHunters()
{
    if (const UMultiplayerSettings* Settings = GetGenericSettings()) return Settings->HunterToPropRatio.NumHunters;
    return 1;
}

int32 UMultiplayerSettings::GetNumProps()
{
    if (const UMultiplayerSettings* Settings = GetGenericSettings()) return Settings->HunterToPropRatio.NumProps;
    return 1;
}

int32 UMultiplayerSettings::GetTotalPlayerNum()
{
    if (const UMultiplayerSettings* Settings = GetGenericSettings()) return Settings->HunterToPropRatio.GetTotal();
    return 2;
}

EStartPreference UMultiplayerSettings::GetPlayerStartPreference()
{
    if (const UMultiplayerSettings* Settings = GetGenericSettings()) return Settings->PlayerStartPreference;
    return EStartPreference::CustomStarts;
}

float UMultiplayerSettings::GetCountdownTimerUpdateInterval()
{
    if (const UMultiplayerSettings* Settings = GetGenericSettings()) return Settings->CountdownTimerUpdateInterval;
    return 1.0f;
}

float UMultiplayerSettings::GetGamePhaseDuration(EGamePhase Phase)
{
    if (const UMultiplayerSettings* Settings = GetGenericSettings())
    {
        if (const float* FoundFloat = Settings->GamePhaseDuration.Find(Phase))
        {
            return *FoundFloat;
        }
    }
    
    return 1.0f;
}

FVector UMultiplayerSettings::GetMaxTargetableObjectSize()
{
    if (const UMultiplayerSettings* Settings = GetGenericSettings()) return Settings->MaxTargetableObjectSize;
    return FVector::OneVector;
}

bool UMultiplayerSettings::GetIsMeshForbidden(UStaticMesh* StaticMeshToCheck)
{
    if (const UMultiplayerSettings* Settings = GetGenericSettings())
    {
        return Settings->ForbiddenMeshes.ContainsByPredicate([StaticMeshToCheck](TSoftObjectPtr<UStaticMesh> Mesh)
        {
            // Note: If the StaticMeshToCheck was ever loaded, that means Mesh.Get() will be valid upon doing this check.
            if (Mesh.Get())
            {
                return Mesh.Get() == StaticMeshToCheck;
            }

            return false;
        });
    }
    
    return false;
}
