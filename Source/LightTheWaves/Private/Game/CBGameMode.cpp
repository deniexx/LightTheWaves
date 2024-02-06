// Made By Cubic Burrito


#include "Game/CBGameMode.h"

#include "Actor/CBBoat.h"
#include "Components/SplineComponent.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "Interface/CBPath.h"
#include "Kismet/GameplayStatics.h"
#include "LightTheWaves/LightTheWaves.h"

#define BOAT_SPAWN_FORMULA(WaveNumber) WaveNumber
#define MAX_BOATS_PER_PATH_FORMULA(WaveNumber) 1
#define MONSTER_SPAWN_FORMULA(WaveNumber) WaveNumber

void ACBGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	UGameplayStatics::GetAllActorsWithTag(this, FName("Path"), PathActors);

	ProcessBoatSpawning();
	ProcessMonsterSpawning();
}

USplineComponent* ACBGameMode::GetRandomPath_Implementation()
{
	if (PathActors.Num() > 0)
	{
		return ICBPath::Execute_GetPath(PathActors[FMath::RandRange(0, PathActors.Num() - 1)]);
	}

	UE_LOG(CBLog, Error, TEXT("No Spline Paths available! Could be an issue, or the function was called too early."))
	return nullptr;
}

USplineComponent* ACBGameMode::GetClosestPath_Implementation(AActor* ReferenceActor)
{
	if (!ReferenceActor)
	{
		UE_LOG(CBLog, Error, TEXT("Attempting to find closest path for a nullptr ReferenceActor!"))
		return nullptr;
	}
	
	return GetSplineClosestToLocation(ReferenceActor->GetActorLocation());
}

void ACBGameMode::RegisterPathingActorWithPath_Implementation(AActor* ActorToRegister, USplineComponent* TargetPath)
{
	for (AActor* Actor : PathActors)
	{
		if (ICBPath::Execute_GetPath(Actor) == TargetPath)
		{
			ICBPath::Execute_RegisterBoatOnPath(Actor, ActorToRegister);
			return;
		}
	}
}

void ACBGameMode::ProcessBoatSpawning()
{
	if (BoatSpawningSettings.BoatSpawningMode == EBoatSpawningMode::None)
	{
		return;
	}

	const float BoatSpawnPeriod = GetBoatSpawnPeriod();
	GetWorldTimerManager().SetTimer(BoatSpawnTimerHandle, this, &ThisClass::SpawnBoat, BoatSpawnPeriod, false);
}

void ACBGameMode::SpawnBoat()
{
	if (BoatSpawningSettings.SpawnableBoatClasses.Num() <= 0)
	{
		checkf(false, TEXT("Spawn Boat Classes in Game Mode is not filled in!"));
	}

	TrySpawnBoat();
	ProcessBoatSpawning();
}

void ACBGameMode::ProcessMonsterSpawning()
{
	float MonsterSpawnPeriod;

	if (MonsterSpawningSettings.bUseCurveForMonsterSpawnPeriod && WaveNumberToSwitchToFormula >= WaveNumber)
	{
		MonsterSpawnPeriod = MonsterSpawningSettings.MonsterSpawnCurve->GetFloatValue(WaveNumber);
	}
	else if (!MonsterSpawningSettings.bUseCurveForMonsterSpawnPeriod && MonsterSpawningSettings.MonsterSpawnPeriods.Num() > WaveNumber)
	{
		MonsterSpawnPeriod = MonsterSpawningSettings.MonsterSpawnPeriods[WaveNumber - 1];
	}
	else
	{
		MonsterSpawnPeriod = MONSTER_SPAWN_FORMULA(WaveNumber);
	}

	GetWorldTimerManager().SetTimer(MonsterSpawnTimerHandle, this, &ThisClass::RunMonsterSpawnEQS, MonsterSpawnPeriod, false);
}

void ACBGameMode::RunMonsterSpawnEQS()
{
	UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(this, MonsterSpawnQuery, this, EEnvQueryRunMode::RandomBest5Pct, nullptr);

	if (ensure(QueryInstance))
	{
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ThisClass::SpawnMonster);
	}
}

void ACBGameMode::SpawnMonster(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(CBLog, Warning, TEXT("Monster Spawn Query Failed!"));
		ProcessMonsterSpawning();
		return;
	}

	const TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();
	if (Locations.Num() > 0)
	{
		const FVector RandomLocation = Locations[FMath::RandRange(0, Locations.Num() - 1)];
		const USplineComponent* ClosestSpline = GetSplineClosestToLocation(RandomLocation);
		FVector SpawnLocation = ClosestSpline->FindLocationClosestToWorldLocation(RandomLocation, ESplineCoordinateSpace::World);
		const float XOffset = FMath::FRandRange(0, MonsterSpawningSettings.MaxRadiusForLocationOffset);
		const float YOffset = FMath::FRandRange(0, MonsterSpawningSettings.MaxRadiusForLocationOffset);
		SpawnLocation += FVector(XOffset, YOffset, 0.f);

		// @NOTE: Might be worth to check if the path has a current boat(if prediction is added a future boat incoming)#
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		AActor* NewMonster = GetWorld()->SpawnActor<AActor>(MonsterSpawningSettings.MonsterClass, SpawnLocation, FRotator::ZeroRotator, SpawnParameters);

		ProcessMonsterSpawning();
	}
}

float ACBGameMode::GetBoatSpawnPeriod()
{
	float BoatSpawnPeriod;
	
	if (BoatSpawningSettings.bUseCurveForBoatSpawnAmount && WaveNumberToSwitchToFormula >= WaveNumber)
	{
		BoatSpawnPeriod = BoatSpawningSettings.BoatSpawnCurve->GetFloatValue(WaveNumber);
	}
	else if (!BoatSpawningSettings.bUseCurveForBoatSpawnAmount && BoatSpawningSettings.BoatSpawnArray.Num() > WaveNumber)
	{
		BoatSpawnPeriod = BoatSpawningSettings.BoatSpawnArray[WaveNumber];
	}
	else
	{
		BoatSpawnPeriod = BOAT_SPAWN_FORMULA(WaveNumber);
	}
	
	if (BoatSpawningSettings.BoatSpawningMode == EBoatSpawningMode::Period)
	{
		return BoatSpawnPeriod;
	}
	
	return BoatSpawnPeriod / WaveDuration;
}

bool ACBGameMode::TrySpawnBoat()
{
	float MaxBoatsPerPath;
	if (BoatSpawningSettings.bUseCurveForMaxBoatsPerPath && WaveNumberToSwitchToFormula >= WaveNumber)
	{
		MaxBoatsPerPath = BoatSpawningSettings.MaxBoatsPerPathCurve->GetFloatValue(WaveNumber);
	}
	else if (BoatSpawningSettings.MaxBoatsPerPath.Num() >= WaveNumber)
	{
		MaxBoatsPerPath = BoatSpawningSettings.MaxBoatsPerPath[WaveNumber - 1];
	}
	else
	{
		MaxBoatsPerPath = MAX_BOATS_PER_PATH_FORMULA(WaveNumber);
	}

	AActor* FreePathActor = nullptr;

	if (MaxBoatsPerPath <= 0)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		AActor* Boat = GetWorld()->SpawnActor<AActor>(GetRandomSpawnableBoat(), SpawnParameters);
		ICBPathingActor::Execute_SetPath(Boat, Execute_GetRandomPath(this));
		return true;
	}
	
	if (IsAnyPathFree(MaxBoatsPerPath, FreePathActor))
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		AActor* Boat = GetWorld()->SpawnActor<AActor>(GetRandomSpawnableBoat(), SpawnParameters);
		ICBPathingActor::Execute_SetPath(Boat, ICBPath::Execute_GetPath(FreePathActor));
		return true;
	}
	
	return false;
}

bool ACBGameMode::IsAnyPathFree(int32 MaxBoatsPerPath, AActor*& OutPath)
{
	TArray<AActor*> FreePaths;
	
	for (AActor* Actor : PathActors)
	{
		if (ICBPath::Execute_GetNumberOfBoatsOnPath(Actor) < MaxBoatsPerPath)
		{
			FreePaths.Add(Actor);
		}
	}

	if (FreePaths.Num() == 0)
	{
		return false;
	}

	// Grab a random free path, that way we don't just go in sequence
	OutPath = FreePaths[FMath::RandRange(0, FreePaths.Num() - 1)];
	return true;
}

TSubclassOf<AActor> ACBGameMode::GetRandomSpawnableBoat()
{
	// @TODO: Expand this function to be able to evaluate randomization weights, etc...
	return BoatSpawningSettings.SpawnableBoatClasses[FMath::RandRange(0, BoatSpawningSettings.SpawnableBoatClasses.Num() - 1)];
}

USplineComponent* ACBGameMode::GetSplineClosestToLocation(const FVector& Location)
{
	if (PathActors.Num() > 0)
	{
		USplineComponent* Closest = ICBPath::Execute_GetPath(PathActors[0]);
		float ClosestDistance = TNumericLimits<float>::Max();

		for (AActor* Actor : PathActors)
		{
			USplineComponent* Spline = ICBPath::Execute_GetPath(Actor);
			const float CurrentDistance = (Spline->FindLocationClosestToWorldLocation(Location, ESplineCoordinateSpace::World) - Location).Length();
			if (CurrentDistance < ClosestDistance)
			{
				ClosestDistance = CurrentDistance;
				Closest = Spline;
			}
		}
		
		return Closest;
	}

	UE_LOG(CBLog, Error, TEXT("No Spline Paths available! Could be an issue, or the function was called too early."))
	return nullptr;
}
