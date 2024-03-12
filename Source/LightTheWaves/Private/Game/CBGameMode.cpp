// Made By Cubic Burrito


#include "Game/CBGameMode.h"

#include "CBBlueprintFunctionLibrary.h"
#include "CBInitialiser.h"
#include "EngineUtils.h"
#include "Actor/CBBoat.h"
#include "Actor/CBMonsterHazard.h"
#include "Async/CBMonsterSpawnerAction.h"
#include "Components/SplineComponent.h"
#include "Interface/CBPath.h"
#include "Interface/CBPlayerInterface.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerState.h"
#include "LightTheWaves/LightTheWaves.h"

#define BOAT_SPAWN_FORMULA(WaveNumber) WaveNumber

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

FOnEntitySpawned& ACBGameMode::OnBoatSpawnedEvent()
{
	return OnBoatSpawned;
}

FOnEntitySpawned& ACBGameMode::OnMonsterSpawnedEvent()
{
	return OnMonsterSpawnedEnt;
}

FOnEntitySpawned& ACBGameMode::OnBossSpawnedEvent()
{
	return OnBossSpawned;
}

FOnGameStarted& ACBGameMode::OnGameStartedEvent()
{
	return OnGameStarted;
}

void ACBGameMode::Init_Implementation(const FInitData& InitData)
{
	Cast<ICBPlayerInterface>(InitData.PlayerState)->OnGameLostEvent().AddDynamic(this, &ThisClass::GameFinished);
}

void ACBGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	UGameplayStatics::GetAllActorsWithTag(this, FName("Path"), PathActors);
	
	UCBInitialiser* Initialiser = UCBBlueprintFunctionLibrary::GetInitialisationSubsystem(this);
	if (!Initialiser)
	{
		UE_LOG(CBLog, Error, TEXT("Initialiser not available at BeginPlay of CBGameMode"));
	}
	else
	{
		Initialiser->RegisterObjectToInitialiser(this);
		Initialiser->RegisterGameMode(this);
	}
}

void ACBGameMode::OnGameFinished_Implementation(const FGameLostData& Data)
{
	// Implemented in blueprints
}

void ACBGameMode::GameFinished(const FGameLostData& Data)
{
	GetWorldTimerManager().ClearTimer(BoatSpawnTimerHandle);
	GetWorldTimerManager().ClearTimer(MonsterSpawnTimerHandle);
	GetWorldTimerManager().ClearTimer(BossSpawnTimerHandle);
	
	for (TActorIterator<ACBBoat> It(GetWorld()); It; ++It)
	{
		ACBBoat* Boat = *It;
		if (Boat)
		{
			Boat->Destroy();
		}
	}

	for (TActorIterator<ACBMonsterHazard> It(GetWorld()); It; ++It)
	{
		// @TODO: Change this to a submerge function potentially
		ACBMonsterHazard* Hazard = *It;
		if (IsValid(Hazard))
		{
			Hazard->Destroy();
		}
	}

	if (IsValid(Boss))
	{
		// @TODO: When boss reiteration happens, make sure that the boss has a going back down animation, which can be played here
		Boss->Destroy();
	}
	
	OnGameFinished(Data);	
}

void ACBGameMode::TestGameplay()
{
	StartNewWave();
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

	OnGameStarted.Broadcast();
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

void ACBGameMode::OnBoatDestroyed(AActor* DestroyedBoat)
{
	if (DestroyedBoat)
	{
		Boats.Remove(DestroyedBoat);
	}
	else
	{
		Boats.RemoveAll([=](const AActor* Actor) { return IsValid(Actor); });
	}
	
	++DestroyedBoats;
	
	if (bNextWaveWaitingForBoats && SpawnedBoats <= DestroyedBoats)
	{
		UE_LOG(CBLog, Log, TEXT("Spawned Boats: %d, Destoyed Boats: %d"), SpawnedBoats, DestroyedBoats);
		bNextWaveWaitingForBoats = false;
		WaveTimer_Elapsed();
	}
}

void ACBGameMode::ProcessMonsterSpawning()
{
	const float MonsterSpawnPeriod = MonsterSpawningSettings.MonsterSpawnPeriod + FMath::FRandRange(MonsterSpawningSettings.MinTimeVariation, MonsterSpawningSettings.MaxTimeVariation);
	GetWorldTimerManager().SetTimer(MonsterSpawnTimerHandle, this, &ThisClass::SpawnMonster, MonsterSpawnPeriod, false);
}

void ACBGameMode::SpawnMonster()
{
	UCBMonsterSpawnerAction* MonsterSpawnerAction = UCBMonsterSpawnerAction::Create(this, Boats, Monsters, PathActors, MonsterSpawningSettings.SpawnParams);
	MonsterSpawnerAction->OnMonsterSpawned.AddDynamic(this, &ThisClass::OnMonsterSpawned);
	MonsterSpawnerAction->Activate();
	ProcessMonsterSpawning();
}

void ACBGameMode::OnMonsterSpawned(AActor* SpawnedMonster)
{
	OnMonsterSpawnedEnt.Broadcast(SpawnedMonster);
	SpawnedMonster->OnDestroyed.AddDynamic(this, &ThisClass::OnMonsterDestroyed);
	Monsters.Add(SpawnedMonster);
}

void ACBGameMode::OnMonsterDestroyed(AActor* DestroyedMonster)
{
	if (DestroyedMonster)
	{
		Monsters.Remove(DestroyedMonster);
		return;
	}
	
	Monsters.RemoveAll([=](const AActor* Actor) { return Actor == nullptr; });
}

void ACBGameMode::ClearDebris()
{
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsWithTag(this, "Debris", OutActors);

	for (const auto Actor : OutActors)
	{
		Actor->Destroy();
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
	else if (!BoatSpawningSettings.bUseCurveForBoatSpawnAmount && BoatSpawningSettings.BoatSpawnPeriods.Num() >= WaveNumber)
	{
		const float Start = BoatSpawningSettings.BoatSpawnPeriods[WaveNumber - 1].X;
		const float End = BoatSpawningSettings.BoatSpawnPeriods[WaveNumber - 1].Y;
		BoatSpawnPeriod = FMath::Lerp(Start, End, GetPercentRoundTimeElapsed());
	}
	else
	{
		const float Start = BoatSpawningSettings.BoatSpawnPeriods.Last().X;
		const float End = BoatSpawningSettings.BoatSpawnPeriods.Last().Y;
		BoatSpawnPeriod = BOAT_SPAWN_FORMULA(FMath::Lerp(Start, End, GetPercentRoundTimeElapsed()));
	}
	
	return BoatSpawnPeriod;
}

void ACBGameMode::SpawnBoat(AActor* PathActor)
{
	FActorSpawnParameters SpawnParameters;
	USplineComponent* Path = ICBPath::Execute_GetPath(PathActor);
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActor* Boat = GetWorld()->SpawnActor<AActor>(GetRandomSpawnableBoat(), Path->GetWorldLocationAtSplinePoint(0), FRotator::ZeroRotator,  SpawnParameters);
	if (Boat)
	{
		Boats.Add(Boat);
		Boat->OnDestroyed.AddDynamic(this, &ThisClass::OnBoatDestroyed);
		ICBPathingActor::Execute_SetPath(Boat, Path);
		ICBPath::Execute_RegisterBoatOnPath(PathActor, Boat);
		OnBoatSpawned.Broadcast(Boat);
		++SpawnedBoats;
	}
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
	float Start = BoatSpawningSettings.MaxBoats.Last().X;
	float End = BoatSpawningSettings.MaxBoats.Last().Y;
	if (BoatSpawningSettings.MaxBoats.Num() >= WaveNumber)
	{
		Start = BoatSpawningSettings.MaxBoats[WaveNumber - 1].X;
		End = BoatSpawningSettings.MaxBoats[WaveNumber - 1].Y;
	}
	const float MaxBoatsAllowed = FMath::Lerp(Start, End, GetPercentRoundTimeElapsed());
	const int32 MaxBoatsAllowedInt = FMath::RoundToInt32(MaxBoatsAllowed * 100.f) * 0.01f;
	
	return Boats.Num() >= MaxBoatsAllowedInt;
}

void ACBGameMode::StartNewWave(EGameActivity PreviousActivity)
{
	++WaveNumber;
	GetWorldTimerManager().SetTimer(WaveTimerHandle, this, &ThisClass::WaveTimer_Elapsed, WaveDuration, false);

	SpawnedBoats = 0;
	DestroyedBoats = 0;
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
	GetWorldTimerManager().ClearTimer(BoatSpawnTimerHandle);
	GetWorldTimerManager().ClearTimer(BossSpawnTimerHandle);
	WaveTimerHandle.Invalidate();

	if (Boss)
	{
		bNextWaveWaitingForBoss = true;
		return;
	}

	if (SpawnedBoats > DestroyedBoats)
	{
		UE_LOG(CBLog, Log, TEXT("Spawned Boats: %d, Destoyed Boats: %d"), SpawnedBoats, DestroyedBoats);
		bNextWaveWaitingForBoats = true;
		return;
	}
	
	GetWorldTimerManager().ClearTimer(MonsterSpawnTimerHandle);
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
	StartNewWave();
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
	const FVector SpawnLocation = FVector(-458.000000,7278.000000,3450.000000);
	Boss = GetWorld()->SpawnActor<AActor>(BossSpawningSettings.BossActorClass, SpawnLocation, FRotator::ZeroRotator, SpawnParameters);

	FActivityStateUpdatedData Data;
	Data.OldActivity = EGameActivity::Wave;
	Data.NewActivity = EGameActivity::Boss;
	Data.ActivityFinishedState = EActivityFinishedState::Ongoing;
	OnActivityStateUpdated.Broadcast(Data);

	OnBossSpawned.Broadcast(Boss);
	Boss->OnDestroyed.AddDynamic(this, &ThisClass::OnBossKilled);
}

void ACBGameMode::OnBossKilled(AActor* DestroyedActor)
{
	UE_LOG(CBLog, Log, TEXT("Boss Defeated!"));
	Boss = nullptr;
	
	if (bNextWaveWaitingForBoss)
	{
		bNextWaveWaitingForBoss = false;
		WaveTimer_Elapsed();
		return;
	}
	
	FActivityStateUpdatedData Data;
	Data.ActivityFinishedState = EActivityFinishedState::Successful;
	Data.OldActivity = EGameActivity::Boss;
	Data.NewActivity = EGameActivity::Wave;
	OnActivityStateUpdated.Broadcast(Data);
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

TSubclassOf<AActor> ACBGameMode::GetRandomSpawnableBoat()
{
	// @TODO: Expand this function to be able to evaluate randomization weights, etc...
	return BoatSpawningSettings.SpawnableBoatClasses[FMath::RandRange(0, BoatSpawningSettings.SpawnableBoatClasses.Num() - 1)];
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
