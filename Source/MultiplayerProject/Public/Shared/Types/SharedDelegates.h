// Copyright Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "UObject/Object.h"
#include "Shared/Types/SharedStructs.h"
#include "SharedDelegates.generated.h"

class FOnlineSessionSettings;

DECLARE_DELEGATE_TwoParams(FOnSessionCreated, bool bSuccess, const FOnlineSessionSettings& Settings);
DECLARE_DELEGATE_TwoParams(FOnSessionsFound, bool bSuccess, const TArray<FSessionResult>& FoundSessions);
DECLARE_DELEGATE_TwoParams(FOnSessionJoined, EOnJoinSessionCompleteResult::Type Result, const FName& SessionName);
DECLARE_DELEGATE_TwoParams(FOnSessionDestroyed, bool bSuccess, const FName& SessionName);

/**
 * Contains delegates shared across classes.
 */
UCLASS()
class MULTIPLAYERPROJECT_API USharedDelegates : public UObject
{
	GENERATED_BODY()
};
