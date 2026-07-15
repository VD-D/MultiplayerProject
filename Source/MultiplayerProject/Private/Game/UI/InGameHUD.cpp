// Copyright Robert Uszynski

/* Class header. */
#include "Game/UI/InGameHUD.h"

/* Project includes. */
#include "Game/Actors/HunterCharacter.h"
#include "Game/Actors/PropCharacter.h"
#include "Shared/Libraries/MultiplayerLibrary.h"

/* Engine includes. */
#include "Components/TextBlock.h"

void UInGameHUD::UpdateGameTimeText(float NewGameTime) const
{
	if (!IsValid(GameTimeText)) return;
	
	const int32 IntGameTime = FMath::CeilToInt(NewGameTime);
	const int32 Seconds = IntGameTime % 60;
	const FString SecondsString = Seconds < 10 ? FString::Printf(TEXT("0%d"), Seconds) : FString::Printf(TEXT("%d"), Seconds);

	const int32 Minutes = FMath::FloorToInt(IntGameTime / 60.0f);
	const FString MinutesString = Minutes < 10 ? FString::Printf(TEXT("0%d"), Minutes) : FString::Printf(TEXT("%d"), Minutes);

	const FString Final = MinutesString + ":" + SecondsString;
	GameTimeText->SetText(FText::FromString(Final));
}

void UInGameHUD::UpdateCurrentHealthText(float NewCurrentHealth) const
{
	if (!IsValid(CurrentHealthText)) return;
	CurrentHealthText->SetText(FText::AsNumber(FMath::FloorToInt(NewCurrentHealth)));
}

void UInGameHUD::UpdateMaxHealthText(float NewMaxHealth) const
{
	if (!IsValid(MaxHealthText)) return;
	MaxHealthText->SetText(FText::AsNumber(FMath::FloorToInt(NewMaxHealth)));
}

void UInGameHUD::UpdatePlayerRoleText() const
{
	if (!IsValid(PlayerRoleText)) return;
	
	if (const APlayerController* PlayerController = UMultiplayerLibrary::GetLocalPlayerController(this); IsValid(PlayerController))
	{
		if (const APawn* Pawn = PlayerController->GetPawn(); IsValid(Pawn) && IsValid(Pawn->GetClass()))
		{
			if (Pawn->GetClass()->IsChildOf(AHunterCharacter::StaticClass()))
			{
				PlayerRoleText->SetText(HunterRoleText);
			}
			else if (Pawn->GetClass()->IsChildOf(APropCharacter::StaticClass()))
			{
				PlayerRoleText->SetText(PropRoleText);
			}

			return;
		}
	}

	PlayerRoleText->SetText(FText::GetEmpty());
}
