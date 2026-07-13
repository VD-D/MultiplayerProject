// Copyright Robert Uszynski

/* Class header. */
#include "Shared/Types/SharedStructs.h"

/* Engine includes. */
#include "OnlineSessionSettings.h"

int32 FSessionResult::GetPing() const
{
	return OnlineResult != nullptr ? OnlineResult->PingInMs : -1;
}