// Copyright Robert Uszynski

/* Class header. */
#include "Shared/Libraries/Logging.h"

/* Project includes. */
#include "Core/Settings/MultiplayerSettings.h"

DEFINE_LOG_CATEGORY(LogGameDebug);

void ULogging::LogMessage(const FString& Message, EVerbosity Verbosity, ELoggingType LoggingType, int32 OnScreenKey, float OnScreenTime)
{
    if (LoggingType != ELoggingType::Console && GEngine != nullptr)
    {
        const FColor MessageColor = Verbosity == EVerbosity::Log ? UMultiplayerSettings::GetOnScreenMessageLogColor() : Verbosity == EVerbosity::Warning ? UMultiplayerSettings::GetOnScreenWarningLogColor() : UMultiplayerSettings::GetOnScreenErrorLogColor();
        GEngine->AddOnScreenDebugMessage(OnScreenKey, OnScreenTime, MessageColor, Message, UMultiplayerSettings::GetNewScreenMessagesOnTop(), UMultiplayerSettings::GetOnScreenMessageTextScale());
    }

    if (LoggingType != ELoggingType::Screen)
    {
        switch (Verbosity)
        {
            case EVerbosity::Log:
                UE_LOG(LogGameDebug, Log, TEXT("%s"), *Message);
                break;
            
            case EVerbosity::Warning:
                UE_LOG(LogGameDebug, Warning, TEXT("%s"), *Message);
                break;

            case EVerbosity::Error:
                UE_LOG(LogGameDebug, Error, TEXT("%s"), *Message);
                break;
        }
    }
}

void ULogging::LogMessageToConsole(const FString& Message)
{
    LogMessage(Message, EVerbosity::Log, ELoggingType::Console);
}

void ULogging::LogWarningToConsole(const FString& Message)
{
    LogMessage(Message, EVerbosity::Warning, ELoggingType::Console);
}

void ULogging::LogErrorToConsole(const FString& Message)
{
    LogMessage(Message, EVerbosity::Error, ELoggingType::Console);
}

void ULogging::LogVerboseError(const FString& ObjectName, const FString& FunctionName, const FString& Message)
{
    LogErrorToConsole(FString::Printf(TEXT("%s [%s] : %s"), *ObjectName, *FunctionName, *Message));
}

void ULogging::LogVerboseWarning(const FString& ObjectName, const FString& FunctionName, const FString& Message)
{
    LogWarningToConsole(FString::Printf(TEXT("%s [%s] : %s"), *ObjectName, *FunctionName, *Message));
}

FString ULogging::BoolToString(bool bValue)
{
    return bValue ? TEXT("True") : TEXT("False");
}

FString ULogging::ObjectValidityToString(const UObject* Object)
{
    return IsValid(Object) ? TEXT("True") : TEXT("False");
}

FString ULogging::StringArrayToString(const TArray<FString>& Array)
{
    return FString::Join(Array, TEXT(","));
}

FString ULogging::NameArrayToString(const TArray<FName>& Array)
{
    TArray<FString> StringArray;
    for (const auto& Name : Array) StringArray.Emplace(Name.ToString());
    return FString::Join(StringArray, TEXT(","));
}