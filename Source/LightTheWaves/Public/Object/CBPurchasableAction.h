// Made By Cubic Burrito

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CBPurchasableAction.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class LIGHTTHEWAVES_API UCBPurchasableAction : public UObject
{
	GENERATED_BODY()

public:

	/** This will be executed once the upgrade has been bought */
	UFUNCTION(BlueprintNativeEvent, Category = "PurchasableAction")
	void OnPurchased(APlayerState* PlayerState);
};
