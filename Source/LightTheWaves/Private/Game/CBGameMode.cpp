// Made By Cubic Burrito


#include "Game/CBGameMode.h"

#include "Components/SplineComponent.h"
#include "Kismet/GameplayStatics.h"

USplineComponent* ACBGameMode::GetPath_Implementation()
{
	if (SplinePaths.Num() > 0)
	{
		return SplinePaths[FMath::RandRange(0, SplinePaths.Num() - 1)];
	}

	return nullptr;
}

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
}

AActor* ACBGameMode::SpawnBoat()
{
	// @TODO: Implement this
	return nullptr;
}
