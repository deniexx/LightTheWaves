// Made By Cubic Burrito


#include "Actor/CBPathActor.h"

#include "Components/SplineComponent.h"
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
	PathingActorsOnPath = 0;
}

USplineComponent* ACBPathActor::GetPath_Implementation()
{
	return SplineComponent;
}

int32 ACBPathActor::GetNumberOfBoatsOnPath_Implementation()
{
	return PathingActorsOnPath;
}

void ACBPathActor::RegisterBoatOnPath_Implementation(AActor* Boat)
{
	// @NOTE: This might need to be expanded
	PathingActorsOnPath = FMath::Clamp(PathingActorsOnPath - 1, 0, PathingActorsOnPath);
	Cast<ICBPathingActor>(Boat)->PathingActorLeftPathEvent().AddDynamic(this, &ThisClass::OnPathingActorLeftPath);
}

void ACBPathActor::OnPathingActorLeftPath(AActor* PathingActor)
{
	--PathingActorsOnPath;
}
