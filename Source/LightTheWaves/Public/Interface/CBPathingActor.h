// Made By Cubic Burrito

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CBPathingActor.generated.h"

class USplineComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPathingActorLeftPath, AActor*, PathingActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNewPathChosen, USplineComponent*, NewPath);

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

	UFUNCTION(BlueprintNativeEvent, Category = "PathingActor")
	USplineComponent* GetPath();	
	
	virtual FOnPathingActorLeftPath& PathingActorLeftPathEvent() = 0;

	virtual FOnNewPathChosen& NewPathChosenEvent() = 0;
};
