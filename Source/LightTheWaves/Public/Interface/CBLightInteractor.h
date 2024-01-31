// Made By Cubic Burrito

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CBLightInteractor.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCBLightInteractor : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LIGHTTHEWAVES_API ICBLightInteractor
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, Category = "LightInteractor")
	void OnLightFocused(UPrimitiveComponent* TargetComponent);

	UFUNCTION(BlueprintNativeEvent, Category = "LightInteractor")
	void OnLightFocusEnd();
};
