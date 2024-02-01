// Made By Cubic Burrito

#pragma once

#include "CoreMinimal.h"
#include "Interface/CBPathProvider.h"
#include "GameFramework/GameModeBase.h"
#include "CBGameMode.generated.h"

UENUM(BlueprintType)
enum class EBoatSpawningMode
{
	// Spawns boats within a given period(delay between spawns)
	Period,
	// Spawns boats throughout the round based on a maximum number of boats to be spawned
	MaxPerWave,
	
	None,
};

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
	virtual USplineComponent* GetRandomPath_Implementation() override;
	virtual USplineComponent* GetClosestPath_Implementation(AActor* ReferenceActor) override;
	/** End Path Provider Interface */
	
protected:

	virtual void BeginPlay() override;
	
	UPROPERTY()
	TArray<USplineComponent*> SplinePaths;

	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	TArray<TSubclassOf<AActor>> SpawnBoatClasses;

	/** A curve of period of boat spawning(duration between spawns) to wave number(waves not implemented yet, so it will only use the first wave) */
	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	TObjectPtr<UCurveFloat> BoatSpawnCurve;
	
	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	EBoatSpawningMode BoatSpawningMode = EBoatSpawningMode::None;
	
	UPROPERTY()
	FTimerHandle BoatSpawnTimerHandle;

	UFUNCTION()
	void SpawnBoat();

private:

	void ProcessBoatSpawning();

};
