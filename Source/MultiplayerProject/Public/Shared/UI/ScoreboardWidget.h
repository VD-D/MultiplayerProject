// Copyright Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Shared/Types/SharedEnums.h"
#include "ScoreboardWidget.generated.h"

/**
 * Endgame widget which is displayed when the match ends.
 */
UCLASS()
class MULTIPLAYERPROJECT_API UScoreboardWidget : public UUserWidget
{
	GENERATED_BODY()
#pragma region BlueprintAPI
public:
	/**
	 * Called after a side is declared a winner.
	 * @param WinningSide The side which won.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Scoreboard")
	void OnSideDeclaredWinner(ERoleType WinningSide);
#pragma endregion BlueprintAPI
};
