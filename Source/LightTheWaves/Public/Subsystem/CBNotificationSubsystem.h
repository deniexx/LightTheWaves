// Made By Cubic Burrito

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CBNotificationSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNotificationSent, const FString&, Notification, float, Duration);

/**
 * 
 */
UCLASS()
class LIGHTTHEWAVES_API UCBNotificationSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintAssignable)
	FOnNotificationSent OnNotificationSent;

	void SendNotification(const FString& Notification, float Duration);
};
