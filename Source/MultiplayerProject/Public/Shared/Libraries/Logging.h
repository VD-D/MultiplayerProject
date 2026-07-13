// Copyright Robert Uszynski

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Logging.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogGameDebug, Log, All);

/* Defines verbosity of log categories. */
UENUM(BlueprintType)
enum class EVerbosity : uint8
{
	Log     UMETA(DisplayName = "Log"),
	Warning UMETA(DisplayName = "Warning"),
	Error   UMETA(DisplayName = "Error")
};

/* Defines where log should appear. */
UENUM(BlueprintType)
enum class ELoggingType : uint8
{
	Console UMETA(DisplayName = "Console"),
	Screen  UMETA(DisplayName = "Screen"),
	Both    UMETA(DisplayName = "Both")
};

/**
 * Library for logging.
 */
UCLASS()
class MULTIPLAYERPROJECT_API ULogging : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	#pragma region Logging
public:
	/**
	 * @param Message Message to send to log.
	 * @param Verbosity Verbosity of the message.
	 * @param LoggingType Whether message should appear in console, on screen or both.
	 * @param OnScreenKey Only relevant if message appears on screen. Removes other messages of the same key.
	 * @param OnScreenTime Only relevant if message appears on screen. How long message is on screen for.
	 */
	UFUNCTION(BlueprintCallable, Category = "Logging")
	static void LogMessage(const FString& Message, EVerbosity Verbosity = EVerbosity::Log, ELoggingType LoggingType = ELoggingType::Both, int32 OnScreenKey = -1, float OnScreenTime = 5.0f);

	/**
	 * Wrapper for LogMessage with verbosity = Log and LogType = Console.
	 * @param Message Message to send to log.
	 */
	UFUNCTION(BlueprintCallable, Category = "Logging")
	static void LogMessageToConsole(const FString& Message);

	/**
	 * Wrapper for LogMessage with verbosity = Warning and LogType = Console.
	 * @param Message Message to send to log.
	 */
	UFUNCTION(BlueprintCallable, Category = "Logging")
	static void LogWarningToConsole(const FString& Message);

	/**
	 * Wrapper for LogMessage with verbosity = Error and LogType = Console.
	 * @param Message Message to send to log.
	 */
	UFUNCTION(BlueprintCallable, Category = "Logging")
	static void LogErrorToConsole(const FString& Message);

	/**
	 * Logs error such that "ObjectName [FunctionName] Message"
	 * @param ObjectName Identifies object in log.
	 * @param FunctionName Identifies function calling error.
	 * @param Message Says want went wrong.
	 */
	UFUNCTION(BlueprintCallable, Category = "Logging")
	static void LogVerboseError(const FString& ObjectName, const FString& FunctionName, const FString& Message);

	/**
	 * Logs warning such that "ObjectName [FunctionName] Message"
	 * @param ObjectName Identifies object in log.
	 * @param FunctionName Identifies function calling warning.
	 * @param Message Says want went wrong.
	 */
	UFUNCTION(BlueprintCallable, Category = "Logging")
	static void LogVerboseWarning(const FString& ObjectName, const FString& FunctionName, const FString& Message);

	/**
	 * @param bValue Value to covert to string 
	 * @return "True" if bValue, "False" if !bValue
	 */
	UFUNCTION(BlueprintPure, Category = "Logging")
	static FString BoolToString(bool bValue);

	/**
	 * @param Object Checked for validity, which is converted to string. 
	 * @return "True" if object is valid, "False" otherwise.
	 */
	UFUNCTION(BlueprintPure, Category = "Logging")
	static FString ObjectValidityToString(const UObject* Object);

	/**
	 * Converts an array into a single string delimited by commas.
	 * @param Array Array to flatten into a string.
	 * @return Flattened string.
	 */
	UFUNCTION(BlueprintPure, Category = "Logging")
	static FString StringArrayToString(const TArray<FString>& Array);

	/**
	 * Converts an array into a single string delimited by commas.
	 * @param Array Array to flatten into a string.
	 * @return Flattened string.
	 */
	UFUNCTION(BlueprintPure, Category = "Logging")
	static FString NameArrayToString(const TArray<FName>& Array);
#pragma endregion Logging
};
