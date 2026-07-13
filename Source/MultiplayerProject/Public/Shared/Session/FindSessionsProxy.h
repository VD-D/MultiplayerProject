// Copyright Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineSessionDelegates.h"
#include "OnlineSessionSettings.h"
#include "UObject/Object.h"
#include "Shared/Types/SharedDelegates.h"
#include "FindSessionsProxy.generated.h"

/**
 * Based on FindSessionsCallbackProxy from OnlineSubsystemUtils, but designed to be used in c++
 */
UCLASS()
class MULTIPLAYERPROJECT_API UFindSessionsProxy : public UObject
{
	GENERATED_BODY()
#pragma region Internal
	/* References to controller and world. */
	TWeakObjectPtr<APlayerController> WeakPlayerController;
	TWeakObjectPtr<UWorld> WeakWorld;

	/* Object tracking search results. */
	TSharedPtr<FOnlineSessionSearch> SearchObject;
	
	/* Delegates and handles for executing logic */
	FOnFindSessionsCompleteDelegate Delegate;
	FDelegateHandle DelegateHandle;
	FOnSessionsFound Callback;
	
	/* Internal variables for logic. */
	bool bUseLan;
	bool bUseLobbies;
	int32 MaxResults;
#pragma endregion Internal

#pragma region Construction
public:
	/**
	 * Default constructor.
	 */
	explicit UFindSessionsProxy(const FObjectInitializer& ObjectInitializer);
	
	/**
	 * Attempts to find existing sessions.
	 * @param PlayerController Player controller searching for sessions.
	 * @param MaxResults How many sessions to search for.
	 * @param bUseLan Whether using LAN (true) or online (false)
	 * @param bUseLobbies Whether lobbies should be enabled.
	 * @param OnSessionsFound Callback after sessions are found.
	 * @return Newly created FindSessionsProxy instance.
	 */
	static UFindSessionsProxy* FindSessionsProxy(APlayerController* PlayerController, int32 MaxResults, bool bUseLan, bool bUseLobbies, const FOnSessionsFound& OnSessionsFound);

	/**
	 * Actually performs the necessary logic to find the sessions.
	 */
	void Activate();

protected:
	/**
	 * Callback when the session search completes, calls out to the public success/failure callbacks
	 * @param bSuccess For broadcasting success or failure
	 */
	void OnCompleted(bool bSuccess);
#pragma endregion Construction
};
