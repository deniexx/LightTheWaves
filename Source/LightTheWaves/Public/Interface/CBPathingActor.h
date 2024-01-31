// Made By Cubic Burrito

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CBPathingActor.generated.h"

class USplineComponent;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCBPathingActor : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LIGHTTHEWAVES_API ICBPathingActor
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, Category = "PathingActor")
	void SetPath(USplineComponent* NewPath);
};
