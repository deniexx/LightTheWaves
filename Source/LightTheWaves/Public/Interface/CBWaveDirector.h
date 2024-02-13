// Made By Cubic Burrito

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CBWaveDirector.generated.h"

UENUM(BlueprintType)
enum class EActivityFinishedState : uint8
{
	Successful,
	Failed,
	Ongoing,

	MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EGameActivity : uint8
{
	Wave,
	Recess,
	Boss,
	Unknown,

	// No previous Activity
	None,

	MAX UMETA(Hidden)
	
};

USTRUCT(BlueprintType)
struct FActivityStateUpdatedData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	EActivityFinishedState ActivityFinishedState;

	UPROPERTY(BlueprintReadOnly)
	EGameActivity OldActivity;

	UPROPERTY(BlueprintReadOnly)
	EGameActivity NewActivity;
	
	/** @NOTE: Add any additional data that needs to be sent here */
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActivityStateUpdated, FActivityStateUpdatedData, ActivityStateUpdatedData);

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCBWaveDirector : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LIGHTTHEWAVES_API ICBWaveDirector
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Wave Director")
	void StartWaveGameplay();
	
	virtual FOnActivityStateUpdated& OnActivityStateUpdatedEvent() = 0;
};
