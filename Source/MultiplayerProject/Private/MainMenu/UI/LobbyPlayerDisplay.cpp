// Copyright Robert Uszynski

/* Class header. */
#include "MainMenu/UI/LobbyPlayerDisplay.h"

/* Engine includes. */
#include "Components/TextBlock.h"

ULobbyPlayerDisplay* ULobbyPlayerDisplay::CreateLobbyPlayerDisplay(UUserWidget* OwnerWidget, TSubclassOf<ULobbyPlayerDisplay> LobbyPlayerDisplayClass, const FText& PlayerName)
{
	if (!IsValid(OwnerWidget) || LobbyPlayerDisplayClass.Get() == nullptr) return nullptr;

	if (ULobbyPlayerDisplay* NewInstance = CreateWidget<ULobbyPlayerDisplay>(OwnerWidget, LobbyPlayerDisplayClass); IsValid(NewInstance))
	{
		if (IsValid(NewInstance->PlayerNameText))
		{
			NewInstance->PlayerNameText->SetText(PlayerName);
		}
		
		return NewInstance;
	}

	return nullptr;
}
