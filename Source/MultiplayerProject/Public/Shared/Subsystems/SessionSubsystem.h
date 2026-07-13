// Copyright Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "OnlineSessionSettings.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Shared/Types/SharedDelegates.h"
#include "SessionSubsystem.generated.h"

class APlayerController;
class UCreateSessionProxy;
class UDestroySessionProxy;
class UFindSessionsProxy;
class UJoinSessionProxy;

/**
 * Subsystem for handling multiplayer session creation, destruction and joining.
 * This acts as a wrapper for the session callback proxies, to make them easier to use in native c++. 
 */
UCLASS()
class MULTIPLAYERPROJECT_API USessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
#pragma region Proxies
	UPROPERTY()
	TObjectPtr<UCreateSessionProxy> CreateSessionProxyInstance;

	UPROPERTY()
	TObjectPtr<UFindSessionsProxy> FindSessionsProxy;

	UPROPERTY()
	TObjectPtr<UJoinSessionProxy> JoinSessionProxy;

	UPROPERTY()
	TObjectPtr<UDestroySessionProxy> DestroySessionProxy;
	
	FOnlineSessionSettings LastSessionSettings;
#pragma endregion Proxies
	
#pragma region Session
public:
	/**
	 * @return The current instance of this subsystem.
	 */
	static USessionSubsystem* Get(const UObject* WorldContextObject);

	/**
	 * Gets most recent session settings. This is only updated on server.
	 * @param WorldContextObject Gets world.
	 * @return The session settings for the last created session
	 */
	static FOnlineSessionSettings GetSessionSettings(const UObject* WorldContextObject);
	
	/**
	 * Creates a new session (also immediately starts the session).
	 * @param PlayerController Player creating the session.
	 * @param NumPublicConnections Effectively how many players the session can have.
	 * @param bIsLan Whether using LAN (true) or online (false).
	 * @param bUseLobbies Whether lobbies should be enabled.
	 * @param OnSessionCreated Callback after the session is created.
	 */
	static void CreateSession(APlayerController* PlayerController, int32 NumPublicConnections, bool bIsLan, bool bUseLobbies = true, const FOnSessionCreated& OnSessionCreated = FOnSessionCreated());

	/**
	 * Attempts to find existing sessions.
	 * @param PlayerController Player controller searching for sessions.
	 * @param MaxResults How many sessions to search for.
	 * @param bUseLan Whether using LAN (true) or online (false)
	 * @param bUseLobbies Whether lobbies should be enabled.
	 * @param OnSessionsFound Callback after sessions are found.
	 */
	static void FindSessions(APlayerController* PlayerController, int32 MaxResults, bool bUseLan = true, bool bUseLobbies = true, const FOnSessionsFound& OnSessionsFound = FOnSessionsFound());

	/**
	 * Attempts to join an existing session.
	 * @param PlayerController Player controller attempting to join session.
	 * @param SessionSearchResult The session we want to join.
	 * @param OnSessionJoined Callback after the session is joined.
	 */
	static void JoinSession(APlayerController* PlayerController, const FOnlineSessionSearchResult* SessionSearchResult, const FOnSessionJoined& OnSessionJoined = FOnSessionJoined());

	/**
	 * Attempts to leave a session.
	 * @param PlayerController Player controller wanting to leave session.
	 * @param OnSessionDestroyed Callback after session is destroyed.
	 */
	static void DestroySession(APlayerController* PlayerController, const FOnSessionDestroyed& OnSessionDestroyed = FOnSessionDestroyed());

	/**
	 * Makes local player travel to the lobby.
	 * @param WorldContextObject Gets world.
	 */
	static void TravelToLobby(const UObject* WorldContextObject);
#pragma endregion Session
};
