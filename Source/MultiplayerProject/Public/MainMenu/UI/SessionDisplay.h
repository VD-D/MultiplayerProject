// Copyright Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "OnlineSessionSettings.h"
#include "Blueprint/UserWidget.h"
#include "SessionDisplay.generated.h"

class UButton;
class UTextBlock;

/**
 * Widget for connecting to an existing session.
 */
UCLASS()
class MULTIPLAYERPROJECT_API USessionDisplay : public UUserWidget
{
	GENERATED_BODY()
#pragma region Widget
protected:
	/* Connects to a given session. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget", meta = (BindWidget))
	TObjectPtr<UButton> ConnectToSessionButton;

	/* The actual name of the session. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget", meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> SessionNameText;

	/* Displays how many players can join. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget", meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> MaxPlayersText;

	/* Displays how many players currently have joined. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget", meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> CurrentPlayersText;

	/* Displays session ping. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget", meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> PingText;
#pragma endregion Widget

#pragma region Internal
private:
	/* The session this widget displays. */
	FOnlineSessionSearchResult SessionSearchResult;
#pragma endregion Internal

#pragma region Construction
public:
	/**
	 * Utility to create a session display widget.
	 * @param OwningWidget
	 * @param SessionDisplayClass Class to create widget from.
	 * @param SearchResult The search result for this widget to display.
	 * @return Newly constructed session display widget.
	 */
	static USessionDisplay* CreateSessionDisplay(UUserWidget* OwningWidget, TSubclassOf<USessionDisplay> SessionDisplayClass, const FOnlineSessionSearchResult& SearchResult);

	/**
	 * Updates optional visual components of the display.
	 * NOTE: const because it does not change the state of the widget itself.
	 */
	void RefreshVisuals() const;
	
protected:
	/**
	 * Binds delegates
	 */
	virtual void NativeConstruct() override;

private:
	/**
	 * Performs connect to session logic.
	 */
	UFUNCTION()
	void OnConnectToSessionClicked();
#pragma endregion Construction
};
