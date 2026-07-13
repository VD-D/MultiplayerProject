// Copyright Robert Uszynski

/* Class header. */
#include "Shared/Subsystems/UIManager.h"

/* Project includes. */
#include "Core/Settings/MultiplayerSettings.h"
#include "Shared/Libraries/Logging.h"
#include "Shared/Types/SharedStructs.h"

/* Engine includes. */
#include "Blueprint/UserWidget.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"

UUIManager* UUIManager::GetUIManager(const UObject* WorldContextObject)
{
    if (GEngine == nullptr) return nullptr;

    if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
    {
        if (const UGameInstance* GameInstance = World->GetGameInstance()) return GameInstance->GetSubsystem<UUIManager>();
    }

    return nullptr;
}

UUserWidget* UUIManager::GetLoadedWidget(const UObject* WorldContextObject, EViewportWidget ViewportWidgetToGet)
{
    if (UUIManager* UIManager = GetUIManager(WorldContextObject))
    {
        if (UUserWidget** FoundWidget = UIManager->LoadedWidgets.Find(ViewportWidgetToGet))
        {
            if (FoundWidget != nullptr) return *FoundWidget;
        }
    }

    return nullptr;
}

bool UUIManager::GetIsWidgetLoading(const UObject* WorldContextObject, EViewportWidget ViewportWidgetToGet)
{
    if (const UUIManager* UIManager = GetUIManager(WorldContextObject))
    {
        return UIManager->WidgetsPendingLoad.Contains(ViewportWidgetToGet);
    }

    return false;
}

void UUIManager::SetLoadedWidgetVisibility(const UObject* WorldContextObject, EViewportWidget ViewportWidget, ESlateVisibility Visibility)
{
    if (UUserWidget* LoadedWidget = GetLoadedWidget(WorldContextObject, ViewportWidget); IsValid(LoadedWidget))
    {
        LoadedWidget->SetVisibility(Visibility);
    }
}

bool UUIManager::LoadViewportWidget(const UObject* WorldContextObject, EViewportWidget ViewportWidgetToLoad)
{
    return LoadViewportWidget(WorldContextObject, ViewportWidgetToLoad, FOnWidgetLoaded());
}

bool UUIManager::LoadViewportWidget(const UObject* WorldContextObject, EViewportWidget ViewportWidgetToLoad, const FOnWidgetLoaded& OnLoadedCallback)
{
    if (UUserWidget* AlreadyLoadedWidget = GetLoadedWidget(WorldContextObject, ViewportWidgetToLoad))
    {
        OnLoadedCallback.ExecuteIfBound(AlreadyLoadedWidget);;
        return true;
    }
    
    if (UUIManager* UIManager = GetUIManager(WorldContextObject))
    {
        if (const FViewportWidgetSettings* Settings = UMultiplayerSettings::GetViewportSetting(ViewportWidgetToLoad))
        {
            TWeakObjectPtr WeakUIManager = UIManager;
            
            FStreamableDelegate StreamableDelegate;
            StreamableDelegate.BindLambda([WeakUIManager, Settings, OnLoadedCallback, ViewportWidgetToLoad]{
                if (WeakUIManager.IsValid() && Settings != nullptr) WeakUIManager.Get()->OnWidgetLoaded(ViewportWidgetToLoad, Settings->ViewportWidget, Settings->ZOrder, OnLoadedCallback);
            });

            UIManager->WidgetsPendingLoad.Emplace(ViewportWidgetToLoad);
                
            FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
            StreamableManager.RequestAsyncLoad(Settings->ViewportWidget.ToSoftObjectPath(), StreamableDelegate);
            return true;
        }

        ULogging::LogVerboseError("UIManager", "UUIManager::LoadViewportWidget", FString::Printf(TEXT("Could not find viewport widget settings for %s!"), *UEnum::GetValueAsString(ViewportWidgetToLoad)));
    }
    else
    {
        ULogging::LogVerboseError("UIManager", "UUIManager::LoadViewportWidget", "Failed to get UI Manager!");
    }

    return false;
}

void UUIManager::RemoveViewportWidget(const UObject* WorldContextObject, EViewportWidget ViewportWidgetToRemove)
{
    if (UUIManager* UIManager = GetUIManager(WorldContextObject))
    {
        if (UUserWidget** FoundWidget = UIManager->LoadedWidgets.Find(ViewportWidgetToRemove))
        {
            if (FoundWidget != nullptr)
            {
                UUserWidget* Widget = *FoundWidget;
                UIManager->LoadedWidgets.Remove(ViewportWidgetToRemove);
                Widget->RemoveFromParent();
            }
        }
    }
}

void UUIManager::OnWidgetLoaded(EViewportWidget ViewportWidgetToLoad, TSoftClassPtr<UUserWidget> WidgetPath, int32 ZOrder, const FOnWidgetLoaded& OnLoadedCallback)
{
    WidgetsPendingLoad.RemoveSwap(ViewportWidgetToLoad);
    
    if (UClass* WidgetClass = WidgetPath.Get())
    {
        if (UUserWidget* NewUserWidget = CreateWidget<UUserWidget>(GetWorld(), WidgetClass, FName(WidgetPath.ToString())))
        {
            NewUserWidget->AddToViewport(ZOrder);
            LoadedWidgets.Emplace(ViewportWidgetToLoad, NewUserWidget);
            OnLoadedCallback.ExecuteIfBound(NewUserWidget);
        }
        else
        {
            ULogging::LogVerboseError("UIManager", "UUIManager::OnWidgetLoaded", "Failed to construct widget from class!");
        }
    }
    else
    {
        ULogging::LogVerboseError("UIManager", "UUIManager::OnWidgetLoaded", "Widget path was still invalid after loading!");
    }
}