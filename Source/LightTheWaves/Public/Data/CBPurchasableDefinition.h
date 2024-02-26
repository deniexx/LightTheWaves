// Made By Cubic Burrito

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "CBPurchasableDefinition.generated.h"

class UCBPurchasableAction;
/**
 * 
 */
UCLASS()
class LIGHTTHEWAVES_API UCBPurchasableDefinition : public UDataAsset
{
	GENERATED_BODY()

public:

	/** This will be used to identify the upgrade so that we do not have copies of the upgrade */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PurchasableDefinition")
	FGameplayTag IdentityTag;

	/** The name that will be shown to the player */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PurchasableDefinition")
	FName UpgradeName;

	/** How much should the upgrade cost */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PurchasableDefinition")
	int32 Cost;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PurchasableDefinition")
	int32 MaxPurchases = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PurchasableDefinition")
	bool bIsInfinite = false;

	/** The icon that will be displayed on the button */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PurchasableDefinition")
	TSoftObjectPtr<UTexture2D> UpgradeIcon;

	/** The action to be executed once the upgrade has been bought */
	UPROPERTY(EditDefaultsOnly, Category = "PurchasableDefinition")
	TSubclassOf<UCBPurchasableAction> PurchasableAction;
};
