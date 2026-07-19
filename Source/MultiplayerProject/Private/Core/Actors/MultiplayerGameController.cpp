// Copyright Robert Uszynski

/* Class header. */
#include "Core/Actors/MultiplayerGameController.h"

/* Project includes. */
#include "Shared/Libraries/Logging.h"
#include "Shared/Subsystems/UIManager.h"

/* Engine includes. */
#include "EnhancedInputSubsystems.h"
#include "Core/Settings/MultiplayerSettings.h"
#include "GameFramework/PlayerState.h"
#include "Shared/UI/ScoreboardWidget.h"

void AMultiplayerGameController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalController()) return;

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(InputMappingContext, 0);
	}
	else
	{
		ULogging::LogVerboseError(GetName(), "AMultiplayerGameController::SetupInputComponent", "Failed to get EnhancedInputLocalPlayerSubsystem!");
	}
}

void AMultiplayerGameController::SetSpectatorState()
{
	if (IsValid(PlayerState)) PlayerState->SetIsSpectator(true);
	ChangeState(NAME_Spectating);
	ClientGotoState(NAME_Spectating);
}

void AMultiplayerGameController::SetRoleType(ERoleType NewRole)
{
	if (UMultiplayerSettings::GetEnableOptionalLogging())
	{
		ULogging::LogMessageToConsole(FString::Printf(TEXT("%s : Set new role = %s"), *GetName(), *UEnum::GetValueAsString(NewRole)));
	}
	
	if (HasAuthority())
	{
		CurrentRole = NewRole;
	}
}

void AMultiplayerGameController::DisplayScoreboardForDuration_Implementation(ERoleType WinningSide, float Duration)
{
	const bool bSuccess = UUIManager::LoadViewportWidget(this, EViewportWidget::Scoreboard, FOnWidgetLoaded::CreateLambda([WinningSide](UUserWidget* Widget)
	{
		if (UScoreboardWidget* ScoreboardWidget = Cast<UScoreboardWidget>(Widget))
		{
			ScoreboardWidget->OnSideDeclaredWinner(WinningSide);
		}
	}));

	if (bSuccess)
	{
		TWeakObjectPtr WeakThis = this;
		
		FTimerHandle TimerHandle;
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindLambda([WeakThis]()
		{
			if (WeakThis.IsValid())
			{
				UUIManager::RemoveViewportWidget(WeakThis.Get(), EViewportWidget::Scoreboard);
			}
		});
		
		GetWorldTimerManager().SetTimer(TimerHandle, TimerDelegate, FMath::Max(1.0f, Duration), false);
	}
}
