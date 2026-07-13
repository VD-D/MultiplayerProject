// Copyright Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineSessionDelegates.h"
#include "OnlineSessionSettings.h"
#include "UObject/Object.h"
#include "Shared/Types/SharedDelegates.h"
#include "CreateSessionProxy.generated.h"

/**
 * Based on CreateSessionProxyCallback from OnlineSubsystemUtils, but designed to be used in c++ and exposes more
 * information.
 */
UCLASS()
class MULTIPLAYERPROJECT_API UCreateSessionProxy : public UObject
{
	GENERATED_BODY()
#pragma region Internal
	/* Generated upon creation of session proxy object. */
	FOnlineSessionSettings SessionSettings;

	/* Used as context object and to get session ID from local player. */
	TWeakObjectPtr<APlayerController> WeakPlayerController;
	TWeakObjectPtr<UWorld> WeakWorld;

	/* Callback after session is created. */
	FOnSessionCreated Callback;

	/* Delegates and handles for execution of online subsystem logic. */
	FOnCreateSessionCompleteDelegate CreateCompleteDelegate;
	FOnStartSessionCompleteDelegate StartCompleteDelegate;
	
	FDelegateHandle CreateCompleteDelegateHandle;
	FDelegateHandle StartCompleteDelegateHandle;
#pragma endregion Internal
	
#pragma region Construction
public:
	/**
	 * Default constructor.
	 */
	explicit UCreateSessionProxy(const FObjectInitializer& ObjectInitializer);
	
	/**
	 * Creates a new session.
	 * @param PlayerController Player creating the session.
	 * @param PublicConnections Effectively how many players the session can have.
	 * @param bUseLan Whether using LAN (true) or online (false).
	 * @param bUseLobbiesIfAvailable Whether lobbies should be enabled.
	 * @param OnSessionCreatedCallback Callback after the session is created.
	 * @return The newly created session proxy object.
	 */
	static UCreateSessionProxy* CreateSessionProxy(APlayerController* PlayerController, int32 PublicConnections, bool bUseLan, bool bUseLobbiesIfAvailable = true, const FOnSessionCreated& OnSessionCreatedCallback = FOnSessionCreated());

	/**
	 * Creates a new session. Allows specifying the entire session settings.
	 * @param PlayerController Player creating the session.
	 * @param NewSessionSettings All session settings.
	 * @param OnSessionCreatedCallback Callback after the session is created.
	 * @return The newly created session proxy object.
	 */
	static UCreateSessionProxy* CreateSessionProxy(APlayerController* PlayerController, const FOnlineSessionSettings& NewSessionSettings, const FOnSessionCreated& OnSessionCreatedCallback = FOnSessionCreated());

	/**
	 * Actually performs the necessary logic to create the session.
	 */
	void Activate();

protected:
	/**
	 * Callback for session creation completion.
	 * @param SessionName Unused.
	 * @param bWasSuccessful Broadcasts success or failure.
	 */
	void OnCreateCompleted(FName SessionName, bool bWasSuccessful);

	/**
	 * Callback for session start completion.
	 * @param SessionName Unused.
	 * @param bWasSuccessful Broadcasts success or failure.
	 */
	void OnStartCompleted(FName SessionName, bool bWasSuccessful);
#pragma endregion Construction
};
