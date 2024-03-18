// Made By Cubic Burrito

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CBBlueprintFunctionLibrary.generated.h"

class UCBNotificationSubsystem;
class FOnMonsterSpawned;
class UCBShopSubsystem;
class UCBInitialiser;
/**
 * 
 */
UCLASS()
class LIGHTTHEWAVES_API UCBBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Gets the Initialisation Subsystem
	 * @param WorldContextObject The world context object, used to get the world
	 * @return The Initialisation Subsystem, can be nullptr
	 */
	UFUNCTION(BlueprintCallable, Category = "CBFunctionLibrary", meta = (WorldContext = "WorldContextObject", HidePin = "WorldContextObject"))
	static UCBInitialiser* GetInitialisationSubsystem(UObject* WorldContextObject);
	
	/**
	 * Registers an object to the initialisation subsystem, this can initialise the object straight away if then needed data is available
	 * @param WorldContextObject The world context object, used to get the world
	 * @param ObjectToRegister The object to be registered
	 * @return TRUE - if registration was Successful, FALSE - if registration failed
	 */
	UFUNCTION(BlueprintCallable, Category = "CBFunctionLibrary", meta = (WorldContext = "WorldContextObject", HidePin = "WorldContextObject"))
	static bool RegisterObjectForInitialisation(UObject* WorldContextObject, UObject* ObjectToRegister);

	/**
	 * Gets the Shop Subsystem
	 * @param WorldContextObject The world context object, used to get the world
	 * @return The Initialisation Subsystem, can be nullptr
	 */
	UFUNCTION(BlueprintCallable, Category = "CBFunctionLibrary", meta = (WorldContext = "WorldContextObject", HidePin = "WorldContextObject"))
	static UCBShopSubsystem* GetShopSubsystem(UObject* WorldContextObject);

	/**
	 * Checks if the tutorial should be played
	 * @param WorldContextObject The world context object, used to get the world
	 * @return Whether the tutorial should be played or not
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CBFunctionLibrary", meta = (WorldContext = "WorldContextObject", HidePin = "WorldContextObject"))
	static bool ShouldPlayTutorial(UObject* WorldContextObject);

	/**
	 * Sets the tutorial to be enabled/disabled
	 * @param WorldContextObject The world context object, used to get the world
	 * @param bEnabled Whether the tutorial is enabled or disabled
	 */
	UFUNCTION(BlueprintCallable, Category = "CBFunctionLibrary", meta = (WorldContext = "WorldContextObject", HidePin = "WorldContextObject"))
	static void SetTutorialEnabled(UObject* WorldContextObject, bool bEnabled);

	/**
 	 * Gets the Notification Subsystem
 	 * @param WorldContextObject The world context object, used to get the world
 	 * @return The Notification Subsystem, can be nullptr
 	 */
	UFUNCTION(BlueprintCallable, Category = "CBFunctionLibrary", meta = (WorldContext = "WorldContextObject", HidePin = "WorldContextObject"))
	static UCBNotificationSubsystem* GetNotificationSubsystem(UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable, Category = "CBFunctionLibrary", meta = (WorldContext = "WorldContextObject", HidePin = "WorldContextObject"))
	static void SendNotification(UObject* WorldContextObject, const FString& Notification, float Duration);
};
