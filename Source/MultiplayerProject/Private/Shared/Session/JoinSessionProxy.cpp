// Copyright Robert Uszynski

/* Class header. */
#include "Shared/Session/JoinSessionProxy.h"

/* Engine includes. */
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSubsystemUtils.h"

UJoinSessionProxy::UJoinSessionProxy(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer),
	Delegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCompleted))
{
}

UJoinSessionProxy* UJoinSessionProxy::JoinSession(APlayerController* PlayerController, const FOnlineSessionSearchResult& OnlineSession, const FOnSessionJoined& OnSessionJoined)
{
	if (!IsValid(PlayerController) || !IsValid(PlayerController->GetWorld()))
	{
		OnSessionJoined.ExecuteIfBound(EOnJoinSessionCompleteResult::Type::UnknownError, NAME_None);
		return nullptr;
	}

	if (UJoinSessionProxy* NewInstance = NewObject<UJoinSessionProxy>())
	{
		NewInstance->WeakPlayerController = PlayerController;
		NewInstance->WeakWorld = PlayerController->GetWorld();
		NewInstance->OnlineSearchResult = OnlineSession;
		NewInstance->Callback = OnSessionJoined;
		return NewInstance;
	}

	OnSessionJoined.ExecuteIfBound(EOnJoinSessionCompleteResult::Type::UnknownError, NAME_None);
	return nullptr;
}

void UJoinSessionProxy::Activate()
{
	if (!WeakPlayerController.IsValid())
	{
		Callback.ExecuteIfBound(EOnJoinSessionCompleteResult::Type::UnknownError, NAME_None);
		return;
	}
	
	const APlayerController* PlayerController = WeakPlayerController.Get();
	const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(PlayerController->GetWorld());
	
	if (!SessionInterface.IsValid())
	{
		Callback.ExecuteIfBound(EOnJoinSessionCompleteResult::Type::UnknownError, NAME_None);
		return;
	}

	DelegateHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(Delegate);

	if (const ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer(); !SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, OnlineSearchResult))
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(DelegateHandle);
		Callback.ExecuteIfBound(EOnJoinSessionCompleteResult::Type::UnknownError, NAME_None);
	}
}

void UJoinSessionProxy::OnCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (!WeakWorld.IsValid())
	{
		Callback.ExecuteIfBound(EOnJoinSessionCompleteResult::Type::UnknownError, NAME_None);
		return;
	}
	
	const UWorld* World = WeakWorld.Get();
	const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(World);
	
	if (!SessionInterface.IsValid())
	{
		Callback.ExecuteIfBound(EOnJoinSessionCompleteResult::Type::UnknownError, NAME_None);
		return;
	}

	SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(DelegateHandle);

	if (Result == EOnJoinSessionCompleteResult::Success)
	{
		if (FString ConnectString; SessionInterface->GetResolvedConnectString(NAME_GameSession, ConnectString) && WeakPlayerController.IsValid())
		{
			WeakPlayerController.Get()->ClientTravel(ConnectString, TRAVEL_Absolute);
		}
	}

	Callback.ExecuteIfBound(Result, SessionName);
}
