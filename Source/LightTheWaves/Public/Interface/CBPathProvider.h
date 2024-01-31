// Made By Cubic Burrito

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "UObject/Interface.h"
#include "CBPathProvider.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCBPathProvider : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LIGHTTHEWAVES_API ICBPathProvider
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	// @TODO: Change this to have a name of GetRandomPath and create one where it provides a path closest the boat
	UFUNCTION(BlueprintNativeEvent, Category = "PathProvider")
	USplineComponent* GetPath();
};
