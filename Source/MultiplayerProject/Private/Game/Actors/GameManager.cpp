// Copyright Robert Uszynski

/* Class header. */
#include "Game/Actors/GameManager.h"

/* Project includes. */
#include "Core/Actors/MultiplayerGameController.h"
#include "Core/Actors/MultiplayerGameMode.h"
#include "Core/Settings/MultiplayerSettings.h"
#include "Game/Actors/HunterCharacter.h"
#include "Game/Actors/HunterPropStart.h"
#include "Game/Actors/PropCharacter.h"
#include "Shared/Libraries/Logging.h"
#include "Shared/Libraries/MultiplayerLibrary.h"
#include "Shared/Subsystems/SessionSubsystem.h"

/* Engine includes. */
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"
#include "Net/UnrealNetwork.h"

AGameManager::AGameManager()
{
	SideWhichWon = ERoleType::Unknown;
}

AGameManager* AGameManager::CreateInstance(const UObject* WorldContextObject, TSubclassOf<AGameManager> GameManagerClass)
{
	if (GEngine == nullptr || GameManagerClass.Get() == nullptr) return nullptr;

	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull); IsValid(World))
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		if (AGameManager* NewInstance = World->SpawnActor<AGameManager>(GameManagerClass); IsValid(NewInstance))
		{
			NewInstance->AssignRolesAndPossessControllers();
			return NewInstance;
		}
	}

	return nullptr;
}

void AGameManager::AssignRolesAndPossessControllers()
{
	if (!HasAuthority() || !IsValid(GetWorld())) return;

	// Step 0. Update game phase.
	CurrentGamePhase = EGamePhase::GameCountdown;
	OnRep_CurrentGamePhase();

	// Step 1. Get all controllers and shuffle the resulting controller, to ensure they are in random order.
	// (This means roles are assigned randomly).
	TArray<APlayerController*> Controllers;
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PlayerController = Iterator->Get();
		if (!IsValid(PlayerController)) continue;
		Controllers.Emplace(PlayerController);
	}

	if (Controllers.Num() > 1)
	{
		const int32 LastIndex = Controllers.Num() - 1;
		for (int32 i = 0; i <= LastIndex; ++i)
		{
			if (const int32 Index = FMath::RandRange(i, LastIndex); i != Index)
			{
				Controllers.Swap(i, Index);
			}
		}
	}

	// Step 2: Get number of props and hunters, spawn them and have the player controller possess them.
	const int32 NumProps = FMath::Max(1, UMultiplayerSettings::GetNumProps());
	const int32 NumHunters = FMath::Max(1, UMultiplayerSettings::GetNumHunters());

	if (NumProps + NumHunters != Controllers.Num())
	{
		ULogging::LogVerboseWarning(
			GetName(),
			"AGameManager::AssignRolesAndPossessControllers",
			FString::Printf(TEXT("There are %d total props and hunters, but %d controllers!"), NumProps + NumHunters, Controllers.Num())
			);
	}

	TArray<APlayerStart*> PlayerStarts;
	TArray<AHunterPropStart*> HunterPropStarts;
	const EStartPreference StartPreference = UMultiplayerSettings::GetPlayerStartPreference();
	

	ERoleType CurrentRoleType = ERoleType::Hunter;
	int32 Index = 0;

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	for (const auto PlayerController : Controllers)
	{
		AMultiplayerGameController* GameController = Cast<AMultiplayerGameController>(PlayerController);
		if (!IsValid(GameController))
		{
			ULogging::LogVerboseError(GetName(), "AGameManager::AssignRolesAndPossessControllers", "Found a controller not of type AMultiplayerGameController! Could not assign role!");
			continue;
		}
		
		// Switch to assigning props once we have assigned all hunters.
		if (Index == NumHunters && CurrentRoleType == ERoleType::Hunter)
		{
			Index = 0;
			CurrentRoleType = ERoleType::Prop;
			HunterPropStarts.Empty();
		}

		const FTransform SpawnTransform = StartPreference == EStartPreference::CustomStarts ? GetSpawnTransformFromHunterPropStart(CurrentRoleType, HunterPropStarts) : GetSpawnTransformFromPlayerStart(PlayerStarts);
		Index += 1;
		
		GameController->SetRoleType(CurrentRoleType);
		
		if (CurrentRoleType == ERoleType::Hunter)
		{
			if (AHunterCharacter* HunterCharacter = GetWorld()->SpawnActor<AHunterCharacter>(AMultiplayerGameMode::GetHunterCharacterClass(this), SpawnTransform, SpawnParameters); IsValid(HunterCharacter))
			{
				PlayerController->Possess(HunterCharacter);
				HunterCharacter->SetEnableCharacterInput(false);
			}
		}
		else if (CurrentRoleType == ERoleType::Prop)
		{
			if (APropCharacter* PropCharacter = GetWorld()->SpawnActor<APropCharacter>(AMultiplayerGameMode::GetPropCharacterClass(this), SpawnTransform, SpawnParameters); IsValid(PropCharacter))
			{
				PlayerController->Possess(PropCharacter);
			}
		}
	}

	// Step 3. Begin the timer to go from pre-game countdown to in game.
	BeginTimerForPhase();
}

AGameManager* AGameManager::GetGameManager(const UObject* WorldContextObject)
{
	if (GEngine == nullptr) return nullptr;

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull); IsValid(World))
	{
		for (TActorIterator<AGameManager> ActorIterator(World); ActorIterator; ++ActorIterator)
		{
			AGameManager* GameManager = *ActorIterator;
			if (!IsValid(GameManager)) continue;

			return GameManager;
		}
	}

	return nullptr;
}

void AGameManager::CheckGameFinished(const UObject* WorldContextObject)
{
	if (AGameManager* CurrentInstance = GetGameManager(WorldContextObject))
	{
		if (!CurrentInstance->HasAuthority()) return;

		bool bAnyHunterLeft = false;
		for (TActorIterator<AHunterCharacter> HunterItr(CurrentInstance->GetWorld()); HunterItr; ++HunterItr)
		{
			if (const AHunterCharacter* HunterCharacter = *HunterItr; !IsValid(HunterCharacter) || HunterCharacter->GetCurrentHealth() <= 0.0f) continue;

			bAnyHunterLeft = true;
			break;
		}

		if (!bAnyHunterLeft)
		{
			CurrentInstance->SetGameFinishedWithResult(ERoleType::Prop);
			return;
		}

		bool bAnyPropLeft = false;
		for (TActorIterator<APropCharacter> PropItr(CurrentInstance->GetWorld()); PropItr; ++PropItr)
		{
			if (const APropCharacter* PropCharacter = *PropItr; !IsValid(PropCharacter) || PropCharacter->GetCurrentHealth() <= 0.0f) continue;

			bAnyPropLeft = true;
			break;
		}

		if (!bAnyPropLeft)
		{
			CurrentInstance->SetGameFinishedWithResult(ERoleType::Hunter);
		}
	}
}

void AGameManager::GetPlayerStarts(const UObject* WorldContextObject, TArray<APlayerStart*>& PlayerStarts)
{
	PlayerStarts.Empty();

	if (GEngine == nullptr) return;
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull); IsValid(World))
	{
		for (TActorIterator<APlayerStart> Iterator(World); Iterator; ++Iterator)
		{
			APlayerStart* PlayerStart = *Iterator;
			if (!IsValid(PlayerStart)) continue;

			PlayerStarts.Emplace(PlayerStart);
		}
	}
}

void AGameManager::GetHunterPropStartsOfType(const UObject* WorldContextObject, ERoleType RoleType, TArray<AHunterPropStart*>& HunterPropStarts)
{
	HunterPropStarts.Empty();
	
	if (GEngine == nullptr) return;
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull); IsValid(World))
	{
		for (TActorIterator<AHunterPropStart> Iterator(World); Iterator; ++Iterator)
		{
			AHunterPropStart* HunterPropStart = *Iterator;
			if (!IsValid(HunterPropStart)) continue;

			if (HunterPropStart->GetStartRoleType() == RoleType)
			{
				HunterPropStarts.Emplace(HunterPropStart);
			}
		}	
	}
}

void AGameManager::SetGameFinishedWithResult(ERoleType WinningSide)
{
	SideWhichWon = WinningSide;

	if (CurrentGamePhase != EGamePhase::Scoreboard)
	{
		CurrentGamePhase = EGamePhase::InGame;
		OnTimerForPhaseEnded();
	}
}

FTransform AGameManager::GetSpawnTransformFromPlayerStart(TArray<APlayerStart*>& PlayerStarts) const
{
	FTransform OutTransform = FTransform::Identity;
	
	if (PlayerStarts.IsEmpty())
	{
		GetPlayerStarts(this, PlayerStarts);
	}

	if (!PlayerStarts.IsEmpty())
	{
		const int32 RandomIndex = FMath::RandRange(0, PlayerStarts.Num() - 1);
		OutTransform.SetLocation(PlayerStarts[RandomIndex]->GetActorLocation());
		OutTransform.SetRotation(FQuat(PlayerStarts[RandomIndex]->GetActorRotation()));
		PlayerStarts.RemoveAtSwap(RandomIndex);
	}
	else
	{
		ULogging::LogVerboseWarning(
			GetName(),
			"AGameManager::GetSpawnTransformFromHunterPropStart",
			"Found no player starts! Using default transform."
			);
	}
	
	return OutTransform;
}

FTransform AGameManager::GetSpawnTransformFromHunterPropStart(ERoleType RoleType, TArray<AHunterPropStart*>& HunterPropStarts) const
{
	FTransform OutTransform = FTransform::Identity;
	
	if (HunterPropStarts.IsEmpty())
	{
		GetHunterPropStartsOfType(this, RoleType, HunterPropStarts);
	}

	if (!HunterPropStarts.IsEmpty())
	{
		const int32 RandomIndex = FMath::RandRange(0, HunterPropStarts.Num() - 1);
		OutTransform.SetLocation(HunterPropStarts[RandomIndex]->GetActorLocation());
		OutTransform.SetRotation(FQuat(HunterPropStarts[RandomIndex]->GetActorRotation()));
		HunterPropStarts.RemoveAtSwap(RandomIndex);
	}
	else
	{
		ULogging::LogVerboseWarning(
			GetName(),
			"AGameManager::GetSpawnTransformFromHunterPropStart",
			FString::Printf(TEXT("Found no starts of for start type %s! Using default transform."), *UEnum::GetValueAsString(RoleType))
			);
	}
	
	return OutTransform;
}

void AGameManager::BeginTimerForPhase()
{
	if (!HasAuthority() || CountdownTimeTimerHandle.IsValid() || GamePhaseTimerHandle.IsValid()) return;

	FTimerDelegate CountdownDelegate;
	CountdownDelegate.BindUObject(this, &AGameManager::UpdateCountdownTime);

	FTimerDelegate GamePhaseDelegate;
	GamePhaseDelegate.BindUObject(this, &AGameManager::OnTimerForPhaseEnded);

	const float CountdownTimeRefreshTime = FMath::Max(UMultiplayerSettings::GetCountdownTimerUpdateInterval(), 1.0f);
	const float PhaseDuration = FMath::Max(UMultiplayerSettings::GetGamePhaseDuration(CurrentGamePhase), 1.0f);

	CountdownTime = PhaseDuration;
	OnRep_CountdownTime();

	GetWorldTimerManager().SetTimer(CountdownTimeTimerHandle, CountdownDelegate, CountdownTimeRefreshTime, true);
	GetWorldTimerManager().SetTimer(GamePhaseTimerHandle, GamePhaseDelegate, PhaseDuration, false);
}

void AGameManager::UpdateCountdownTime()
{
	if (!HasAuthority()) return;
		
	CountdownTime = GetWorldTimerManager().GetTimerRemaining(GamePhaseTimerHandle);
	OnRep_CountdownTime();
}

void AGameManager::OnTimerForPhaseEnded()
{
	if (!HasAuthority()) return;
	
	GetWorldTimerManager().ClearTimer(CountdownTimeTimerHandle);
	GetWorldTimerManager().ClearTimer(GamePhaseTimerHandle);

	CountdownTime = 0.0f;
	OnRep_CountdownTime();

	if (CurrentGamePhase == EGamePhase::GameCountdown)
	{
		CurrentGamePhase = EGamePhase::InGame;
		OnRep_CurrentGamePhase();

		for (TActorIterator<AHunterCharacter> ActorIterator(GetWorld()); ActorIterator; ++ActorIterator)
		{
			AHunterCharacter* HunterCharacter = *ActorIterator;
			if (!IsValid(HunterCharacter)) continue;

			HunterCharacter->SetEnableCharacterInput(true);
		}
	}
	else if (CurrentGamePhase == EGamePhase::InGame)
	{
		// This condition is only met if the in-game timer finishes without either side having won.
		// By default, the props win; if the above condition is met, this means at least one prop is still alive.
		if (SideWhichWon == ERoleType::Unknown)
		{
			SideWhichWon = ERoleType::Prop;
		}
		
		CurrentGamePhase = EGamePhase::Scoreboard;
		OnRep_CurrentGamePhase();
		
		for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			if (APlayerController* PlayerController = Iterator->Get(); IsValid(PlayerController))
			{
				if (AMultiplayerGameController* GameController = Cast<AMultiplayerGameController>(PlayerController); IsValid(GameController))
				{
					GameController->SetRoleType(ERoleType::Unknown);
					GameController->DisplayScoreboardForDuration(SideWhichWon, UMultiplayerSettings::GetGamePhaseDuration(CurrentGamePhase));
				}
			}
		}
	}
	else
	{
		if (AMultiplayerGameMode* MultiplayerGameMode = AMultiplayerGameMode::GetMultiplayerGameMode(this))
		{
			MultiplayerGameMode->GameManagerInstance = nullptr;
		}

		// Because this is only ever called on authority, the "owning client" is the server.
		// This destroys the session on the server, causing all clients to get booted to the main menu.
		EndGameSession();
		return;
	}

	BeginTimerForPhase();
}

void AGameManager::EndGameSession_Implementation()
{
	UMultiplayerLibrary::DisconnectLocalPlayer(this);
}

void AGameManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGameManager, CountdownTime);
	DOREPLIFETIME(AGameManager, CurrentGamePhase);
}

void AGameManager::OnRep_CountdownTime()
{
	OnCountdownTimeTick.Broadcast(CountdownTime);
}

void AGameManager::OnRep_CurrentGamePhase()
{
	OnGamePhaseUpdated.Broadcast(CurrentGamePhase);
}
