// Made By Cubic Burrito

#pragma once

#include "CoreMinimal.h"
#include "Engine/CurveTable.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"
#include "Interface/CBPathProvider.h"
#include "GameFramework/GameModeBase.h"
#include "Interface/CBWaveDirector.h"
#include "CBGameMode.generated.h"

class UEnvQuery;

USTRUCT(BlueprintType)
struct FBoatSpawningSettings
{
	GENERATED_BODY()

	/** The classes of boats to be spawned(this can be improved by adding in weights for randomization, etc...) */
	UPROPERTY(EditDefaultsOnly, Category = "Spawning|Boats")
	TArray<TSubclassOf<AActor>> SpawnableBoatClasses;

	/** The maximum number of boats to spawned per round (goes towards the second value throughout the round) */
	UPROPERTY(EditDefaultsOnly, Category = "Spawning|Boats")
	TArray<FVector2D> MaxBoats;

	UPROPERTY(EditDefaultsOnly, Category = "Spawning|Boats", meta = (Units = "cm"))
	float MinimumDistanceFromPathStart = 300.f;

	UPROPERTY(EditDefaultsOnly, Category = "Spawning|Boats")
	float MinTimeVariation = -0.5f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Spawning|Boats", meta = (Units = "s"))
	float MaxTimeVariation = 3.f;

	UPROPERTY(EditDefaultsOnly, Category = "Spawning|Boats", meta = (Units = "cm"))
	float MinDistanceVariation = -100.f;

	UPROPERTY(EditDefaultsOnly, Category = "Spawning|Boats", meta = (Units = "cm"))
	float MaxDistanceVariation = 100.f;
	
	/** Should a curve or the array be used to find the maximum number of boats per path
	 * TRUE - Curve will be used
	 * FALSE - Array will be used
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Spawning|Boats")
	bool bUseCurveForMaxBoatsPerPath = false;
	
	/** The maximum number of boats per path per wave(index is wave number), if this is <= 0, then there is no maximum */
	UPROPERTY(EditDefaultsOnly, Category = "Spawning|Boats", meta = (EditCondition = "!bUseCurveForMaxBoatsPerPath", EditConditionHides))
	TArray<FVector2D> MaxBoatsPerPath;

	/** A curve of the max number of boats per path to wave number, <= 0 will be regarded as no maximum */
	UPROPERTY(EditDefaultsOnly, Category = "Spawning|Boats", meta = (EditCondition = "bUseCurveForMaxBoatsPerPath", EditConditionHides))
	FCurveTableRowHandle StartOfRoundMaxBoatsPerPathCurve;

	/** A curve of the max number of boats per path to wave number, <= 0 will be regarded as no maximum */
	UPROPERTY(EditDefaultsOnly, Category = "Spawning|Boats", meta = (EditCondition = "bUseCurveForMaxBoatsPerPath", EditConditionHides))
	FCurveTableRowHandle EndOfRoundMaxBoatsPerPathCurve;

	/** Should a curve or the array be used to find the number of boats to spawn this wave
	 * TRUE - Curve will be used
	 * FALSE - Array will be used
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Spawning|Boats")
	bool bUseCurveForBoatSpawnAmount = false;

	/** A curve of period of boat spawning(duration between spawns X: Start of Round, Y: End of Round) to wave number */
	UPROPERTY(EditDefaultsOnly, Category = "Spawning|Boats", meta = (EditCondition = "!bUseCurveForBoatSpawnAmount", EditConditionHides))
	TArray<FVector2D> BoatSpawnPeriods;

	/** A curve of period of boat spawning(duration between spawns) to wave number */
	UPROPERTY(EditDefaultsOnly, Category = "Spawning|Boats", meta = (EditCondition = "bUseCurveForBoatSpawnAmount", EditConditionHides))
	FCurveTableRowHandle StartOfRoundBoatSpawnCurve;

	/** A curve of period of boat spawning(duration between spawns) to wave number */
	UPROPERTY(EditDefaultsOnly, Category = "Spawning|Boats", meta = (EditCondition = "bUseCurveForBoatSpawnAmount", EditConditionHides))
	FCurveTableRowHandle EndOfRoundBoatSpawnCurve;
};

USTRUCT(BlueprintType)
struct FMonsterSpawningSettings
{
	GENERATED_BODY()

	/** The minimum value to be used for the spawn period randomization */
	UPROPERTY(EditDefaultsOnly, Category = "Spawning|Monsters")
	float MinTimeVariation = -0.5f;

	/** The maximum value to be used for the spawn period randomization */
	UPROPERTY(EditDefaultsOnly, Category = "Spawning|Monsters", meta = (Units = "s"))
	float MaxTimeVariation = 3.f;

	/** Should a curve or the array be used to find the monster spawning period
	 * TRUE - Curve will be used
	 * FALSE - Array will be used
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Spawning|Monsters")
	bool bUseCurveForMonsterSpawnPeriod = false;
	
	UPROPERTY(EditDefaultsOnly, Category = "Spawning|Monsters", meta = (EditCondition="!bUseCurveForMonsterSpawnPeriod", EditConditionHides))
	TArray<float> MonsterSpawnPeriods;

	UPROPERTY(EditDefaultsOnly, Category = "Spawning|Monsters", meta = (EditCondition="bUseCurveForMonsterSpawnPeriod", EditConditionHides))
	FCurveTableRowHandle MonsterSpawnCurve;

	/** Should a curve or the array be used to find the max numbers of monsters spawned on a given path
	 * TRUE - Curve will be used
	 * FALSE - Array will be used
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Spawning|Monsters")
	bool bUseCurveForMaxMonstersOnPath = false;

	/** Should the number of boats be used to find the max number of spawned spawned on it
	 *  TRUE - The number of boats will be used (by also adding the AdditionalMaxMonstersPerPath)
	 *  FALSE - The curve of the array will be used
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Spawning|Monsters")
	bool bUseBoatsAsForMaxMonsterOnPath = false;

	/** An array containing the max number of monsters on a path for a given wave */
	UPROPERTY(EditDefaultsOnly, Category = "Spawning|Monsters", meta = (EditCondition="!bUseCurveForMaxMonstersOnPath && !bUseBoatsAsForMaxMonsterOnPath", EditConditionHides))
	TArray<float> MaxMonstersOnPath;

	/** A curve containing the max number of monsters on a path for a given wave */
	UPROPERTY(EditDefaultsOnly, Category = "Spawning|Monsters", meta = (EditCondition="bUseCurveForMaxMonstersOnPath && !bUseBoatsAsForMaxMonsterOnPath", EditConditionHides))
	FCurveTableRowHandle MaxMonstersOnPathCurve;

	/** The maximum radius for offsetting the monster spawn location (make them look a bit more realistic) */
	UPROPERTY(EditDefaultsOnly, Category = "Spawning|Monsters")
    float MaxRadiusForLocationOffset = 50.f;

	/** This number will be added to the number of boats on the path to get the max number of monster spawnable on the path */
	UPROPERTY(EditDefaultsOnly, Category = "Spawning|Monsters", meta = (EditCondition="bUseBoatsAsForMaxMonsterOnPath", EditConditionHides))
	int32 AdditionalMaxMonstersPerPath = 1;

	/** The minimum distance from the path starting point where monsters can spawn */
	UPROPERTY(EditDefaultsOnly, Category = "Spawning|Monsters", meta = (Units = "cm"))
	float MinimumDistanceFromPathStart = 1000.f;

	/** The monster class to be spawned (can be changed to an array if we decide to have multiple monsters) */
	UPROPERTY(EditDefaultsOnly, Category = "Spawning|Monsters")
	TSubclassOf<AActor> MonsterClass;
};

USTRUCT(BlueprintType)
struct FBossSpawningSettings
{
	GENERATED_BODY()

	/** The actor class to be used to spawn the boss */
	UPROPERTY(EditDefaultsOnly, Category = "Spawning|Boss")
	TSubclassOf<AActor> BossActorClass;

	/** The minimum time elapsed during the wave, for the boss to be spawned */
	UPROPERTY(EditDefaultsOnly, Category = "Spawning|Boss")
	float BossSpawnTimePeriodMin = 180.f;

	/** The maximum time elapsed during the wave, for the boss to be spawned */
	UPROPERTY(EditDefaultsOnly, Category = "Spawning|Boss")
	float BossSpawnTimePeriodMax = 240.f;
};

class USplineComponent;
/**
 * 
 */
UCLASS()
class LIGHTTHEWAVES_API ACBGameMode : public AGameModeBase, public ICBPathProvider, public ICBWaveDirector
{
	GENERATED_BODY()

public:

	/** Path Provider Interface */
	virtual USplineComponent* GetRandomPath_Implementation() override;
	virtual USplineComponent* GetClosestPath_Implementation(AActor* ReferenceActor) override;
	virtual void RegisterPathingActorWithPath_Implementation(AActor* ActorToRegister, USplineComponent* TargetPath) override;
	/** End Path Provider Interface */

	/** Wave Director Interface */
	virtual void StartWaveGameplay_Implementation() override;
	virtual FOnActivityStateUpdated& OnActivityStateUpdatedEvent() override;
	/** End Wave Director Interface */
	
protected:

	virtual void BeginPlay() override;

	/** Starts the gameplay loop */
	UFUNCTION(Exec)
	void TestGameplay();

	/** The settings that will be used when spawning boats */
	UPROPERTY(EditDefaultsOnly, Category = "Spawning|Boats")
	FBoatSpawningSettings BoatSpawningSettings;

	/** The settings that will be used when spawning monsters */
	UPROPERTY(EditDefaultsOnly, Category = "Spawning|Monsters")
	FMonsterSpawningSettings MonsterSpawningSettings;

	/** The environment query that will be used to find monster spawn locations */
	UPROPERTY(EditDefaultsOnly, Category = "Spawning|Monsters")
	TObjectPtr<UEnvQuery> MonsterSpawnQuery;

	UPROPERTY(EditDefaultsOnly, Category = "Spawning|Boss")
	FBossSpawningSettings BossSpawningSettings;

	/** The wave number to switch to using formula for spawn settings */
	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	int32 WaveNumberToSwitchToFormula = 999;

	/** How long should a wave last for? */
	UPROPERTY(EditDefaultsOnly, Category = "Waves", meta = (Units = "s"))
	float WaveDuration = 300.f;

	/** How long should the recess period last for? */
	UPROPERTY(EditDefaultsOnly, Category = "Waves", meta = (Units = "s"))
	float RecessPeriodDuration = 30.f;
	
	UPROPERTY(BlueprintAssignable, Category = "Activity")
	FOnActivityStateUpdated OnActivityStateUpdated;

	int32 WaveNumber = 0;

	UPROPERTY()
	TArray<AActor*> PathActors;
	
	UPROPERTY()
	FTimerHandle BoatSpawnTimerHandle;

	UPROPERTY()
	FTimerHandle MonsterSpawnTimerHandle;

	UPROPERTY()
	FTimerHandle BossSpawnTimerHandle;

	UPROPERTY()
	FTimerHandle WaveTimerHandle;

	UPROPERTY()
	FTimerHandle RecessTimerHandle;

private:
	
	float GetBoatSpawnPeriod();
	void SpawnBoat(AActor* PathActor);
	void ProcessBoatSpawning();
	void ProcessMonsterSpawning();
	void SpawnBoss();
	void ProcessBossSpawning();
	void RunMonsterSpawnEQS();
	bool TrySpawnBoat();
	bool IsAtMaxBoats();
	
	void StartNewWave(EGameActivity PreviousActivity = EGameActivity::None);

	int32 GetMaxAllowedMonstersOnPath(AActor* Path);

	float GetRoundTimeElapsed() const;
	float GetPercentRoundTimeElapsed() const;

	UFUNCTION()
	void SpawnBoat_TimerElapsed();

	UFUNCTION()
	void SpawnMonster(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

	UFUNCTION()
	void RecessTimer_Elapsed();
	
	UFUNCTION()
	void WaveTimer_Elapsed();
	
	UFUNCTION()
	void OnBossKilled(AActor* DestroyedActor);

	/** Will grab a random free path and put in the OutPath variable(can fail) */
	bool IsAnyPathFree(int32 MaxBoatsPerPath, AActor*& OutPath);
	TSubclassOf<AActor> GetRandomSpawnableBoat();

	AActor* GetRandomSpline(USplineComponent*& OutSplineComponent);
	AActor* GetSplineClosestToLocation(const FVector& Location, USplineComponent*& OutSplineComponent);
};
