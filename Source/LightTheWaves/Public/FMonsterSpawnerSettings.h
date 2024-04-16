#pragma once

#include "FMonsterSpawnerSettings.generated.h"

USTRUCT(BlueprintType)
struct FMonsterSpawnerParams
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MonsterSpawner")
	FVector2D MinMaxDistanceToBoat = FVector2D(2000.f, 3500.f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MonsterSpawner")
	float MinDistanceToNonTargetBoat = 600.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MonsterSpawner")
	float MinDistanceBehindNonTargetBoat = 1200.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MonsterSpawner")
	float MinDistanceBetweenMonsters = 600.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MonsterSpawner")
	float MinDistanceFromStart = 1500.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MonsterSpawner")
	float MinDistanceFromFinish = 1500.f;	
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MonsterSpawner")
	float MaxRadiusForLocationOffset = 50.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MonsterSpawner")
	int32 MaxSpawnAttempts = 3;	

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "MonsterSpawner")
	TSubclassOf<AActor> MonsterClass;
};