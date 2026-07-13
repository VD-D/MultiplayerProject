// Copyright Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Shared/Types/SharedEnums.h"
#include "UIManager.generated.h"

class UUserWidget;

DECLARE_DELEGATE_OneParam(FOnWidgetLoaded, UUserWidget*);

/**
 * Manager class which loads and displays widgets in viewport.
 */
UCLASS()
class MULTIPLAYERPROJECT_API UUIManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
#pragma region Internal
	/* Map of all widgets which have been loaded in the viewport. */
	UPROPERTY()
	TMap<EViewportWidget, UUserWidget*> LoadedWidgets;

	/* Tracks widgets which are in the process of being loaded. */
	TArray<EViewportWidget> WidgetsPendingLoad;
#pragma endregion Internal

#pragma region Accessors
public:
	/**
	 * @param WorldContextObject Gets world.
	 * @return Manager class which loads and displays widgets in viewport.
	 */
	UFUNCTION(BlueprintPure, Category = "UI Manager", meta = (WorldContext = "WorldContextObject"))
	static UUIManager* GetUIManager(const UObject* WorldContextObject);

	/**
	 * @param WorldContextObject Gets world.
	 * @param ViewportWidgetToGet Which widget we wish to get and add to viewport.
	 * @return If the viewport widget was ever loaded, it will be returned. Otherwise, return null.
	 */
	UFUNCTION(BlueprintPure, Category = "UI Manager", meta = (WorldContext = "WorldContextObject"))
	static UUserWidget* GetLoadedWidget(const UObject* WorldContextObject, EViewportWidget ViewportWidgetToGet);

	template<class T>
	static T* GetLoadedWidget(const UObject* WorldContextObject, EViewportWidget ViewportWidgetToGet)
	{
		return Cast<T>(GetLoadedWidget(WorldContextObject, ViewportWidgetToGet));
	}

	/**
	 * @param WorldContextObject Gets world.
	 * @param ViewportWidgetToGet The widget we want to check for loading.
	 * @return True if the widget is still being loaded.
	 */
	UFUNCTION(BlueprintPure, Category = "UI Manager", meta = (WorldContext = "WorldContextObject"))
	static bool GetIsWidgetLoading(const UObject* WorldContextObject, EViewportWidget ViewportWidgetToGet);

	/**
	 * Changes visibility for a widget, assuming it has been loaded beforehand.
	 * @param WorldContextObject Gets world.
	 * @param ViewportWidget The viewport widget to manipulate visibility for.
	 * @param Visibility The new visibility.
	 */
	UFUNCTION(BlueprintCallable, Category = "UI Manager", meta = (WorldContext = "WorldContextObject"))
	static void SetLoadedWidgetVisibility(const UObject* WorldContextObject, EViewportWidget ViewportWidget, ESlateVisibility Visibility);
#pragma endregion Accessors

#pragma region WidgetManagement
	/**
	 * Begins loading process of widget, then adds it to the viewport.
	 * @param WorldContextObject Gets world.
	 * @param ViewportWidgetToLoad Key of widget definition we wish to load from settings.
	 * @return Whether loading sequence was successfully initiated.
	 */
	UFUNCTION(BlueprintCallable, Category = "UI Manager", meta = (WorldContext = "WorldContextObject"))
	static bool LoadViewportWidget(const UObject* WorldContextObject, EViewportWidget ViewportWidgetToLoad);

	/**
	 * Begins loading process of widget, then adds it to the viewport.
	 * @param WorldContextObject Gets world.
	 * @param ViewportWidgetToLoad Key of widget definition we wish to load from settings.
	 * @param OnLoadedCallback Function executed after loading has finished.
	 * @return Whether loading sequence was successfully initiated.
	 */
	static bool LoadViewportWidget(const UObject* WorldContextObject, EViewportWidget ViewportWidgetToLoad, const FOnWidgetLoaded& OnLoadedCallback);

	/**
	 * Removes a loaded widget from the viewport.
	 * @param WorldContextObject Gets world.
	 * @param ViewportWidgetToRemove Which widget to remove
	 */
	UFUNCTION(BlueprintCallable, Category = "UI Manager", meta = (WorldContext = "WorldContextObject"))
	static void RemoveViewportWidget(const UObject* WorldContextObject, EViewportWidget ViewportWidgetToRemove);

private:
	/**
	 * Callback after widget is loaded.
	 * @param ViewportWidgetToLoad Which viewport widget was loaded.
	 * @param WidgetPath This should now point to a valid widget.
	 * @param ZOrder Desired widget Z-Order.
	 * @param OnLoadedCallback Function executed after loading has finished.
	 */
	void OnWidgetLoaded(EViewportWidget ViewportWidgetToLoad, TSoftClassPtr<UUserWidget> WidgetPath, int32 ZOrder, const FOnWidgetLoaded& OnLoadedCallback);
#pragma endregion WidgetManagement
};
