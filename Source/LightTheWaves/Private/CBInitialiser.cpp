// Made By Cubic Burrito


#include "CBInitialiser.h"

void UCBInitialiser::RegisterObjectToInitialiser(UObject* ObjectToAdd)
{
	ObjectsToInit.Add(ObjectToAdd);

	if (IsInitPossible())
	{
		InitObjects();
	}
}

void UCBInitialiser::RegisterGameMode(AGameModeBase* InGameMode)
{
	InitData.GameMode = InGameMode;
	if (IsInitPossible())
	{
		InitObjects();
	}
}

void UCBInitialiser::RegisterPlayerPawn(APawn* InPlayerPawn)
{
	InitData.PlayerPawn = InPlayerPawn;

	if (IsInitPossible())
	{
		InitObjects();
	}
}

void UCBInitialiser::RegisterPlayerState(APlayerState* InPlayerState)
{
	InitData.PlayerState = InPlayerState;
	if (IsInitPossible())
	{
		InitObjects();
	}
}

void UCBInitialiser::InitObjects()
{
	for (const auto Object : ObjectsToInit)
	{
		ensureMsgf(Object->Implements<UCBInitialiserInterface>(), TEXT("Object [%ls] does not implement Initialiser interface"), *GetNameSafe(Object));
		ICBInitialiserInterface::Execute_Init(Object, InitData);
	}

	ObjectsToInit.Empty();
}

bool UCBInitialiser::IsInitPossible() const
{
	return InitData.GameMode && InitData.PlayerPawn && InitData.PlayerState;
}
