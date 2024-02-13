// Made By Cubic Burrito


#include "Actor/CBPathActor.h"

#include "Components/SplineComponent.h"
#include "Interface/CBMonsterInterface.h"
#include "Interface/CBPathingActor.h"
#include "LightTheWaves/LightTheWaves.h"

// Sets default values
ACBPathActor::ACBPathActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SplineComponent = CreateDefaultSubobject<USplineComponent>(FName("Path"));
	SetRootComponent(SplineComponent);
	Tags.Add("Path");
}

// Called when the game starts or when spawned
void ACBPathActor::BeginPlay()
{
	Super::BeginPlay();

	MonstersOnPath = 0;
}

USplineComponent* ACBPathActor::GetPath_Implementation()
{
	return SplineComponent;
}

int32 ACBPathActor::GetNumberOfBoatsOnPath_Implementation()
{
	return PathingActorsOnPath.Num();
}

void ACBPathActor::RegisterBoatOnPath_Implementation(AActor* Boat)
{
	if (Boat)
	{
		PathingActorsOnPath.AddUnique(Boat);
		Cast<ICBPathingActor>(Boat)->PathingActorLeftPathEvent().AddDynamic(this, &ThisClass::OnPathingActorLeftPath);
	}
}

int32 ACBPathActor::GetNumberOfMonstersOnPath_Implementation()
{
	return MonstersOnPath;
}

void ACBPathActor::AddMonsterToPath_Implementation(AActor* Monster)
{
	if (!Monster)
	{
		return;
	}
	
	++MonstersOnPath;
	Cast<ICBMonsterInterface>(Monster)->OnMonsterDeadEvent().AddDynamic(this, &ThisClass::OnMonsterDead);
}

bool ACBPathActor::IsBoatWithinDistanceFromStart_Implementation(float Distance)
{
	if (PathingActorsOnPath.Num() == 0)
	{
		return true;
	}
	
	const FVector Start = SplineComponent->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);

	if (PathingActorsOnPath.Last())
	{
		if ((PathingActorsOnPath.Last()->GetActorLocation() - Start).Length() > Distance)
		{
			return true;
		}
	}
	else
	{
		// A bit of a hack, but this ensures that we do not crash and we still check for a boat being too close to the start without going through the whole array
		for (int i = PathingActorsOnPath.Num() - 1; i > 0; --i)
		{
			if (PathingActorsOnPath[i])
			{
				if ((PathingActorsOnPath[i]->GetActorLocation() - Start).Length() > Distance)
				{
					return true;
				}
				break;
			}
		}
	}
	
	return false;
}

void ACBPathActor::OnMonsterDead(AActor* Actor)
{
	MonstersOnPath = FMath::Clamp(MonstersOnPath - 1, 0, MonstersOnPath);
}

void ACBPathActor::OnPathingActorLeftPath(AActor* PathingActor)
{
	if (!PathingActor)
	{
		CleanArray();
	}
	else
	{
		PathingActorsOnPath.Remove(PathingActor);
	}
}

void ACBPathActor::CleanArray()
{
	for (int i = PathingActorsOnPath.Num(); i > 0; --i)
	{
		const int AmountRemoved = PathingActorsOnPath.RemoveAll([=](const AActor* Actor) { return Actor == nullptr; });
		if (AmountRemoved > 0)
		{
			UE_LOG(CBLog, Log, TEXT("Cleaned up %d nullptrs from PathActor"), AmountRemoved);
		}
	}
}
