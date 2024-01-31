// Made By Cubic Burrito


#include "Actor/CBBoat.h"

#include "Components/SphereComponent.h"
#include "Components/SplineComponent.h"
#include "LightTheWaves/LightTheWaves.h"

// Sets default values
ACBBoat::ACBBoat()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereTrigger = CreateDefaultSubobject<USphereComponent>(FName("SphereTrigger"));
	BoatMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("BoatMesh"));

	SetRootComponent(SphereTrigger);
	BoatMesh->SetupAttachment(GetRootComponent());
}

// Called every frame
void ACBBoat::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsFollowingLight())
	{
		FollowLight(DeltaTime);
		return;
	}
	if (IsFollowingPath())
	{
		FollowPath(DeltaTime);
		return;
	}
	if (IsReturningToPath()) // @NOTE: This might be able to be integrated in follow path?
	{
		ReturnToPath(DeltaTime);
	}
}

void ACBBoat::FollowLight(float DeltaTime)
{
	if (!FollowTarget)
	{
		UE_LOG(CBLog, Error, TEXT("Light is in follow light state, but does not have a FollowTarget! THIS IS A BUG!"))
		CurrentPathingState = EvaluateStatePostFollowLight();
		return;
	}

	// @NOTE: This might have to be improved, by adding collision detection
	const FVector Direction = (FollowTarget->GetComponentLocation() - GetActorLocation()).GetSafeNormal();
	AddActorWorldOffset(Direction * MovementSpeed);
}

void ACBBoat::FollowPath(float DeltaTime)
{
	if (!CurrentPath)
	{
		// @TODO: Find a new path and set it
		
	}

	const FVector LocationOnSpline = CurrentPath->FindLocationClosestToWorldLocation(GetActorLocation(), ESplineCoordinateSpace::World);
	const FVector Direction = CurrentPath->FindDirectionClosestToWorldLocation(LocationOnSpline, ESplineCoordinateSpace::World);
	AddActorWorldOffset(Direction * MovementSpeed * DeltaTime);
}

void ACBBoat::ReturnToPath(float DeltaTime)
{
	// @TODO: Generate a path from the boat location to the nearest spline point
	// @HIGH_PRIORITY

	CurrentPathingState = EBoatPathingState::FollowingPath;
}

void ACBBoat::SetPath_Implementation(USplineComponent* NewPath)
{
	CurrentPath = NewPath;
	SetActorLocation(CurrentPath->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World));

	CurrentPathingState = EBoatPathingState::FollowingPath;
}

void ACBBoat::OnLightFocused_Implementation(UPrimitiveComponent* TargetComponent)
{
	if (IsFollowingLight())
	{
		UE_LOG(CBLog, Warning, TEXT("Receiving a secondary light interaction, when boat is already following path."))
		return;
	}
	
	FollowTarget = TargetComponent;
	CurrentPathingState = EBoatPathingState::FollowingLight;
}

void ACBBoat::OnLightFocusEnd_Implementation()
{
	if (IsNotFollowingLight())
	{
		UE_LOG(CBLog, Warning, TEXT("Attempting to stop light focus on boat, when boat is not following a light."))
		return;
	}

	FollowTarget = nullptr;
	CurrentPathingState = EvaluateStatePostFollowLight();
}

#if WITH_EDITOR
void ACBBoat::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName PropertyName = (PropertyChangedEvent.Property) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	if (PropertyName == GET_MEMBER_NAME_CHECKED(ThisClass, PointsReward))
	{
		CurrencyReward = PointsReward * ( 1 / RatioCurrencyToPoints);
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(ThisClass, CurrencyReward))
	{
		PointsReward = CurrencyReward * RatioCurrencyToPoints;
	}
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(ThisClass, RatioCurrencyToPoints))
	{
		if (bUpdateBasedOnPoints)
		{
			CurrencyReward = PointsReward * ( 1 / RatioCurrencyToPoints);
		}
		else
		{
			PointsReward = CurrencyReward * RatioCurrencyToPoints;
		}
	}
}
#endif

EBoatPathingState ACBBoat::EvaluateStatePostFollowLight()
{
	// @TODO: Determine if the light is close enough to the spline to where we can just keep going, instead of returning to it
	return EBoatPathingState::ReturningToPath;
}
