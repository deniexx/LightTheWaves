// Made By Cubic Burrito


#include "Async/CBMonsterSpawnerAction.h"

#include "Components/SplineComponent.h"
#include "Interface/CBMonsterInterface.h"
#include "Interface/CBPath.h"
#include "Interface/CBPathingActor.h"
#include "LightTheWaves/LightTheWaves.h"

UCBMonsterSpawnerAction* UCBMonsterSpawnerAction::Create(UObject* WorldContextObject, TArray<AActor*> InBoats,
                                                         TArray<AActor*> InMonsters, TArray<AActor*> InPaths, FMonsterSpawnerParams& SpawnerParams)
{
	UCBMonsterSpawnerAction* MonsterSpawnerAction = NewObject<UCBMonsterSpawnerAction>();
	MonsterSpawnerAction->Boats = InBoats;
	MonsterSpawnerAction->Monsters = InMonsters;
	MonsterSpawnerAction->SpawnerParams = MoveTemp(SpawnerParams);
	MonsterSpawnerAction->WCO = WorldContextObject;

	for (const auto Path : InPaths)
	{
		MonsterSpawnerAction->Paths.Add(ICBPath::Execute_GetPath(Path));
	}
	
	return MonsterSpawnerAction;
}

void UCBMonsterSpawnerAction::Activate()
{
	Super::Activate();

	SpawnAttempts = 0;
	AttemptSpawn();
}

UWorld* UCBMonsterSpawnerAction::GetWorld() const
{
	if (WCO)
	{
		return WCO->GetWorld();
	}

	// Attempt to get world from paths
	for (const auto Path : Paths)
	{
		if (Path)
		{
			return Path->GetWorld();
		}
	}

	for (const auto Boat : Boats)
	{
		if (Boat)
		{
			return Boat->GetWorld();
		}
	}

	// Something really wrong happened so it's fine to crash
	return nullptr;
}

void UCBMonsterSpawnerAction::AttemptSpawn()
{
	if (Boats.Num() == 0)
	{
		UE_LOG(CBLog, Warning, TEXT("No boats, skipping monster spawn!"));
		return;
	}

	if (Monsters.Num() >= Boats.Num() + 1)
	{
		UE_LOG(CBLog, Warning, TEXT("More monsters than boats, skipping monster spawn!"));
		return;
	}
	
	++SpawnAttempts;
	ShuffleBoatArray();
	if (AActor* Target = ChooseTarget())
	{
		CurrTarget = Target;
		FVector SpawnLocation;
		if (FindDesirableSpawnLocationAroundTarget(Target, SpawnLocation))
		{
			SpawnMonster(SpawnLocation, Target);
		}
		else if (SpawnAttempts < SpawnerParams.MaxSpawnAttempts)
		{
			UE_LOG(CBLog, Warning, TEXT("Couldn't find a desirable spawn location! Attempting another spawn"));
			AttemptSpawn();
		}
	}
	else if (Boats.Num() + 1 > Monsters.Num())
	{
		UE_LOG(CBLog, Warning, TEXT("Target not found, but will spawn through boat numbers"));
		// @NOTE: Not sure if we need this
	}
}

void UCBMonsterSpawnerAction::ShuffleBoatArray()
{
	Algo::Sort(Boats, [this](const AActor* Item1, const AActor* Item2)
	{
		// @NOTE: Adjust this if we want to add in weighting for ones that are further ahead
		return FMath::FRand() < 0.5f;
	});
}

void UCBMonsterSpawnerAction::SpawnMonster(const FVector& Vector, AActor* Target) const
{
	if (Vector.X < SpawnerParams.MinDistanceFromStart || Vector.X > 23000.f)
	{
		return;
	}
	
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	const FRotator Rotation = (Vector - Target->GetActorLocation()).Rotation() + FRotator(0.f, -90.f, 0.f);
	if (AActor* NewMonster = GetWorld()->SpawnActor<AActor>(SpawnerParams.MonsterClass, Vector, Rotation, SpawnParameters))
	{
		ICBMonsterInterface::Execute_SetTarget(NewMonster, Target);
		OnMonsterSpawned.Broadcast(NewMonster);
	}
}

AActor* UCBMonsterSpawnerAction::ChooseTarget()
{
	TArray<AActor*> TargetedBoats;
	for (const auto Monster : Monsters)
	{
		if (AActor* Target = ICBMonsterInterface::Execute_GetTarget(Monster))
		{
			TargetedBoats.Add(Target);
		}
	}
	
	for (const auto Boat : Boats)
	{
		if (TargetedBoats.Contains(Boat))
		{
			continue;
		}

		return Boat;
	}

	UE_LOG(CBLog, Warning, TEXT("Target not found!!"));
	return nullptr;
}

bool UCBMonsterSpawnerAction::FindDesirableSpawnLocationAroundTarget(AActor* Actor, FVector& OutLocation)
{
	const USplineComponent* Path = ICBPathingActor::Execute_GetPath(Actor);
	const float PathLength = Path->GetSplineLength();
	const float InputKey = Path->FindInputKeyClosestToWorldLocation(Actor->GetActorLocation());
	const float AlongSplineDistanceToActor = Path->GetDistanceAlongSplineAtSplineInputKey(InputKey);
	const float RandomDistanceToSpawn = FMath::FRandRange(SpawnerParams.MinMaxDistanceToBoat.X, SpawnerParams.MinMaxDistanceToBoat.Y);
	const float AlongSplineMaxDistanceToSpawn = PathLength - SpawnerParams.MinDistanceFromFinish;
	const float RandomOffsetDistance = AlongSplineDistanceToActor + RandomDistanceToSpawn;
	const float DistanceToSpawnOn = FMath::Clamp(RandomOffsetDistance, RandomOffsetDistance, AlongSplineMaxDistanceToSpawn);
	OutLocation = Path->GetWorldLocationAtDistanceAlongSpline(DistanceToSpawnOn);

	AActor* CloseActor;
	if (AnyMonsterCloseToLocation(OutLocation, SpawnerParams.MinDistanceBetweenMonsters, CloseActor))
	{
		if (!AttemptAdjustSpawnLocationForMonster(OutLocation, CloseActor, AlongSplineDistanceToActor, Path))
		{
			return false;
		}
	}
	CloseActor = nullptr;
	if (AnyBoatCloseToLocation(OutLocation, SpawnerParams.MinDistanceToNonTargetBoat, CloseActor))
	{
		// There is another boat that is close to our desired location so try to adjust, if not possible ignore this target and try with another one
		if (!AttemptAdjustSpawnLocationForBoat(OutLocation, CloseActor, AlongSplineDistanceToActor, Path))
		{
			// Spawning not possible
			return false;
		}
	}
	
	// Spawning Possible
	return true;
}

bool UCBMonsterSpawnerAction::AnyMonsterCloseToLocation(const FVector& Vector, float MinDistanceRequired, AActor*& CloseActor)
{
	for (const auto Monster : Monsters)
	{
		if ((Monster->GetActorLocation() - Vector).Length() < MinDistanceRequired)
		{
			CloseActor = Monster;
			return true;
		}
	}

	return false;
}

bool UCBMonsterSpawnerAction::AttemptAdjustSpawnLocationForMonster(FVector& Location, const AActor* CloseActor,
	float AlongSplineDistanceToActor, const USplineComponent* Path) const
{
	const float DistanceToCloseActor = (CloseActor->GetActorLocation() - Location).Length();
	const float DistanceOnSplineWay1 = AlongSplineDistanceToActor - DistanceToCloseActor;
	const float DistanceOnSplineWay2 = AlongSplineDistanceToActor + DistanceToCloseActor;
	const FVector SplineLocationWay1 = Path->GetWorldLocationAtDistanceAlongSpline(DistanceOnSplineWay1);
	const FVector SplineLocationWay2 = Path->GetWorldLocationAtDistanceAlongSpline(DistanceOnSplineWay2);
	
	if ((SplineLocationWay1 - Location).Length() > SpawnerParams.MinDistanceBetweenMonsters)
	{
		// Location adjusted successfully
		Location = SplineLocationWay1;
		return true;
	}
	if ((SplineLocationWay2 - Location).Length() > SpawnerParams.MinDistanceBetweenMonsters)
	{
		// Location adjusted successfully
		Location = SplineLocationWay2;
		return true;
	}
	
	return false;
}


bool UCBMonsterSpawnerAction::AnyBoatCloseToLocation(const FVector& Vector, float MinDistanceRequired, AActor*& CloseActor)
{
	for (const auto Boat : Boats)
	{
		if (Boat == CurrTarget) continue;
		if ((Boat->GetActorLocation() - Vector).Length() < MinDistanceRequired)
		{
			if (Boat->GetActorLocation().X - Vector.X < SpawnerParams.MinDistanceBehindNonTargetBoat)
			{
				UE_LOG(CBLog, Warning, TEXT("Monster too close to a boat, skipping spawn!"));
				CloseActor = Boat;
				return true;	
			}
		}
	}

	return false;
}

bool UCBMonsterSpawnerAction::AttemptAdjustSpawnLocationForBoat(FVector& Location, const AActor* CloseActor, float AlongSplineDistanceToActor, const USplineComponent* Path) const
{
	const float DistanceToCloseActor = (CloseActor->GetActorLocation() - Location).Length();
	const float DistanceOnSpline = AlongSplineDistanceToActor - DistanceToCloseActor;
	const FVector SplineLocation = Path->GetWorldLocationAtDistanceAlongSpline(DistanceOnSpline);
	
	if ((SplineLocation - Location).Length() < SpawnerParams.MinMaxDistanceToBoat.X)
	{
		UE_LOG(CBLog, Warning, TEXT("Couldn't adjust location due to min distance to boat!"));
		return false;
	}
	
	if (DistanceOnSpline - Path->GetSplineLength() < SpawnerParams.MinDistanceFromFinish)
	{
		UE_LOG(CBLog, Warning, TEXT("Couldn't adjust location due to min distance from start and finish!"));
		return false;
	}

	// Location adjusted successfully
	Location = SplineLocation;
	return true;
}
