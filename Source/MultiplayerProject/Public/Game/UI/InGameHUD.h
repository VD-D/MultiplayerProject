// Copyright Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Shared/Types/SharedEnums.h"
#include "InGameHUD.generated.h"

class UTextBlock;

/**
 * In-Game HUD displaying game time, health and role. 
 */
UCLASS()
class MULTIPLAYERPROJECT_API UInGameHUD : public UUserWidget
{
	GENERATED_BODY()
#pragma region Widget
protected:
	/* Displays game time. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> GameTimeText;

	/* Displays the player's role. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> PlayerRoleText;

	/* Displays player's current health. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> CurrentHealthText;

	/* Displays player's max health. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> MaxHealthText;
#pragma endregion Widget

#pragma region Config
	/* Player-friendly display name if they are a hunter. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "In Game HUD")
	FText HunterRoleText;

	/* Player-friendly display name if they are a prop. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "In Game HUD")
	FText PropRoleText;
#pragma endregion Config

#pragma region Construction
public:
	/**
	 * Updates GameTimeText display.
	 * @param NewGameTime Current game time to display.
	 */
	UFUNCTION(BlueprintCallable, Category = "In Game HUD")
	void UpdateGameTimeText(float NewGameTime) const;

	UFUNCTION(BlueprintCallable, Category = "In Game HUD")
	void UpdateCurrentHealthText(float NewCurrentHealth) const;

	UFUNCTION(BlueprintCallable, Category = "In Game HUD")
	void UpdateMaxHealthText(float NewMaxHealth) const;

	/**
	 * Shows the player's role, based on their pawn's class.
	 */
	UFUNCTION(BlueprintCallable, Category = "In Game HUD")
	void UpdatePlayerRoleText() const;
#pragma endregion Construction

#pragma region Blueprint
	/**
	 * Called whenever the game phase updates.
	 * @param NewGamePhase Current game phase.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "In Game HUD")
	void OnGamePhaseChanged(EGamePhase NewGamePhase);
#pragma endregion Blueprint
};
