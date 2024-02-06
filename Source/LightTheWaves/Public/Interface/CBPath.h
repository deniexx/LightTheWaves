// Made By Cubic Burrito

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CBPath.generated.h"

class USplineComponent;
// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCBPath : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LIGHTTHEWAVES_API ICBPath
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, Category = "Path")
	USplineComponent* GetPath();

	UFUNCTION(BlueprintNativeEvent, Category = "Path")
	int32 GetNumberOfBoatsOnPath();

	UFUNCTION(BlueprintNativeEvent, Category = "Path")
	int32 GetNumberOfMonstersOnPath();

	UFUNCTION(BlueprintNativeEvent, Category = "Path")
	void AddMonsterToPath(AActor* Monster);

	UFUNCTION(BlueprintNativeEvent, Category = "Path")
	void RegisterBoatOnPath(AActor* Boat);

	UFUNCTION(BlueprintNativeEvent, Category = "Path")
	bool IsBoatWithinDistanceFromStart(float Distance);
};
