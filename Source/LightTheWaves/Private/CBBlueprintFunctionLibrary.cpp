// Made By Cubic Burrito


#include "CBBlueprintFunctionLibrary.h"

#include "CBInitialiser.h"
#include "Game/CBGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystem/CBShopSubsystem.h"

UCBInitialiser* UCBBlueprintFunctionLibrary::GetInitialisationSubsystem(UObject* WorldContextObject)
{
	if (const UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject))
	{
		return GameInstance->GetSubsystem<UCBInitialiser>();
	}
	return nullptr;
}

bool UCBBlueprintFunctionLibrary::RegisterObjectForInitialisation(UObject* WorldContextObject, UObject* ObjectToRegister)
{
	if (UCBInitialiser* Initialiser = GetInitialisationSubsystem(WorldContextObject))
	{
		Initialiser->RegisterObjectToInitialiser(ObjectToRegister);
		return true;
	}

	return false;
}

UCBShopSubsystem* UCBBlueprintFunctionLibrary::GetShopSubsystem(UObject* WorldContextObject)
{
	if (const UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(WorldContextObject))
	{
		return GameInstance->GetSubsystem<UCBShopSubsystem>();
	}
	return nullptr;
}

bool UCBBlueprintFunctionLibrary::ShouldPlayTutorial(UObject* WorldContextObject)
{
	AGameModeBase* GameMode = UGameplayStatics::GetGameMode(WorldContextObject);
	const ACBGameMode* CBGameMode = GameMode ? Cast<ACBGameMode>(GameMode) : nullptr;

	return CBGameMode ? CBGameMode->bPlayTutorial : false;
}

void UCBBlueprintFunctionLibrary::SetTutorialEnabled(UObject* WorldContextObject, bool bEnabled)
{
	AGameModeBase* GameMode = UGameplayStatics::GetGameMode(WorldContextObject);
	ACBGameMode* CBGameMode = GameMode ? Cast<ACBGameMode>(GameMode) : nullptr;
	if (CBGameMode)
	{
		CBGameMode->bPlayTutorial = bEnabled;
	}
}

void UCBBlueprintFunctionLibrary::AnalyticsAddBoatKilledByTentacle(UObject* WorldContextObject)
{
	AGameModeBase* GameMode = UGameplayStatics::GetGameMode(WorldContextObject);
	ACBGameMode* CBGameMode = GameMode ? Cast<ACBGameMode>(GameMode) : nullptr;
	if (CBGameMode)
	{
		++CBGameMode->BoatsKilledByTentacle;
	}
}

void UCBBlueprintFunctionLibrary::AnalyticsAddBoatKilledByRocks(UObject* WorldContextObject)
{
	AGameModeBase* GameMode = UGameplayStatics::GetGameMode(WorldContextObject);
	ACBGameMode* CBGameMode = GameMode ? Cast<ACBGameMode>(GameMode) : nullptr;
	if (CBGameMode)
	{
		++CBGameMode->BoatsKilledByRocks;
	}
}

void UCBBlueprintFunctionLibrary::AnalyticsAddNumberOfItemsPurchased(UObject* WorldContextObject, FString ItemName)
{
	AGameModeBase* GameMode = UGameplayStatics::GetGameMode(WorldContextObject);
	ACBGameMode* CBGameMode = GameMode ? Cast<ACBGameMode>(GameMode) : nullptr;
	if (CBGameMode)
	{
		if (CBGameMode->ItemPurchases.Contains(ItemName))
		{
			++CBGameMode->ItemPurchases[ItemName];
		}
		else
		{
			CBGameMode->ItemPurchases.Add(ItemName, 1);
		}
	}
}

void UCBBlueprintFunctionLibrary::AnalyticsAddBoatReachedPort(UObject* WorldContextObject)
{
	AGameModeBase* GameMode = UGameplayStatics::GetGameMode(WorldContextObject);
	ACBGameMode* CBGameMode = GameMode ? Cast<ACBGameMode>(GameMode) : nullptr;
	if (CBGameMode)
	{
		++CBGameMode->BoatsToPort;
	}
}

void UCBBlueprintFunctionLibrary::AnalyticsAddBoatKilledByDebris(UObject* WorldContextObject)
{
	AGameModeBase* GameMode = UGameplayStatics::GetGameMode(WorldContextObject);
	ACBGameMode* CBGameMode = GameMode ? Cast<ACBGameMode>(GameMode) : nullptr;
	if (CBGameMode)
	{
		++CBGameMode->BoatsKilledByDebris;
	}
}

