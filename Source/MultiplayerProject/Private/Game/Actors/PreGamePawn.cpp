// Copyright Robert Uszynski

/* Class header. */
#include "Game/Actors/PreGamePawn.h"

/* Project includes. */
#include "Core/Actors/MultiplayerGameMode.h"
#include "Core/Settings/MultiplayerSettings.h"
#include "Shared/Libraries/Logging.h"

/* Engine includes. */
#include "Camera/CameraComponent.h"

APreGamePawn::APreGamePawn()
{
 	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	RootComponent = Camera;
}

void APreGamePawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (HasAuthority() && IsLocallyControlled())
	{
		if (!TryStartGame())
		{
			if (AMultiplayerGameMode* MultiplayerGameMode = AMultiplayerGameMode::GetMultiplayerGameMode(this); IsValid(MultiplayerGameMode))
			{
				MultiplayerGameMode->OnPlayerConnected.AddDynamic(this, &APreGamePawn::OnPlayerConnected);
			}
		}
	}
}

void APreGamePawn::OnPlayerConnected(APlayerController* NewController, int32 CurrentNumPlayers, int32 MaxNumPlayers)
{
	if (const bool bSuccess = TryStartGame(); bSuccess && UMultiplayerSettings::GetEnableOptionalLogging())
	{
		ULogging::LogMessageToConsole("Successfully started new game after all players connected!");
	}
}

bool APreGamePawn::TryStartGame() const
{
	if (HasAuthority() && IsLocallyControlled())
	{
		if (AMultiplayerGameMode* MultiplayerGameMode = AMultiplayerGameMode::GetMultiplayerGameMode(this); IsValid(MultiplayerGameMode))
		{
			return MultiplayerGameMode->TryToStartGame();
		}
		
		ULogging::LogVerboseError(GetName(), "APreGamePawn::TryStartGame", "Game mode is not of type AMultiplayerGameMode!");
	}

	return false;
}
