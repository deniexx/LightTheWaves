// Made By Cubic Burrito

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CBShopSubsystem.generated.h"

class UCBPurchasableDefinition;

UENUM(BlueprintType)
enum class EPurchaseFailReason
{
	AlreadyBoughtMax,
	NotEnoughCurrency,
	PlayerStateNotAvailable,

	None
};

USTRUCT(BlueprintType)
struct FPurchaseReturnInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "PurchaseReturnInfo")
	bool bWasSuccessful = false;

	UPROPERTY(BlueprintReadOnly, Category = "PurchaseReturnInfo")
	EPurchaseFailReason FailReason = EPurchaseFailReason::None;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnShopItemBought, UCBPurchasableDefinition*, Purchasable, int32, Amount);

/**
 * 
 */
UCLASS()
class LIGHTTHEWAVES_API UCBShopSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/**
	 * Attempt to purchase something from the shop
	 * @param PurchasableDefinition The definition data asset for the purchasable object
	 * @return PurchasableReturnInfo struct holding all necessary data about the transaction, including whether it was successful or not
	 */
	UFUNCTION(BlueprintCallable)
	FPurchaseReturnInfo TryPurchase(UCBPurchasableDefinition* PurchasableDefinition);
	
	void RegisterPlayerState(APlayerState* InPlayerState);

	UPROPERTY(BlueprintAssignable)
	FOnShopItemBought OnShopItemBought;

private:

	UPROPERTY()
	TMap<FGameplayTag, int32> BoughtUpgrades;

	UPROPERTY()
	APlayerState* PlayerState;
};
