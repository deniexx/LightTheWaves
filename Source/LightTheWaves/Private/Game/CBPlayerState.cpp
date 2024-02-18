// Made By Cubic Burrito


#include "Game/CBPlayerState.h"

#include "CBBlueprintFunctionLibrary.h"
#include "CBInitialiser.h"
#include "LightTheWaves/LightTheWaves.h"

class UCBInitialiser;

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

FOnAttributeChanged& ACBPlayerState::OnCurrencyChangedEvent()
{
	return OnCurrencyChanged;
}

FOnAttributeChanged& ACBPlayerState::OnPointsChangedEvent()
{
	return OnPointsChanged;
}

void ACBPlayerState::BeginPlay()
{
	Super::BeginPlay();

	UCBInitialiser* Initialiser = UCBBlueprintFunctionLibrary::GetInitialisationSubsystem(this);
	UE_LOG(CBLog, Error, TEXT("Initialiser not available at BeginPlay of CBPlayerState"));
	Initialiser->RegisterPlayerState(this);
}
