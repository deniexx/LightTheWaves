// Made By Cubic Burrito


#include "Game/CBGameMode.h"

#include "Actor/CBBoat.h"
#include "Components/SplineComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LightTheWaves/LightTheWaves.h"

void ACBGameMode::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> SplineActors;
	UGameplayStatics::GetAllActorsWithTag(this, FName("Path"), SplineActors);

	for (const AActor* Actor : SplineActors)
	{
		if (USplineComponent* SplineComponent = Actor->GetComponentByClass<USplineComponent>())
		{
			SplinePaths.Add(SplineComponent);
		}
	}

	ProcessBoatSpawning();
}

USplineComponent* ACBGameMode::GetRandomPath_Implementation()
{
	if (SplinePaths.Num() > 0)
	{
		return SplinePaths[FMath::RandRange(0, SplinePaths.Num() - 1)];
	}

	return nullptr;
}

USplineComponent* ACBGameMode::GetClosestPath_Implementation(AActor* ReferenceActor)
{
	if (!ReferenceActor)
	{
		UE_LOG(CBLog, Error, TEXT("Attempting to find closest path for a nullptr ReferenceActor!"))
		return nullptr;
	}
	
	if (SplinePaths.Num() > 0)
	{
		const FVector RefActorLocation = ReferenceActor->GetActorLocation();
		USplineComponent* Closest = SplinePaths[0];
		float ClosestDistance = TNumericLimits<float>::Max();

		for (USplineComponent* Spline : SplinePaths)
		{
			const float CurrentDistance = (Spline->FindLocationClosestToWorldLocation(RefActorLocation, ESplineCoordinateSpace::World) - RefActorLocation).Length();
			if (CurrentDistance < ClosestDistance)
			{
				ClosestDistance = CurrentDistance;
				Closest = Spline;
			}
		}
		
		return Closest;
	}
	
	return nullptr;
}

void ACBGameMode::ProcessBoatSpawning()
{
	if (BoatSpawningMode == EBoatSpawningMode::None)
	{
		return;
	}
	
	float BoatSpawnPeriod = BoatSpawnCurve->GetFloatValue(1);
	
	if (BoatSpawningMode == EBoatSpawningMode::MaxPerWave)
	{
		// @NOTE: This will need to be improved
		BoatSpawnPeriod = BoatSpawnCurve->GetFloatValue(1) / 5.f;
	}
	
	GetWorldTimerManager().SetTimer(BoatSpawnTimerHandle, this, &ThisClass::SpawnBoat, BoatSpawnPeriod, false);
}

void ACBGameMode::SpawnBoat()
{
	if (SpawnBoatClasses.Num() <= 0)
	{
		checkf(false, TEXT("Spawn Boat Classes in Game Mode is not filled in!"));
	}
	
	AActor* Boat = GetWorld()->SpawnActor<AActor>(SpawnBoatClasses[FMath::RandRange(0, SpawnBoatClasses.Num() - 1)], FActorSpawnParameters());
	ICBPathingActor::Execute_SetPath(Boat, GetRandomPath());

	ProcessBoatSpawning();
}

