// Copyright Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyWidget.generated.h"

class UButton;
class ULobbyPlayerDisplay;
class UTextBlock;
class UVerticalBox;

/**
 * Simple lobby widget which displays connected players and allows the server to server travel to the main level
 * once the lobby is filled up.
 */
UCLASS()
class MULTIPLAYERPROJECT_API ULobbyWidget : public UUserWidget
{
	GENERATED_BODY()
#pragma region Widget
protected:
	/* This button only appears on the server (host) when the lobby is filled up. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> StartGameButton;

	/* Vertical box which holds names of all connected players. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UVerticalBox> PlayerNamesVerticalBox;

	/* Displays max number of players. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> MaxNumPlayersText;

	/* Displays current number of players. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> CurrentNumPlayersText;

	/* Class for displaying player names. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lobby Widget")
	TSubclassOf<ULobbyPlayerDisplay> LobbyPlayerDisplayClass;
#pragma endregion Widget

#pragma region Construction
public:
	/**
	 * Updates player names box, as well as max and current player displays.
	 */
	UFUNCTION(BlueprintCallable, Category = "Lobby Widget")
	void RefreshLobbyWidgetDisplay(int32 CurrentNumPlayers, int32 MaxNumPlayers);

protected:
	/**
	 * Binds delegates.
	 */
	virtual void NativeConstruct() override;

private:
	/**
	 * Tries to start the game. Only works on server (and is only displayed on server).
	 */
	UFUNCTION()
	void OnStartGameButtonClicked();
#pragma endregion Construction
};
