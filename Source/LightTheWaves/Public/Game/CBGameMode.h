// Made By Cubic Burrito

#pragma once

#include "CoreMinimal.h"
#include "Interface/CBPathProvider.h"
#include "GameFramework/GameModeBase.h"
#include "CBGameMode.generated.h"

class USplineComponent;
/**
 * 
 */
UCLASS()
class LIGHTTHEWAVES_API ACBGameMode : public AGameModeBase, public ICBPathProvider
{
	GENERATED_BODY()

public:

	/** Path Provider Interface */
	virtual USplineComponent* GetPath_Implementation() override;
	/** End Path Provider Interface */
	
protected:

	virtual void BeginPlay() override;
	
	UPROPERTY()
	TArray<USplineComponent*> SplinePaths;
	
	AActor* SpawnBoat();
};
