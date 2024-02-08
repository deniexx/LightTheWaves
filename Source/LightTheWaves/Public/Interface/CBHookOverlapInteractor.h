// Made By Cubic Burrito

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CBHookOverlapInteractor.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCBHookOverlapInteractor : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LIGHTTHEWAVES_API ICBHookOverlapInteractor
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HookOverlapInteractor")
	void OnHookOverlapBegin(UPrimitiveComponent* OverlapComponent);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HookOverlapInteractor")
	void OnHookOverlapEnd(UPrimitiveComponent* OverlapComponent);
};
