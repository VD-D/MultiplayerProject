// Copyright Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineSessionDelegates.h"
#include "Shared/Types/SharedDelegates.h"
#include "UObject/Object.h"
#include "DestroySessionProxy.generated.h"

/**
 * Based on DestroySessionCallbackProxy from OnlineSubsystemUtils, but designed to be used in c++
 */
UCLASS()
class MULTIPLAYERPROJECT_API UDestroySessionProxy : public UObject
{
	GENERATED_BODY()
#pragma region Internal
	/* References to controller. */
	TWeakObjectPtr<APlayerController> WeakPlayerController;

	/* Delegates and handles for executing logic. */
	FOnDestroySessionCompleteDelegate Delegate;
	FDelegateHandle DelegateHandle;
	FOnSessionDestroyed Callback;
#pragma endregion Internal

#pragma region Construction
public:
	/**
	 * Default constructor.
	 */
	explicit UDestroySessionProxy(const FObjectInitializer& ObjectInitializer);
	
	/**
	 * Attempts to destroy the existing session. For clients, this disconnects them. For servers, it ends the game.
	 * @param PlayerController Player controller searching for sessions.
	 * @param OnSessionDestroyed Callback after session is destroyed.
	 * @return Newly created DestroySessionProxy instance.
	 */
	static UDestroySessionProxy* DestroySessionProxy(APlayerController* PlayerController, const FOnSessionDestroyed& OnSessionDestroyed = FOnSessionDestroyed());

	/**
	 * Actually performs the necessary logic to end the session.
	 */
	void Activate();

protected:
	/**
	 * Callback when the session destruction completes, calls out to the public success/failure callbacks
	 * @param SessionName Broadcast by delegates.
	 * @param bSuccess For broadcasting success or failure
	 */
	void OnCompleted(FName SessionName, bool bSuccess);
#pragma endregion Construction
};
