// Made By Cubic Burrito

#pragma once

#include "CoreMinimal.h"
#include "FMonsterSpawnerSettings.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "CBMonsterSpawnerAction.generated.h"

class USplineComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMonsterSpawned, AActor*, MonsterSpawned);

/**
 * 
 */
UCLASS()
class LIGHTTHEWAVES_API UCBMonsterSpawnerAction : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, Category = "MonsterSpawnerAction", meta = (DisplayName = "CreateMonsterSpawnAction"))
	static UCBMonsterSpawnerAction* Create(UObject* WorldContextObject, TArray<AActor*> InBoats, TArray<AActor*> InMonsters, TArray<AActor*> Paths, UPARAM(ref) FMonsterSpawnerParams& SpawnerParams);
	
	virtual void Activate() override;

	UPROPERTY(BlueprintAssignable, Category = "MonsterSpawnerAction")
	FOnMonsterSpawned OnMonsterSpawned;

	virtual UWorld* GetWorld() const override;

protected:
	void AttemptSpawn();
	void ShuffleBoatArray();
	
	void SpawnMonster(const FVector& Vector, AActor* Target) const;

	AActor* ChooseTarget();
	bool FindDesirableSpawnLocationAroundTarget(AActor* Actor, FVector& OutLocation);

	bool AnyMonsterCloseToLocation(const FVector& Vector, float MinDistanceRequired, AActor*& CloseActor);
	bool AttemptAdjustSpawnLocationForMonster(FVector& Location, const AActor* CloseActor, float AlongSplineDistanceToActor, const USplineComponent* Path) const;

	bool AnyBoatCloseToLocation(const FVector& Vector, float MinDistanceRequired, AActor*& CloseActor);
	bool AttemptAdjustSpawnLocationForBoat(FVector& Location, const AActor* CloseActor, float AlongSplineDistanceToActor, const USplineComponent* Path) const;

	UPROPERTY()
	TArray<AActor*> Boats;

	UPROPERTY()
	TArray<AActor*> Monsters;

	UPROPERTY()
	TArray<AActor*> Hazards;

	UPROPERTY()
	TArray<USplineComponent*> Paths;

	UPROPERTY()
	FMonsterSpawnerParams SpawnerParams;

	int32 SpawnAttempts = 0;

	AActor* CurrTarget;

	UPROPERTY()
	UObject* WCO;
};
