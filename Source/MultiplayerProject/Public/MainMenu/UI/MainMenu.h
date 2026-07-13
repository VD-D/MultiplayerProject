// Copyright Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenu.generated.h"

class UTextBlock;
class UButton;
class USessionDisplay;
class UVerticalBox;

/**
 * Allows for hosting and joining sessions.
 */
UCLASS()
class MULTIPLAYERPROJECT_API UMainMenu : public UUserWidget
{
	GENERATED_BODY()
#pragma region Widget
protected:
	/* Creates a new session. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget", meta = (BindWidget))
	TObjectPtr<UButton> HostSessionButton;

	/* Exits game. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget", meta = (BindWidget))
	TObjectPtr<UButton> QuitGameButton;

	/* Populated by available sessions to connect to. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget", meta = (BindWidget))
	TObjectPtr<UVerticalBox> SessionDisplayBox;

	/* How long until we search for new sessions. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget", meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> RefreshSessionsTimeText;
#pragma endregion Widget

#pragma region Config
	/* Class of session display widget to populate SessionDisplayBox. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Main Menu")
	TSubclassOf<USessionDisplay> SessionDisplayWidgetClass;

private:
	/* Used to get time remaining. */
	FTimerHandle TimerHandle;
#pragma endregion Config

#pragma region Construction
public:
	/**
	 * Updates SessionDisplayBox with newly available sessions.
	 */
	UFUNCTION(BlueprintCallable, Category = "Main Menu")
	void RefreshSessionsDisplayBox();
	
protected:
	/**
	 * Binds delegates. Generates timer for to refresh sessions.
	 */
	virtual void NativeConstruct() override;

	/**
	 * Updates time remaining text.
	 * @param MyGeometry Unused.
	 * @param InDeltaTime Passed to timer.
	 */
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	/**
	 * Stops timer.
	 */
	virtual void NativeDestruct() override;

private:
	/**
	 * Configures the timer and launches it.
	 */
	void ConfigureTimer();
	
	/**
	 * Generates a new session and displays lobby widget.
	 */
	UFUNCTION()
	void OnHostSessionClicked();

	/**
	 * Exits the game.
	 */
	UFUNCTION()
	void OnQuitClicked();
#pragma endregion Construction

#pragma region Blueprint
public:
	/**
	 * Called when searching for sessions begins.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Main Menu")
	void OnBeginSearchForSessions();

	/**
	 * Called when searching for sessions ends.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Main Menu")
	void OnEndSearchForSessions();
#pragma endregion Blueprint
};
