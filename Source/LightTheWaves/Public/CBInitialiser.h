// Made By Cubic Burrito

#pragma once

#include "CoreMinimal.h"
#include "Interface/CBInitialiserInterface.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CBInitialiser.generated.h"

class ICBInitialiserInterface;
/**
 * 
 */
UCLASS()
class LIGHTTHEWAVES_API UCBInitialiser : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	void RegisterObjectToInitialiser(UObject* ObjectToAdd);
	void RegisterGameMode(AGameModeBase* InGameMode);
	void RegisterPlayerPawn(APawn* InPlayerPawn);
	void RegisterPlayerState(APlayerState* InPlayerState);
	
private:

	void InitObjects();
	bool IsInitPossible() const;

	UPROPERTY()
	FInitData InitData;

	UPROPERTY()
	TArray<UObject*> ObjectsToInit;
};
