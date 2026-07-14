// Copyright Robert Uszynski

/* Class header. */
#include "Game/Actors/GameManager.h"

/* Project includes. */
#include "Core/Settings/MultiplayerSettings.h"
#include "Game/Actors/HunterPropStart.h"
#include "Shared/Libraries/Logging.h"

/* Engine includes. */
#include "EngineUtils.h"
#include "Core/Actors/MultiplayerGameMode.h"
#include "Game/Actors/HunterCharacter.h"
#include "Game/Actors/PropCharacter.h"
#include "GameFramework/PlayerStart.h"

AGameManager::AGameManager()
{
	PrimaryActorTick.bCanEverTick = true;
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
		// We disable the input, since players should not be able to act until the start countdown timer expires.
		PlayerController->DisableInput(PlayerController);
		
		// Switch to assigning props once we have assigned all hunters.
		if (Index == NumHunters && CurrentRoleType == ERoleType::Hunter)
		{
			Index = 0;
			CurrentRoleType = ERoleType::Prop;
		}

		const FTransform SpawnTransform = StartPreference == EStartPreference::CustomStarts ? GetSpawnTransformFromHunterPropStart(CurrentRoleType, HunterPropStarts) : GetSpawnTransformFromPlayerStart(PlayerStarts);
		Index += 1;
		
		if (CurrentRoleType == ERoleType::Hunter)
		{
			if (AHunterCharacter* HunterCharacter = GetWorld()->SpawnActor<AHunterCharacter>(AMultiplayerGameMode::GetHunterCharacterClass(this), SpawnTransform, SpawnParameters); IsValid(HunterCharacter))
			{
				PlayerController->Possess(HunterCharacter);
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

void AGameManager::GetPlayerStarts(TArray<APlayerStart*>& PlayerStarts) const
{
	PlayerStarts.Empty();
	for (TActorIterator<APlayerStart> Iterator(GetWorld()); Iterator; ++Iterator)
	{
		APlayerStart* PlayerStart = *Iterator;
		if (!IsValid(PlayerStart)) continue;

		PlayerStarts.Emplace(PlayerStart);
	}
}

void AGameManager::GetHunterPropStartsOfType(ERoleType RoleType, TArray<AHunterPropStart*>& HunterPropStarts) const
{
	HunterPropStarts.Empty();
	for (TActorIterator<AHunterPropStart> Iterator(GetWorld()); Iterator; ++Iterator)
	{
		AHunterPropStart* HunterPropStart = *Iterator;
		if (!IsValid(HunterPropStart) || HunterPropStart->GetStartRoleType() != RoleType) continue;

		HunterPropStarts.Emplace(HunterPropStart);
	}
}

FTransform AGameManager::GetSpawnTransformFromPlayerStart(TArray<APlayerStart*>& PlayerStarts) const
{
	FTransform OutTransform = FTransform::Identity;
	
	if (PlayerStarts.IsEmpty())
	{
		GetPlayerStarts(PlayerStarts);
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
		GetHunterPropStartsOfType(RoleType, HunterPropStarts);
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
