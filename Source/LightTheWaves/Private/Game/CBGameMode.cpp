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

static TAutoConsoleVariable<int32> CVarDrawDebugMonsterSpawn(
	TEXT("ShowDebugMonsterSpawn"),
	0,
	TEXT("Draws debug info about Monster Spawning")
	TEXT(" 0: Do not show debug info/n")
	TEXT(" 1: Show Debug info/n"),
	ECVF_Cheat
);

FOnActivityStateUpdated& ACBGameMode::OnActivityStateUpdatedEvent()
{
	return OnActivityStateUpdated;
}

void ACBGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	UGameplayStatics::GetAllActorsWithTag(this, FName("Path"), PathActors);
}

USplineComponent* ACBGameMode::GetRandomPath_Implementation()
{
	USplineComponent* Spline = nullptr;
	GetRandomSpline(Spline);
	return Spline;
}

USplineComponent* ACBGameMode::GetClosestPath_Implementation(AActor* ReferenceActor)
{
	if (!ReferenceActor)
	{
		UE_LOG(CBLog, Error, TEXT("Attempting to find closest path for a nullptr ReferenceActor!"))
		return nullptr;
	}

	USplineComponent* Spline = nullptr;
	GetSplineClosestToLocation(ReferenceActor->GetActorLocation(), Spline);
	return Spline;
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

void ACBGameMode::StartWaveGameplay_Implementation()
{
	if (WaveTimerHandle.IsValid())
	{
		return;
	}
	
	StartNewWave();
}

void ACBGameMode::ProcessBoatSpawning()
{
	const float BoatSpawnPeriod = GetBoatSpawnPeriod() + FMath::FRandRange(BoatSpawningSettings.MinTimeVariation, BoatSpawningSettings.MaxTimeVariation);
	GetWorldTimerManager().SetTimer(BoatSpawnTimerHandle, this, &ThisClass::SpawnBoat_TimerElapsed, BoatSpawnPeriod, false);
}

void ACBGameMode::SpawnBoat_TimerElapsed()
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
		MonsterSpawnPeriod = MonsterSpawningSettings.MonsterSpawnCurve.Eval(GetPercentRoundTimeElapsed(), "");
	}
	else if (!MonsterSpawningSettings.bUseCurveForMonsterSpawnPeriod && MonsterSpawningSettings.MonsterSpawnPeriods.Num() >= WaveNumber)
	{
		MonsterSpawnPeriod = MonsterSpawningSettings.MonsterSpawnPeriods[WaveNumber - 1];
	}
	else
	{
		MonsterSpawnPeriod = MONSTER_SPAWN_FORMULA(WaveNumber);
	}

	MonsterSpawnPeriod += + FMath::FRandRange(MonsterSpawningSettings.MinTimeVariation, MonsterSpawningSettings.MaxTimeVariation);
	GetWorldTimerManager().SetTimer(MonsterSpawnTimerHandle, this, &ThisClass::RunMonsterSpawnEQS, MonsterSpawnPeriod, false);
}

void ACBGameMode::RunMonsterSpawnEQS()
{
	UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(this, MonsterSpawnQuery, this, EEnvQueryRunMode::RandomBest25Pct, nullptr);

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

	const bool bShowDebug = CVarDrawDebugMonsterSpawn.GetValueOnAnyThread() > 0;
	const TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();
	if (Locations.Num() > 0)
	{
		const FVector RandomLocation = Locations[FMath::RandRange(0, Locations.Num() - 1)];
		USplineComponent* ClosestSpline;
		AActor* SplineActor = GetSplineClosestToLocation(RandomLocation, ClosestSpline);
		FVector SpawnLocation = ClosestSpline->FindLocationClosestToWorldLocation(RandomLocation, ESplineCoordinateSpace::World);
		const float XOffset = FMath::FRandRange(-MonsterSpawningSettings.MaxRadiusForLocationOffset, MonsterSpawningSettings.MaxRadiusForLocationOffset);
		const float YOffset = FMath::FRandRange(-MonsterSpawningSettings.MaxRadiusForLocationOffset, MonsterSpawningSettings.MaxRadiusForLocationOffset);
		SpawnLocation += FVector(XOffset, YOffset, 0.f);

		if (bShowDebug)
		{
			DrawDebugString(GetWorld(), RandomLocation, TEXT("Random Location"), nullptr, FColor::White, 1.5f);
			DrawDebugSphere(GetWorld(), RandomLocation, 32, 12, FColor::Red, true, 1.5f);
			DrawDebugString(GetWorld(), RandomLocation, TEXT("Spline Corrected Location"), nullptr, FColor::White, 1.5f);
			DrawDebugSphere(GetWorld(), SpawnLocation, 32, 12, FColor::Blue, true, 1.5f);
		}
		
		const float DistanceToStart = (SpawnLocation - ClosestSpline->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World)).Length();
		if (DistanceToStart < MonsterSpawningSettings.MinimumDistanceFromPathStart)
		{
			UE_LOG(CBLog, Log, TEXT("Location found too close to start, skipping spawning."));
			ProcessMonsterSpawning();
			return;
		}
		

		const float MonstersOnChosenPath = ICBPath::Execute_GetNumberOfMonstersOnPath(SplineActor);
		if (MonstersOnChosenPath >= GetMaxAllowedMonstersOnPath(SplineActor))
		{
			UE_LOG(CBLog, Log, TEXT("Too many monsters on path, skipping spawning."));
			ProcessMonsterSpawning();
			return;
		}

		// @NOTE: Might be worth to check if the path has a current boat(if prediction is added a future boat incoming)
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		AActor* NewMonster = GetWorld()->SpawnActor<AActor>(MonsterSpawningSettings.MonsterClass, SpawnLocation, FRotator::ZeroRotator, SpawnParameters);
		ICBPath::Execute_AddMonsterToPath(SplineActor, NewMonster);
		ProcessMonsterSpawning();
	}
}

float ACBGameMode::GetBoatSpawnPeriod()
{
	float BoatSpawnPeriod;
	if (BoatSpawningSettings.bUseCurveForBoatSpawnAmount && WaveNumberToSwitchToFormula >= WaveNumber)
	{
		const float Start = BoatSpawningSettings.StartOfRoundBoatSpawnCurve.Eval(WaveNumber, "");
		const float End = BoatSpawningSettings.EndOfRoundBoatSpawnCurve.Eval(WaveNumber, "");
		BoatSpawnPeriod = FMath::Lerp(Start, End, GetPercentRoundTimeElapsed());
	}
	else if (!BoatSpawningSettings.bUseCurveForBoatSpawnAmount && BoatSpawningSettings.MaxBoatSpawns.Num() >= WaveNumber)
	{
		const float Start = BoatSpawningSettings.MaxBoatSpawns[WaveNumber - 1].X;
		const float End = BoatSpawningSettings.MaxBoatSpawns[WaveNumber - 1].Y;
		BoatSpawnPeriod = FMath::Lerp(Start, End, GetPercentRoundTimeElapsed());
	}
	else
	{
		const float Start = BoatSpawningSettings.MaxBoatSpawns.Last().X;
		const float End = BoatSpawningSettings.MaxBoatSpawns.Last().Y;
		BoatSpawnPeriod = BOAT_SPAWN_FORMULA(FMath::Lerp(Start, End, GetPercentRoundTimeElapsed()));
	}
	
	return BoatSpawnPeriod;
}

void ACBGameMode::SpawnBoat(AActor* PathActor)
{
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActor* Boat = GetWorld()->SpawnActor<AActor>(GetRandomSpawnableBoat(), SpawnParameters);
	ICBPathingActor::Execute_SetPath(Boat, ICBPath::Execute_GetPath(PathActor));
	ICBPath::Execute_RegisterBoatOnPath(PathActor, Boat);
}

bool ACBGameMode::TrySpawnBoat()
{
	float MaxBoatsPerPath;
	if (BoatSpawningSettings.bUseCurveForMaxBoatsPerPath && WaveNumberToSwitchToFormula >= WaveNumber)
	{
		const float Start = BoatSpawningSettings.StartOfRoundMaxBoatsPerPathCurve.Eval(WaveNumber, "");
		const float End = BoatSpawningSettings.EndOfRoundMaxBoatsPerPathCurve.Eval(WaveNumber, "");
		MaxBoatsPerPath = FMath::Lerp(Start, End, GetPercentRoundTimeElapsed());
	}
	else if (!BoatSpawningSettings.bUseCurveForMaxBoatsPerPath && BoatSpawningSettings.MaxBoatsPerPath.Num() >= WaveNumber)
	{
		const float Start = BoatSpawningSettings.MaxBoatsPerPath[WaveNumber - 1].X;
		const float End = BoatSpawningSettings.MaxBoatsPerPath[WaveNumber - 1].Y;
		MaxBoatsPerPath = FMath::Lerp(Start, End, GetPercentRoundTimeElapsed());
	}
	else
	{
		const float Start = BoatSpawningSettings.MaxBoatsPerPath.Last().X;
		const float End = BoatSpawningSettings.MaxBoatsPerPath.Last().Y;
		MaxBoatsPerPath = BOAT_SPAWN_FORMULA(FMath::Lerp(Start, End, GetPercentRoundTimeElapsed()));
	}

	AActor* FreePathActor = nullptr;

	if (MaxBoatsPerPath <= 0)
	{
		USplineComponent* Spline;
		SpawnBoat(GetRandomSpline(Spline));
		return true;
	}
	
	if (IsAnyPathFree(MaxBoatsPerPath, FreePathActor))
	{
		SpawnBoat(FreePathActor);
		return true;
	}
	
	return false;
}

bool ACBGameMode::IsAtMaxBoats()
{
	int32 Boats = 0;

	for (AActor* Actor : PathActors)
	{
		Boats += ICBPath::Execute_GetNumberOfBoatsOnPath(Actor);
	}

	int32 MaxBoatsAllowed = BoatSpawningSettings.MaxBoats.Last();
	if (BoatSpawningSettings.MaxBoats.Num() >= WaveNumber)
	{
		MaxBoatsAllowed = BoatSpawningSettings.MaxBoats[WaveNumber - 1];
	}
	
	return Boats > MaxBoatsAllowed;
}

void ACBGameMode::StartNewWave(EGameActivity PreviousActivity)
{
	//++WaveNumber;
	WaveNumber = 1;
	GetWorldTimerManager().SetTimer(WaveTimerHandle, this, &ThisClass::WaveTimer_Elapsed, WaveDuration, false);

	FActivityStateUpdatedData Data;
	Data.OldActivity = PreviousActivity;
	Data.NewActivity = EGameActivity::Wave;
	Data.ActivityFinishedState = EActivityFinishedState::Successful;
	OnActivityStateUpdated.Broadcast(Data);
	
	ProcessBoatSpawning();
	ProcessMonsterSpawning();
	ProcessBossSpawning();
}

void ACBGameMode::WaveTimer_Elapsed()
{
	/* @TODO: Track if the player is still in combat with the boss, do not start the recess period until the fight is over
	 * @TODO: or "kill" the boss ourselves
	 */
	
	GetWorldTimerManager().ClearTimer(BoatSpawnTimerHandle);
	GetWorldTimerManager().ClearTimer(MonsterSpawnTimerHandle);
	GetWorldTimerManager().ClearTimer(BossSpawnTimerHandle);
	WaveTimerHandle.Invalidate();

	FActivityStateUpdatedData Data;
	Data.OldActivity = EGameActivity::Wave;
	Data.NewActivity = EGameActivity::Recess;
	Data.ActivityFinishedState = EActivityFinishedState::Successful;
	OnActivityStateUpdated.Broadcast(Data);
	
	GetWorldTimerManager().SetTimer(RecessTimerHandle, this, &ThisClass::RecessTimer_Elapsed, RecessPeriodDuration, false);
}

void ACBGameMode::RecessTimer_Elapsed()
{
	RecessTimerHandle.Invalidate();
	//StartNewWave();
	
	// @TODO: Add a finish screen for the demo
}

void ACBGameMode::ProcessBossSpawning()
{
	const float BossSpawnDelay = FMath::FRandRange(BossSpawningSettings.BossSpawnTimePeriodMin, BossSpawningSettings.BossSpawnTimePeriodMax);
	GetWorldTimerManager().SetTimer(BossSpawnTimerHandle, this, &ThisClass::SpawnBoss, BossSpawnDelay, false);
}

void ACBGameMode::SpawnBoss()
{
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	// @TODO: Figure out a way to know where the boss should spawn, probably a variable on the spawn settings
	const FVector SpawnLocation = FVector(-304.0, 4400.0, 2200.0);
	AActor* BossActor = GetWorld()->SpawnActor<AActor>(BossSpawningSettings.BossActorClass, SpawnLocation, FRotator::ZeroRotator, SpawnParameters);

	FActivityStateUpdatedData Data;
	Data.OldActivity = EGameActivity::Wave;
	Data.NewActivity = EGameActivity::Boss;
	Data.ActivityFinishedState = EActivityFinishedState::Ongoing;
	OnActivityStateUpdated.Broadcast(Data);

	BossActor->OnDestroyed.AddDynamic(this, &ThisClass::OnBossKilled);
}

void ACBGameMode::OnBossKilled(AActor* DestroyedActor)
{
	// @TODO: Advance game and whatever else needs to be done
	FActivityStateUpdatedData Data;
	Data.ActivityFinishedState = EActivityFinishedState::Successful;
	Data.OldActivity = EGameActivity::Boss;
	Data.NewActivity = EGameActivity::Wave;
	OnActivityStateUpdated.Broadcast(Data);

	UE_LOG(CBLog, Log, TEXT("Boss Defeated!"));
}

int32 ACBGameMode::GetMaxAllowedMonstersOnPath(AActor* Path)
{
	if (MonsterSpawningSettings.bUseBoatsAsForMaxMonsterOnPath)
	{
		return ICBPath::Execute_GetNumberOfBoatsOnPath(Path) + MonsterSpawningSettings.AdditionalMaxMonstersPerPath;
	}

	if (MonsterSpawningSettings.bUseCurveForMaxMonstersOnPath)
	{
		return MonsterSpawningSettings.MaxMonstersOnPathCurve.Eval(GetPercentRoundTimeElapsed(), "");
	}

	if (WaveNumber >= MonsterSpawningSettings.MaxMonstersOnPath.Num())
	{
		MonsterSpawningSettings.MaxMonstersOnPath.Last();
	}

	return MonsterSpawningSettings.MaxMonstersOnPath[WaveNumber - 1];
}

float ACBGameMode::GetRoundTimeElapsed() const
{
	if (WaveTimerHandle.IsValid())
	{
		return GetWorldTimerManager().GetTimerElapsed(WaveTimerHandle);	
	}

	return 0.f;
}

float ACBGameMode::GetPercentRoundTimeElapsed() const
{
	if (WaveTimerHandle.IsValid())
	{
		return GetRoundTimeElapsed() / WaveDuration;
	}

	return 0.f;
}

bool ACBGameMode::IsAnyPathFree(int32 MaxBoatsPerPath, AActor*& OutPath)
{
	TArray<AActor*> FreePaths;

	if (IsAtMaxBoats())
	{
		return false;
	}
	
	for (AActor* Actor : PathActors)
	{
		if (ICBPath::Execute_GetNumberOfBoatsOnPath(Actor) < MaxBoatsPerPath)
		{
			const float DistanceToCheck = BoatSpawningSettings.MinimumDistanceFromPathStart +
				FMath::FRandRange(BoatSpawningSettings.MinDistanceVariation, BoatSpawningSettings.MaxDistanceVariation);
			if (ICBPath::Execute_IsBoatWithinDistanceFromStart(Actor, DistanceToCheck))
			{
				FreePaths.Add(Actor);
			}
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

AActor* ACBGameMode::GetRandomSpline(USplineComponent*& OutSplineComponent)
{
	if (PathActors.Num() > 0)
	{
		AActor* PathActor = PathActors[FMath::RandRange(0, PathActors.Num() - 1)];
		OutSplineComponent = ICBPath::Execute_GetPath(PathActor);
		return PathActor;
	}

	UE_LOG(CBLog, Error, TEXT("No Spline Paths available! Could be an issue, or the function was called too early."))
	return nullptr;
}

AActor* ACBGameMode::GetSplineClosestToLocation(const FVector& Location, USplineComponent*& OutSplineComponent)
{
	if (PathActors.Num() > 0)
	{
		AActor* ClosestActor = PathActors[0];
		OutSplineComponent = ICBPath::Execute_GetPath(PathActors[0]);
		float ClosestDistance = TNumericLimits<float>::Max();

		for (AActor* Actor : PathActors)
		{
			USplineComponent* Spline = ICBPath::Execute_GetPath(Actor);
			const float CurrentDistance = (Spline->FindLocationClosestToWorldLocation(Location, ESplineCoordinateSpace::World) - Location).Length();
			if (CurrentDistance < ClosestDistance)
			{
				ClosestActor = Actor;
				ClosestDistance = CurrentDistance;
				OutSplineComponent = Spline;
			}
		}
		
		return ClosestActor;
	}

	UE_LOG(CBLog, Error, TEXT("No Spline Paths available! Could be an issue, or the function was called too early."))
	return nullptr;
}
