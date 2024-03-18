// Made By Cubic Burrito


#include "Subsystem/CBNotificationSubsystem.h"

void UCBNotificationSubsystem::SendNotification(const FString& Notification, float Duration)
{
	OnNotificationSent.Broadcast(Notification, Duration);
}
