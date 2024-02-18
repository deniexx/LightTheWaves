// Made By Cubic Burrito

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CBInitialiserInterface.generated.h"

USTRUCT(BlueprintType)
struct FInitData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Initialisation")
	APlayerState* PlayerState = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Initialisation")
	APawn* PlayerPawn = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Initialisation")
	AGameModeBase* GameMode = nullptr;
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCBInitialiserInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LIGHTTHEWAVES_API ICBInitialiserInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, Category = "Initialisation")
	void Init(const FInitData& InitData);
};
