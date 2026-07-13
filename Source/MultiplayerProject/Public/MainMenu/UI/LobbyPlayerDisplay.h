// Copyright Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyPlayerDisplay.generated.h"

class UTextBlock;
/**
 * Simple widget for displaying connected player names in lobby.
 */
UCLASS()
class MULTIPLAYERPROJECT_API ULobbyPlayerDisplay : public UUserWidget
{
	GENERATED_BODY()
#pragma region Widget
protected:
	/* Displays player name. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> PlayerNameText;
#pragma endregion Widget

#pragma region Construction
public:
	/**
	 * Constructs a new LobbyPlayerDisplay.
	 * @param OwnerWidget The widget which acts as the outer of this widget.
	 * @param LobbyPlayerDisplayClass Class to create new instance from.
	 * @param PlayerName Name to display.
	 * @return The newly constructed widget.
	 */
	static ULobbyPlayerDisplay* CreateLobbyPlayerDisplay(UUserWidget* OwnerWidget, TSubclassOf<ULobbyPlayerDisplay> LobbyPlayerDisplayClass, const FText& PlayerName);
#pragma endregion Construction
};
