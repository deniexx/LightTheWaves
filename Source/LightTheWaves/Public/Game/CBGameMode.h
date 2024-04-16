// Made By Cubic Burrito

#pragma once

#include "CoreMinimal.h"
#include "Engine/CurveTable.h"
#include "Interface/CBPathProvider.h"
#include "GameFramework/GameModeBase.h"
#include "Interface/CBWaveDirector.h"
#include "FMonsterSpawnerSettings.h"
#include "Interface/CBInitialiserInterface.h"
#include "Interface/CBPlayerInterface.h"
#include "CBGameMode.generated.h"

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
	
	UPROPERTY(EditDefaultsOnly, Category = "Spawning|Monsters")
	float MonsterSpawnPeriod = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Spawning|Monsters")
	FMonsterSpawnerParams SpawnParams;
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
	/** The different transforms that could be used to spawn the boss tentacles */
	UPROPERTY(EditDefaultsOnly, Category = "Spawning|Boss")
	TArray<FTransform> SpawnTransforms;

	/** The number of tentacles that can appear in the wave */
	UPROPERTY(EditDefaultsOnly, Category = "Spawning|Boss")
	TArray<int32> MaxTentacles;

	UPROPERTY(EditDefaultsOnly, Category = "Spawning|Boss")
	TObjectPtr<USoundBase> BossSpawningSound;

	UPROPERTY(EditDefaultsOnly, Category = "Spawning|Boss")
	TObjectPtr<USoundBase> BossSong;

	UPROPERTY(EditDefaultsOnly, Category = "Spawning|Boss")
	float BossSpawnSoundDuration;
};

class USplineComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaveStarted, int32, WaveNumber);

/**
 * 
 */
UCLASS()
class LIGHTTHEWAVES_API ACBGameMode : public AGameModeBase, public ICBPathProvider, public ICBWaveDirector, public ICBInitialiserInterface
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
	virtual FOnEntitySpawned& OnBoatSpawnedEvent() override;
	virtual FOnEntitySpawned& OnMonsterSpawnedEvent() override;
	virtual FOnEntitySpawned& OnBossSpawnedEvent() override;
	virtual FOnGameStarted& OnGameStartedEvent() override;
	/** End Wave Director Interface */

	/** Initialiser Interface */
	void Init_Implementation(const FInitData& InitData) override;
	/** End Initialiser Interface */

	UPROPERTY(EditDefaultsOnly, Category = "Tutorial")
	bool bPlayTutorial;

	UFUNCTION(BlueprintCallable)
	void RecessTimer_Elapsed();

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

	UPROPERTY(EditDefaultsOnly, Category = "Spawning|Boss")
	FBossSpawningSettings BossSpawningSettings;

	/** The wave number to switch to using formula for spawn settings */
	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	int32 WaveNumberToSwitchToFormula = 999;

	/** How long should a wave last for? */
	UPROPERTY(EditDefaultsOnly, Category = "Waves", meta = (Units = "s"))
	float WaveDuration = 300.f;

	/** How long should the recess period last for? */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Waves", meta = (Units = "s"))
	float RecessPeriodDuration = 30.f;
	
	UPROPERTY(BlueprintAssignable, Category = "Activity")
	FOnActivityStateUpdated OnActivityStateUpdated;

	UPROPERTY(BlueprintAssignable, Category = "Activity")
	FOnEntitySpawned OnBoatSpawned;
		
	UPROPERTY(BlueprintAssignable, Category = "Activity")
	FOnEntitySpawned OnMonsterSpawnedEnt;
		
	UPROPERTY(BlueprintAssignable, Category = "Activity")
	FOnEntitySpawned OnBossSpawned;
		
	UPROPERTY(BlueprintAssignable, Category = "Activity")
	FOnGameStarted OnGameStarted;

	UPROPERTY(BlueprintAssignable, Category = "Activity")
	FOnWaveStarted OnWaveStarted;

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

	UFUNCTION(Exec)
	void ClearDebris();

	UFUNCTION(BlueprintNativeEvent)
	void OnGameFinished(const FGameLostData& Data);

	UFUNCTION(BlueprintNativeEvent)
	void PlayTutorial();

	UFUNCTION(BlueprintNativeEvent)
	void OnBossKilledTutorial();

private:

	UFUNCTION()
	void GameFinished(const FGameLostData& Data);
	
	float GetBoatSpawnPeriod();
	void ProcessBoatSpawning();
	void ProcessMonsterSpawning();

	UFUNCTION(BlueprintCallable)
	void SpawnBoss();
	
	void ProcessBossSpawning();
	bool TrySpawnBoat();
	bool IsAtMaxBoats();

	void StartNewWave(EGameActivity PreviousActivity = EGameActivity::None);
	
	float GetRoundTimeElapsed() const;
	float GetPercentRoundTimeElapsed() const;

	UFUNCTION()
	void SpawnBoat_TimerElapsed();
	
	UFUNCTION()
	void OnMonsterSpawned(AActor* SpawnedMonster);

	UFUNCTION()
	void OnMonsterDestroyed(AActor* DestroyedMonster);

	UFUNCTION()
	void OnBoatDestroyed(AActor* DestroyedBoat);
	
	UFUNCTION(BlueprintCallable)
	void SpawnMonster();

	UFUNCTION(BlueprintCallable)
	AActor* SpawnBoat(AActor* PathActor, TSubclassOf<AActor> BoatToSpawn);
	
	UFUNCTION()
	void WaveTimer_Elapsed();

	UFUNCTION(BlueprintCallable)
	void PlayBossSpawningMusic();
	
	UFUNCTION()
	void OnBossKilled(AActor* DestroyedActor);
	
	TSubclassOf<AActor> GetRandomSpawnableBoat();
	bool IsAnyPathFree(int32 MaxBoatsPerPath, AActor*& OutPath);

	UPROPERTY()
	TArray<AActor*> Boats;

	UPROPERTY()
	TArray<AActor*> Monsters;

	UPROPERTY()
	TArray<AActor*> Bosses;

	UPROPERTY()
	UAudioComponent* BossAudioComponent;
	
	bool bNextWaveWaitingForBoss = false;
	bool bNextWaveWaitingForBoats = false;

	int32 SpawnedBoats = 0;
	int32 DestroyedBoats = 0;
	
	AActor* GetRandomSpline(USplineComponent*& OutSplineComponent);
	AActor* GetSplineClosestToLocation(const FVector& Location, USplineComponent*& OutSplineComponent);
};
