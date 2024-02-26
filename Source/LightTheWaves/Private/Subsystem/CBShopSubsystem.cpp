// Made By Cubic Burrito


#include "Subsystem/CBShopSubsystem.h"

#include "GameFramework/PlayerState.h"
#include "Data/CBPurchasableDefinition.h"
#include "Interface/CBPlayerInterface.h"
#include "LightTheWaves/LightTheWaves.h"
#include "Object/CBPurchasableAction.h"

FPurchaseReturnInfo UCBShopSubsystem::TryPurchase(UCBPurchasableDefinition* PurchasableDefinition)
{
	if (!PlayerState)
	{
		FPurchaseReturnInfo PurchaseReturnInfo;
		PurchaseReturnInfo.bWasSuccessful = false;
		PurchaseReturnInfo.FailReason = EPurchaseFailReason::PlayerStateNotAvailable;
		return PurchaseReturnInfo;
	}
	
	if (!ICBPlayerInterface::Execute_HasEnoughCurrency(PlayerState, PurchasableDefinition->Cost))
	{
		FPurchaseReturnInfo PurchaseReturnInfo;
		PurchaseReturnInfo.bWasSuccessful = false;
		PurchaseReturnInfo.FailReason = EPurchaseFailReason::NotEnoughCurrency;
		return PurchaseReturnInfo;
	}

	if(!PurchasableDefinition->bIsInfinite)
	{
		if (BoughtUpgrades.Contains(PurchasableDefinition->IdentityTag) && BoughtUpgrades[PurchasableDefinition->IdentityTag] >= PurchasableDefinition->MaxPurchases)
		{
			FPurchaseReturnInfo PurchaseReturnInfo;
			PurchaseReturnInfo.bWasSuccessful = false;
			PurchaseReturnInfo.FailReason = EPurchaseFailReason::AlreadyBoughtMax;
			return PurchaseReturnInfo;
		}
	}

	FPurchaseReturnInfo PurchaseReturnInfo;
	PurchaseReturnInfo.bWasSuccessful = true;
	PurchaseReturnInfo.FailReason = EPurchaseFailReason::None;

	if (!BoughtUpgrades.Contains(PurchasableDefinition->IdentityTag))
	{
		BoughtUpgrades.Add(PurchasableDefinition->IdentityTag);
	}
	
	++BoughtUpgrades[PurchasableDefinition->IdentityTag];

	ICBPlayerInterface::Execute_ApplyChangeToCurrency(PlayerState, -PurchasableDefinition->Cost);
	UCBPurchasableAction* PurchasableAction = NewObject<UCBPurchasableAction>(PlayerState, PurchasableDefinition->PurchasableAction);
	if (!PurchasableAction)
	{
		UE_LOG(CBLog, Error, TEXT("Purchasable Action not filled in for [%ls]"), *GetNameSafe(PurchasableDefinition));
	}
	else
	{
		PurchasableAction->OnPurchased(PlayerState);
	}
	
	return PurchaseReturnInfo;
}

void UCBShopSubsystem::RegisterPlayerState(APlayerState* InPlayerState)
{
	BoughtUpgrades.Reset();
	PlayerState = InPlayerState;
}
