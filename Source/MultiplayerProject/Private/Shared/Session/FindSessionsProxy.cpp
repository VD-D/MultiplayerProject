// Copyright Robert Uszynski

/* Class header. */
#include "Shared/Session/FindSessionsProxy.h"

/* Engine includes. */
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Online/OnlineSessionNames.h"

UFindSessionsProxy::UFindSessionsProxy(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer),
	Delegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnCompleted))
{
}

UFindSessionsProxy* UFindSessionsProxy::FindSessionsProxy(APlayerController* PlayerController, int32 MaxResults, bool bUseLan, bool bUseLobbies, const FOnSessionsFound& OnSessionsFound)
{
	if (!IsValid(PlayerController) || !IsValid(PlayerController->GetWorld()))
	{
		OnSessionsFound.ExecuteIfBound(false, TArray<FSessionResult>());
		return nullptr;
	}

	if (UFindSessionsProxy* NewInstance = NewObject<UFindSessionsProxy>())
	{
		NewInstance->WeakPlayerController = PlayerController;
		NewInstance->WeakWorld = PlayerController->GetWorld();
		NewInstance->MaxResults = MaxResults;
		NewInstance->bUseLan = bUseLan;
		NewInstance->bUseLobbies = bUseLobbies;
		NewInstance->Callback = OnSessionsFound;
		return NewInstance;
	}

	OnSessionsFound.ExecuteIfBound(false, TArray<FSessionResult>());
	return nullptr;
}

void UFindSessionsProxy::Activate()
{
	if (!WeakPlayerController.IsValid())
	{
		Callback.ExecuteIfBound(false, TArray<FSessionResult>());
		return;
	}
	
	const APlayerController* PlayerController = WeakPlayerController.Get();
	const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(PlayerController->GetWorld());
	
	if (!SessionInterface.IsValid())
	{
		Callback.ExecuteIfBound(false, TArray<FSessionResult>());
		return;
	}

	DelegateHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(Delegate);
			
	SearchObject = MakeShareable(new FOnlineSessionSearch);
	SearchObject->MaxSearchResults = MaxResults;
	SearchObject->bIsLanQuery = bUseLan;
	
	if (bUseLobbies)
	{
		SearchObject->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);
	}

	if (const ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer(); !SessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), SearchObject.ToSharedRef()))
	{
		SessionInterface->ClearOnCancelFindSessionsCompleteDelegate_Handle(DelegateHandle);
		Callback.ExecuteIfBound(false, TArray<FSessionResult>());
	}
}

void UFindSessionsProxy::OnCompleted(bool bSuccess)
{
	if (!WeakWorld.IsValid())
	{
		Callback.ExecuteIfBound(false, TArray<FSessionResult>());
		return;
	}
	
	const UWorld* World = WeakWorld.Get();
	const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(World);
	
	if (!SessionInterface.IsValid())
	{
		Callback.ExecuteIfBound(false, TArray<FSessionResult>());
		return;
	}

	SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(DelegateHandle);

	TArray<FSessionResult> Results;
	if (bSuccess && SearchObject.IsValid())
	{
		for (auto& Result : SearchObject->SearchResults)
		{
			Results.Emplace(FSessionResult(&Result));
		}
	}
	
	Callback.ExecuteIfBound(bSuccess, Results);
}
