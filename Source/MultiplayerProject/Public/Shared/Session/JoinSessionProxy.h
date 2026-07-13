// Copyright Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "UObject/Object.h"
#include "Shared/Types/SharedDelegates.h"
#include "JoinSessionProxy.generated.h"

/**
 * Based on JoinSessionCallbackProxy from OnlineSubsystemUtils, but designed to be used in c++
 */
UCLASS()
class MULTIPLAYERPROJECT_API UJoinSessionProxy : public UObject
{
	GENERATED_BODY()
#pragma region Internal
	/* References to controller and world. */
	TWeakObjectPtr<APlayerController> WeakPlayerController;
	TWeakObjectPtr<UWorld> WeakWorld;

	/* Session we are trying to join. */
	FOnlineSessionSearchResult OnlineSearchResult;

	/* Delegates and handles required for handling logic.*/
	FOnJoinSessionCompleteDelegate Delegate;
	FDelegateHandle DelegateHandle;
	FOnSessionJoined Callback;
#pragma endregion Internal

#pragma region Construction
public:
	/**
	 * Default constructor.
	 */
	explicit UJoinSessionProxy(const FObjectInitializer& ObjectInitializer);
	
	/**
	 * Attempts to join an existing session.
	 * @param PlayerController Player controller attempting to join session.
	 * @param OnlineSession The session we want to join.
	 * @param OnSessionJoined Callback after the session is joined.
	 * @return The newly constructed join session object.
	 */
	static UJoinSessionProxy* JoinSession(APlayerController* PlayerController, const FOnlineSessionSearchResult& OnlineSession, const FOnSessionJoined& OnSessionJoined);

	/**
	 * Actually performs the necessary logic to join the session.
	 */
	void Activate();

protected:
	/**
	 * Callback after session is joined.
	 * @param SessionName Name of the session we have joined.
	 * @param Result Whether the session was joined successfully.
	 */
	void OnCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
#pragma endregion Construction
	
};
