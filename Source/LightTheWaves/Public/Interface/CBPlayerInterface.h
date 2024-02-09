// Made By Cubic Burrito

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CBPlayerInterface.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAttributeChanged, float, OldAmount, float, NewAmount);

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCBPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LIGHTTHEWAVES_API ICBPlayerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Attributes")
	void ApplyChangeToCurrency(int32 Delta);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Player Attributes")
	void ApplyChangeToPoints(int32 Delta);

	virtual FOnAttributeChanged& OnPointsChangedEvent() = 0;
	virtual FOnAttributeChanged& OnCurrencyChangedEvent() = 0;
};
