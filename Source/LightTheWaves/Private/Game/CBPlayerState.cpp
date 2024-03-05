// Made By Cubic Burrito


#include "Game/CBPlayerState.h"

#include "CBBlueprintFunctionLibrary.h"
#include "CBInitialiser.h"
#include "LightTheWaves/LightTheWaves.h"
#include "Subsystem/CBShopSubsystem.h"

static TAutoConsoleVariable<int32> CVarInfiniteMoney(
	TEXT("CB.InfiniteMoney"),
	0,
	TEXT("Enabled/Disables Infinite Money")
	TEXT(" 0: Money is NOT infinite/n")
	TEXT(" 1: Money is infinite/n"),
	ECVF_Cheat
);

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

	Reputation = StartingReputation;
	MaxReputation = StartingReputation;
}

void ACBPlayerState::ApplyChangeToCurrency_Implementation(int32 Delta)
{
	const bool bInfiniteMoney = CVarInfiniteMoney.GetValueOnAnyThread() > 0;
	if (bInfiniteMoney)
	{
		return;
	}
	
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

void ACBPlayerState::ApplyChangeToPlayerReputation_Implementation(float Delta)
{
	const float OldReputation = Reputation;
	Reputation = FMath::Clamp(Reputation + Delta, 0, MaxReputation);

	if (OldReputation != Reputation)
	{
		OnReputationChanged.Broadcast(OldReputation, Reputation, MaxReputation);
		UE_LOG(CBLog, Log, TEXT("Reputation changed! OldReputation: [%f], CurrentReputation: [%f], Delta: [%f]."), OldReputation, Reputation, Reputation - OldReputation);
	}

	if (Reputation <= 0.001f)
	{
		FGameLostData Data;
		Data.LoseReason = "Lost too much reputation";
		OnGameLost.Broadcast(Data);
	}
}

void ACBPlayerState::IncreaseMaxReputation_Implementation(float IncreaseAmount)
{
	const float OldMaxReputation = MaxReputation;
	MaxReputation += IncreaseAmount;
	const float Delta = MaxReputation - OldMaxReputation;

	Execute_ApplyChangeToPlayerReputation(this, Delta);
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
	return (Currency >= AmountToCheck) || CVarInfiniteMoney.GetValueOnAnyThread() > 0;
}

float ACBPlayerState::GetPlayerReputation_Implementation() const
{
	return Reputation;
}

FOnAttributeChanged& ACBPlayerState::OnCurrencyChangedEvent()
{
	return OnCurrencyChanged;
}

FOnAttributeChanged& ACBPlayerState::OnPointsChangedEvent()
{
	return OnPointsChanged;
}

FOnReputationChanged& ACBPlayerState::OnReputationChangedEvent()
{
	return OnReputationChanged;
}

FOnGameLost& ACBPlayerState::OnGameLostEvent()
{
	return OnGameLost;
}
