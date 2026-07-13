// Copyright Robert Uszynski

/* Class header. */
#include "Shared/Subsystems/SessionSubsystem.h"

/* Project includes. */
#include "Core/Settings/MultiplayerSettings.h"
#include "Shared/Libraries/Logging.h"
#include "Shared/Session/CreateSessionProxy.h"
#include "Shared/Session/DestroySessionProxy.h"
#include "Shared/Session/FindSessionsProxy.h"
#include "Shared/Session/JoinSessionProxy.h"

/* Engine includes. */
#include "GameFramework/PlayerController.h"

USessionSubsystem* USessionSubsystem::Get(const UObject* WorldContextObject)
{
	if (GEngine == nullptr) return nullptr;

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull); IsValid(World))
	{
		return IsValid(World->GetGameInstance()) ? World->GetGameInstance()->GetSubsystem<USessionSubsystem>() : nullptr;
	}

	return nullptr;
}

FOnlineSessionSettings USessionSubsystem::GetSessionSettings(const UObject* WorldContextObject)
{
	if (const USessionSubsystem* CurrentInstance = Get(WorldContextObject))
	{
		return CurrentInstance->LastSessionSettings;
	}

	return FOnlineSessionSettings();
}

void USessionSubsystem::CreateSession(APlayerController* PlayerController, int32 NumPublicConnections, bool bIsLan, bool bUseLobbies, const FOnSessionCreated& OnSessionCreated)
{
	if (USessionSubsystem* CurrentInstance = Get(PlayerController))
	{
		if (IsValid(CurrentInstance->CreateSessionProxyInstance.Get()) || !IsValid(PlayerController))
		{
			OnSessionCreated.ExecuteIfBound(false, FOnlineSessionSettings());
			return;
		}

		TWeakObjectPtr WeakThis = CurrentInstance;
		const FOnSessionCreated InternalDelegate = FOnSessionCreated::CreateLambda([WeakThis, OnSessionCreated](bool bSuccess, const FOnlineSessionSettings& Settings)
		{
			if (WeakThis.IsValid())
			{
				WeakThis.Get()->CreateSessionProxyInstance = nullptr;
				WeakThis.Get()->LastSessionSettings = Settings;
			}
		
			OnSessionCreated.ExecuteIfBound(bSuccess, Settings);
		});

		CurrentInstance->CreateSessionProxyInstance = UCreateSessionProxy::CreateSessionProxy(PlayerController, NumPublicConnections, bIsLan, bUseLobbies, InternalDelegate);
	
		if (!IsValid(CurrentInstance->CreateSessionProxyInstance.Get()))
		{
			OnSessionCreated.ExecuteIfBound(false, FOnlineSessionSettings());
			CurrentInstance->CreateSessionProxyInstance = nullptr;
			return;
		}

		CurrentInstance->CreateSessionProxyInstance.Get()->Activate();
	}
	else
	{
		OnSessionCreated.ExecuteIfBound(false, FOnlineSessionSettings());
	}
}

void USessionSubsystem::FindSessions(APlayerController* PlayerController, int32 MaxResults, bool bUseLan, bool bUseLobbies, const FOnSessionsFound& OnSessionsFound)
{
	if (USessionSubsystem* CurrentInstance = Get(PlayerController))
	{
		if (IsValid(CurrentInstance->FindSessionsProxy.Get()) || !IsValid(PlayerController))
		{
			OnSessionsFound.ExecuteIfBound(false, TArray<FSessionResult>());
			return;
		}

		TWeakObjectPtr WeakThis = CurrentInstance;
		const FOnSessionsFound InternalDelegate = FOnSessionsFound::CreateLambda([WeakThis, OnSessionsFound](bool bSuccess, const TArray<FSessionResult>& Sessions)
		{
			if (WeakThis.IsValid())
			{
				WeakThis.Get()->FindSessionsProxy = nullptr;
			}
		
			OnSessionsFound.ExecuteIfBound(bSuccess, Sessions);
		});

		CurrentInstance->FindSessionsProxy = UFindSessionsProxy::FindSessionsProxy(PlayerController, MaxResults, bUseLan, bUseLobbies, InternalDelegate);
		
		if (!IsValid(CurrentInstance->FindSessionsProxy.Get()))
		{
			OnSessionsFound.ExecuteIfBound(false, TArray<FSessionResult>());
			CurrentInstance->FindSessionsProxy = nullptr;
			return;
		}

		CurrentInstance->FindSessionsProxy.Get()->Activate();
	}
	else
	{
		OnSessionsFound.ExecuteIfBound(false, TArray<FSessionResult>());
	}
}

void USessionSubsystem::JoinSession(APlayerController* PlayerController, const FOnlineSessionSearchResult* SessionSearchResult, const FOnSessionJoined& OnSessionJoined)
{
	if (USessionSubsystem* CurrentInstance = Get(PlayerController))
	{
		if (IsValid(CurrentInstance->JoinSessionProxy.Get()) || !IsValid(PlayerController) || SessionSearchResult == nullptr)
		{
			OnSessionJoined.ExecuteIfBound(EOnJoinSessionCompleteResult::Type::UnknownError, NAME_None);
			return;
		}

		TWeakObjectPtr WeakThis = CurrentInstance;
		const FOnSessionJoined InternalDelegate = FOnSessionJoined::CreateLambda([WeakThis, OnSessionJoined](EOnJoinSessionCompleteResult::Type Result, const FName& SessionName)
		{
			if (WeakThis.IsValid())
			{
				WeakThis.Get()->JoinSessionProxy = nullptr;
			}
		
			OnSessionJoined.ExecuteIfBound(Result, SessionName);
		});

		CurrentInstance->JoinSessionProxy = UJoinSessionProxy::JoinSession(PlayerController, *SessionSearchResult, InternalDelegate);
		
		if (!IsValid(CurrentInstance->JoinSessionProxy.Get()))
		{
			OnSessionJoined.ExecuteIfBound(EOnJoinSessionCompleteResult::Type::UnknownError, NAME_None);
			CurrentInstance->JoinSessionProxy = nullptr;
			return;
		}

		CurrentInstance->JoinSessionProxy.Get()->Activate();
	}
	else
	{
		OnSessionJoined.ExecuteIfBound(EOnJoinSessionCompleteResult::Type::UnknownError, NAME_None);
	}
}

void USessionSubsystem::DestroySession(APlayerController* PlayerController, const FOnSessionDestroyed& OnSessionDestroyed)
{
	if (USessionSubsystem* CurrentInstance = Get(PlayerController))
	{
		if (IsValid(CurrentInstance->DestroySessionProxy.Get()) || !IsValid(PlayerController))
		{
			OnSessionDestroyed.ExecuteIfBound(false, NAME_None);
			return;
		}

		TWeakObjectPtr WeakThis = CurrentInstance;
		const FOnSessionDestroyed InternalDelegate = FOnSessionDestroyed::CreateLambda([WeakThis, OnSessionDestroyed](bool bSuccess, const FName& SessionName)
		{
			if (WeakThis.IsValid())
			{
				WeakThis.Get()->DestroySessionProxy = nullptr;
			}
		
			OnSessionDestroyed.ExecuteIfBound(bSuccess, SessionName);
		});

		CurrentInstance->DestroySessionProxy = UDestroySessionProxy::DestroySessionProxy(PlayerController, InternalDelegate);
		
		if (!IsValid(CurrentInstance->DestroySessionProxy.Get()))
		{
			OnSessionDestroyed.ExecuteIfBound(false, NAME_None);
			CurrentInstance->DestroySessionProxy = nullptr;
			return;
		}

		CurrentInstance->DestroySessionProxy.Get()->Activate();
	}
	else
	{
		OnSessionDestroyed.ExecuteIfBound(false, NAME_None);
	}
}

void USessionSubsystem::TravelToLobby(const UObject* WorldContextObject)
{
	if (GEngine == nullptr) return;

	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull); IsValid(World))
	{
		const FName LevelName = FName(*FPackageName::ObjectPathToPackageName(UMultiplayerSettings::GetLobbyLevel().ToString()));
		const FString AdditionalOptions = UMultiplayerSettings::GetAdditionalLobbyTravelOptions();
		
		FWorldContext &WorldContext = GEngine->GetWorldContextFromWorldChecked(World);
		FString Cmd = LevelName.ToString() + "?listen";
		if (AdditionalOptions.Len() > 0)
		{
			Cmd += FString(TEXT(" ")) + AdditionalOptions;
		}

		if (FURL TestURL(&WorldContext.LastURL, *Cmd, TRAVEL_Absolute); TestURL.IsLocalInternal())
		{
			if (!GEngine->MakeSureMapNameIsValid(TestURL.Map))
			{
				ULogging::LogVerboseError("USessionSubsystem", "USessionSubsystem::TravelToLobby", FString::Printf(TEXT("Lobby level (called %s) does not exist"), *TestURL.Map));
			}
		}

		GEngine->SetClientTravel(World, *Cmd, TRAVEL_Absolute);
	}
}
