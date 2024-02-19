// Made By Cubic Burrito


#include "CBBlueprintFunctionLibrary.h"

#include "CBInitialiser.h"
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
