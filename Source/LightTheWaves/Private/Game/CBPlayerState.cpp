// Made By Cubic Burrito


#include "Game/CBPlayerState.h"

#include "CBBlueprintFunctionLibrary.h"
#include "CBInitialiser.h"
#include "LightTheWaves/LightTheWaves.h"
#include "Subsystem/CBShopSubsystem.h"

void ACBPlayerState::BeginPlay()
{
	Super::BeginPlay();

	UCBInitialiser* Initialiser = UCBBlueprintFunctionLibrary::GetInitialisationSubsystem(this);
	if (!Initialiser)
	{
		UE_LOG(CBLog, Error, TEXT("Initialiser not available at BeginPlay of CBPlayerState"));
	}
	else
	{
		Initialiser->RegisterPlayerState(this);
	}

	UCBShopSubsystem* ShopSubsystem = UCBBlueprintFunctionLibrary::GetShopSubsystem(this);
	if (!ShopSubsystem)
	{
		UE_LOG(CBLog, Error, TEXT("ShopSubsystem not available at BeginPlay of CBPlayerState"));
	}
	else
	{
		ShopSubsystem->RegisterPlayerState(this);
	}
}

void ACBPlayerState::ApplyChangeToCurrency_Implementation(int32 Delta)
{
	const int32 OldCurrency = Currency;
	Currency = FMath::Clamp(Currency + Delta, 0 , 999);

	if (OldCurrency != Currency)
	{
		OnCurrencyChanged.Broadcast(OldCurrency, Currency);
	}
}

void ACBPlayerState::ApplyChangeToPoints_Implementation(int32 Delta)
{
	const int32 OldPoints = Points;
	Points = FMath::Clamp(Points + Delta, 0 , 1000000);

	if (OldPoints != Points)
	{
		OnPointsChanged.Broadcast(OldPoints, Points);
	}
}

int32 ACBPlayerState::GetCurrency_Implementation() const
{
	return Currency;
}

int32 ACBPlayerState::GetPoints_Implementation() const
{
	return Points;
}

bool ACBPlayerState::HasEnoughCurrency_Implementation(int32 AmountToCheck)
{
	return Currency >= AmountToCheck;
}

FOnAttributeChanged& ACBPlayerState::OnCurrencyChangedEvent()
{
	return OnCurrencyChanged;
}

FOnAttributeChanged& ACBPlayerState::OnPointsChangedEvent()
{
	return OnPointsChanged;
}
