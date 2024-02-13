// Made By Cubic Burrito

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CBDestroyableObject.generated.h"

UENUM(BlueprintType)
enum class EDestroyingObject : uint8
{
	Debris,
	Monster,
	Unknown
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCBDestroyableObject : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LIGHTTHEWAVES_API ICBDestroyableObject
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "DestroyableObject")
	void OnDestroyed(AActor* InstigatorActor, EDestroyingObject DestroyingObject);
};
