// Made By Cubic Burrito


#include "Actor/CBPathActor.h"

#include "Components/SplineComponent.h"
#include "Interface/CBMonsterInterface.h"
#include "Interface/CBPathingActor.h"

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
	PathingActorsOnPath.AddUnique(Boat);
	Cast<ICBPathingActor>(Boat)->PathingActorLeftPathEvent().AddDynamic(this, &ThisClass::OnPathingActorLeftPath);
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
	if ((PathingActorsOnPath.Last()->GetActorLocation() - Start).Length() > Distance)
	{
		return true;
	}

	return false;
}

void ACBPathActor::OnMonsterDead(AActor* Actor)
{
	MonstersOnPath = FMath::Clamp(MonstersOnPath - 1, 0, MonstersOnPath);
}

void ACBPathActor::OnPathingActorLeftPath(AActor* PathingActor)
{
	PathingActorsOnPath.Remove(PathingActor);
}
