// Copyright Robert Uszynski

/* Class header. */
#include "Shared/Session/CreateSessionProxy.h"

/* Engine includes. */
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"

UCreateSessionProxy::UCreateSessionProxy(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer),
	CreateCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateCompleted)),
	StartCompleteDelegate(FOnStartSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnStartCompleted))
{
}

UCreateSessionProxy* UCreateSessionProxy::CreateSessionProxy(APlayerController* PlayerController, int32 PublicConnections, bool bUseLan, bool bUseLobbiesIfAvailable, const FOnSessionCreated& OnSessionCreatedCallback)
{
	FOnlineSessionSettings NewSessionSettings;
	NewSessionSettings.NumPublicConnections = PublicConnections;
	NewSessionSettings.bIsLANMatch = bUseLan;
	NewSessionSettings.bUseLobbiesIfAvailable = bUseLobbiesIfAvailable;
	NewSessionSettings.bShouldAdvertise = true;
	NewSessionSettings.bAllowJoinInProgress = true;
	NewSessionSettings.bUsesPresence = true;
	NewSessionSettings.bAllowJoinViaPresence = true;

	return CreateSessionProxy(PlayerController, NewSessionSettings, OnSessionCreatedCallback);
}

UCreateSessionProxy* UCreateSessionProxy::CreateSessionProxy(APlayerController* PlayerController, const FOnlineSessionSettings& NewSessionSettings, const FOnSessionCreated& OnSessionCreatedCallback)
{
	if (!IsValid(PlayerController) || !IsValid(PlayerController->GetWorld()))
	{
		OnSessionCreatedCallback.ExecuteIfBound(false, NewSessionSettings);
		return nullptr;
	}

	if (UCreateSessionProxy* NewInstance = NewObject<UCreateSessionProxy>())
	{
		NewInstance->WeakPlayerController = PlayerController;
		NewInstance->WeakWorld = PlayerController->GetWorld();
		NewInstance->SessionSettings = NewSessionSettings;
		NewInstance->Callback = OnSessionCreatedCallback;
		return NewInstance;
	}

	OnSessionCreatedCallback.ExecuteIfBound(false, NewSessionSettings);
	return nullptr;
}

void UCreateSessionProxy::Activate()
{
	if (!WeakPlayerController.IsValid())
	{
		Callback.ExecuteIfBound(false, SessionSettings);
		return;
	}
	
	const APlayerController* PlayerController = WeakPlayerController.Get();
	const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(PlayerController->GetWorld());
	
	if (!SessionInterface.IsValid())
	{
		Callback.ExecuteIfBound(false, SessionSettings);
		return;
	}

	CreateCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateCompleteDelegate);

	if (const ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer(); !SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SessionSettings))
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateCompleteDelegateHandle);
		Callback.ExecuteIfBound(false, SessionSettings);
	}
}

void UCreateSessionProxy::OnCreateCompleted(FName SessionName, bool bWasSuccessful)
{
	if (!WeakWorld.IsValid())
	{
		Callback.ExecuteIfBound(false, SessionSettings);
		return;
	}

	const UWorld* World = WeakWorld.Get();
	if (const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(World->GetWorld()); SessionInterface.IsValid())
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateCompleteDelegateHandle);
		if (bWasSuccessful)
		{
			StartCompleteDelegateHandle = SessionInterface->AddOnStartSessionCompleteDelegate_Handle(StartCompleteDelegate);
			SessionInterface->StartSession(NAME_GameSession);
			return;
		}
	}

	if (!bWasSuccessful)
	{
		Callback.ExecuteIfBound(false, SessionSettings);
	}
}

void UCreateSessionProxy::OnStartCompleted(FName SessionName, bool bWasSuccessful)
{
	if (!WeakWorld.IsValid())
	{
		Callback.ExecuteIfBound(false, SessionSettings);
		return;
	}

	const UWorld* World = WeakWorld.Get();
	if (const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(World->GetWorld()); SessionInterface.IsValid())
	{
		SessionInterface->ClearOnStartSessionCompleteDelegate_Handle(CreateCompleteDelegateHandle);
	}

	Callback.ExecuteIfBound(bWasSuccessful, SessionSettings);
}
