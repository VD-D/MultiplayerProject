// Copyright Robert Uszynski

/* Class header. */
#include "Shared/Session/DestroySessionProxy.h"

/* Engine includes. */
#include "OnlineSubsystemUtils.h"

UDestroySessionProxy::UDestroySessionProxy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Delegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCompleted))
{
}

UDestroySessionProxy* UDestroySessionProxy::DestroySessionProxy(APlayerController* PlayerController, const FOnSessionDestroyed& OnSessionDestroyed)
{
	if (!IsValid(PlayerController) || !IsValid(PlayerController->GetWorld()))
	{
		OnSessionDestroyed.ExecuteIfBound(false, NAME_None);
		return nullptr;
	}

	if (UDestroySessionProxy* NewInstance = NewObject<UDestroySessionProxy>())
	{
		NewInstance->WeakPlayerController = PlayerController;
		NewInstance->Callback = OnSessionDestroyed;
		return NewInstance;
	}

	return nullptr;
}

void UDestroySessionProxy::Activate()
{
	if (!WeakPlayerController.IsValid())
	{
		Callback.ExecuteIfBound(false, NAME_None);
		return;
	}
	
	const APlayerController* PlayerController = WeakPlayerController.Get();
	const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(PlayerController->GetWorld());
	
	if (!SessionInterface.IsValid())
	{
		Callback.ExecuteIfBound(false, NAME_None);
		return;
	}

	DelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(Delegate);
	if (!SessionInterface->DestroySession(NAME_GameSession))
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DelegateHandle);
		Callback.ExecuteIfBound(false, NAME_None);
	}
}

void UDestroySessionProxy::OnCompleted(FName SessionName, bool bSuccess)
{
	if (!WeakPlayerController.IsValid())
	{
		Callback.ExecuteIfBound(false, NAME_None);
		return;
	}
	
	const UWorld* World = WeakPlayerController.Get()->GetWorld();
	const IOnlineSessionPtr SessionInterface = Online::GetSessionInterface(World);
	
	if (!SessionInterface.IsValid())
	{
		Callback.ExecuteIfBound(false, NAME_None);
		return;
	}

	SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DelegateHandle);
	Callback.ExecuteIfBound(bSuccess, SessionName);
}
