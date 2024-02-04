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
	// Spawning is disabled
	None,
};

USTRUCT(BlueprintType)
struct FBoatSpawningSettings
{
	GENERATED_BODY()

	/** The classes of boats to be spawned(this can be improved by adding in weights for randomization, etc...) */
	UPROPERTY(EditDefaultsOnly, Category = "Spawning|Boats")
	TArray<TSubclassOf<AActor>> SpawnableBoatClasses;
	
	/** Should a curve or the array be used to find the maximum number of boats per path
	 * TRUE - Curve will be used
	 * FALSE - Array will be used
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Spawning|Boats")
	bool bUseCurveForMaxBoatsPerPath = false;
	
	/** The maximum number of boats per path per wave(index is wave number), if this is <= 0, then there is no maximum */
	UPROPERTY(EditDefaultsOnly, Category = "Spawning|Boats", meta = (EditCondition = "!bUseCurveForMaxBoatsPerPath", EditConditionHides))
	TArray<int32> MaxBoatsPerPath;

	/** A curve of the max number of boats per path to wave number, <= 0 will be regarded as no maximum */
	UPROPERTY(EditDefaultsOnly, Category = "Spawning|Boats", meta = (EditCondition = "bUseCurveForMaxBoatsPerPath", EditConditionHides))
	TObjectPtr<UCurveFloat> MaxBoatsPerPathCurve = nullptr;

	/** The spawning mode to be used when spawning boats
	 * Period - Spawns boats within a given period(delay between spawns)
	 * MaxPerWave - Spawns boats throughout the round based on a maximum number of boats to be spawned
	 * None - Disabled spawning
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Spawning|Boats")
	EBoatSpawningMode BoatSpawningMode = EBoatSpawningMode::None;

	/** Should a curve or the array be used to find the number of boats to spawn this wave
	 * TRUE - Curve will be used
	 * FALSE - Array will be used
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Spawning|Boats")
	bool bUseCurveForBoatSpawnAmount = false;

	/** A curve of period of boat spawning(duration between spawns) to wave number */
	UPROPERTY(EditDefaultsOnly, Category = "Spawning|Boats", meta = (EditCondition = "!bUseCurveForBoatSpawnAmount", EditConditionHides))
	TArray<int32> BoatSpawnArray;

	/** A curve of period of boat spawning(duration between spawns) to wave number */
	UPROPERTY(EditDefaultsOnly, Category = "Spawning|Boats", meta = (EditCondition = "bUseCurveForBoatSpawnAmount", EditConditionHides))
	TObjectPtr<UCurveFloat> BoatSpawnCurve;

	/** The wave number to switch to using formula for getting the number of boats to spawn and the maximum number of boats per path */
	UPROPERTY(EditDefaultsOnly, Category = "Spawning|Boats", meta = (EditCondition = "bUseCurveForBoatSpawnCurve||bUseCurveForMaxBoatsPerPath", EditConditionHides))
	int32 WaveNumberToSwitchToFormula = 999;
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
	virtual void RegisterPathingActorWithPath_Implementation(AActor* ActorToRegister, USplineComponent* TargetPath) override;
	/** End Path Provider Interface */
	
protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Spawning|Boats")
	FBoatSpawningSettings BoatSpawningSettings;
	
	UPROPERTY()
	TArray<AActor*> PathActors;
	
	UPROPERTY()
	FTimerHandle BoatSpawnTimerHandle;
	
	UFUNCTION()
	void SpawnBoat();

	/** DUMMY VARIABLE */
	int32 WaveNumber = 1;

	/** DUMMY VARIABLE */
	float WaveDuration = 5.f;

private:
	
	float GetBoatSpawnPeriod();
	void ProcessBoatSpawning();
	bool TrySpawnBoat();

	/** Will grab a random free path and put in the OutPath variable(can fail) */
	bool IsAnyPathFree(int32 MaxBoatsPerPath, AActor*& OutPath);
	TSubclassOf<AActor> GetRandomSpawnableBoat();
};
