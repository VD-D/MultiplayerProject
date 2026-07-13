// Copyright Robert Uszynski

/* Class header. */
#include "Shared/Libraries/MultiplayerLibrary.h"

APlayerController* UMultiplayerLibrary::GetLocalPlayerController(const UObject* WorldContextObject)
{
	if (GEngine == nullptr) return nullptr;

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		return GEngine->GetFirstLocalPlayerController(World);
	}

	return nullptr;
}

int32 UMultiplayerLibrary::GetNumPlayers(const UObject* WorldContextObject)
{
	if (GEngine == nullptr) return 0;

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		TArray<APlayerController*> Controllers;
		for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			APlayerController* PlayerController = Iterator->Get();
			if (!IsValid(PlayerController)) continue;
			Controllers.Emplace(PlayerController);
		}

		return Controllers.Num();	
	}

	return 0;
}
