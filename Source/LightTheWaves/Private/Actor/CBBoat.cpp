// Made By Cubic Burrito


#include "Actor/CBBoat.h"

#include "Components/SphereComponent.h"
#include "Components/SplineComponent.h"
#include "GameFramework/GameModeBase.h"
#include "Interface/CBPathProvider.h"
#include "LightTheWaves/LightTheWaves.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"

static TAutoConsoleVariable<int32> CVarDrawDebugBoatPathing(
	TEXT("ShowDebugBoatPathing"),
	0,
	TEXT("Draws debug info about Boat Pathing")
	TEXT(" 0: Do not show debug info/n")
	TEXT(" 1: Show Debug info/n"),
	ECVF_Cheat
);

// Sets default values
ACBBoat::ACBBoat()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereTrigger = CreateDefaultSubobject<USphereComponent>(FName("SphereTrigger"));
	BoatMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("BoatMesh"));
	DebrisSpawnLocation = CreateDefaultSubobject<USceneComponent>(FName("DebrisSpawnLocation"));
	
	SetRootComponent(SphereTrigger);
	BoatMesh->SetupAttachment(GetRootComponent());
	DebrisSpawnLocation->SetupAttachment(GetRootComponent());

	CurrentPathingState = EBoatPathingState::ReturningToPath;
}

// Called every frame
void ACBBoat::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (IsFollowingLight())
	{
		FollowLight(DeltaTime);
	}
	else if (IsFollowingPath())
	{
		FollowPath(DeltaTime);
	}
	else if (IsReturningToPath())
	{
		ReturnToPath(DeltaTime);
	}

	OrientRotationToMovement(DeltaTime);
	LastFrameLocation = GetActorLocation();
	const bool bDrawDebug = CVarDrawDebugBoatPathing.GetValueOnAnyThread() > 0;
	if (bDrawDebug)
	{
		DrawBoatDebugPathing();
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
	
	MovementDirection = (FollowTarget->GetComponentLocation() - GetActorLocation()).GetSafeNormal();
	MovementDirection.Z = 0;
	AddActorWorldOffset(MovementDirection * MovementSpeed * DeltaTime);
}

void ACBBoat::FollowPath(float DeltaTime)
{
	if (!CurrentPath)
	{
		UE_LOG(CBLog, Warning, TEXT("Boat [%s] has no current path, it is either waiting to receive one, or something is wrong."), *GetNameSafe(this));
		return;
	}

	const FVector LocationOnSpline = CurrentPath->FindLocationClosestToWorldLocation(GetActorLocation(), ESplineCoordinateSpace::World);
	MovementDirection = CurrentPath->FindDirectionClosestToWorldLocation(LocationOnSpline, ESplineCoordinateSpace::World);
	MovementDirection.Z = 0;
	AddActorWorldOffset(MovementDirection * MovementSpeed * DeltaTime);
}

void ACBBoat::ReturnToPath(float DeltaTime)
{
	if (ReturnToPathPoints.Num() <= 0)
	{
		const FVector DirectionOnSpline = CurrentPath->FindDirectionClosestToWorldLocation(GetActorLocation(), ESplineCoordinateSpace::World);
		const bool bDrawDebug = CVarDrawDebugBoatPathing.GetValueOnAnyThread() > 0;
		const FVector ForwardLookLocation = GetActorLocation() + (DirectionOnSpline * ReturnToPathForwardLook);
		FVector ClosestSplinePoint = CurrentPath->FindLocationClosestToWorldLocation(ForwardLookLocation, ESplineCoordinateSpace::World);
		ClosestSplinePoint.Z = GetActorLocation().Z;

		if (bDrawDebug)
		{
			DrawDebugSphere(GetWorld(), ForwardLookLocation, 8, 8, FColor::Yellow, false, 5);
			DrawDebugSphere(GetWorld(), ClosestSplinePoint, 8, 8, FColor::Yellow, false, 5);
		}
		if (UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, GetActorLocation(), ClosestSplinePoint))
		{
			ReturnToPathPoints = MoveTemp(NavPath->PathPoints);
			PointIndex = 0;
		}
	}

	if (ReturnToPathPoints.Num() <= 0)
	{
		// Try again to generate a path
		return;
	}

	// Using this to adjust for height
	const FVector Point = FVector(ReturnToPathPoints[PointIndex].X, ReturnToPathPoints[PointIndex].Y, GetActorLocation().Z);
	if ((GetActorLocation() - Point).Length() < 5.f)
	{
		++PointIndex;
	}
	
	// Have we reached the last point?
	if (PointIndex == ReturnToPathPoints.Num())
	{
		CurrentPathingState = EvaluateStatePostFollowLight();
		ReturnToPathPoints.Empty();
		return;
	}

	FVector Target =  ReturnToPathPoints[PointIndex];
	Target.Z = GetActorLocation().Z;
	MovementDirection = (Target - GetActorLocation()).GetSafeNormal();
	AddActorWorldOffset(MovementDirection * MovementSpeed * DeltaTime);
}

void ACBBoat::OrientRotationToMovement(float DeltaTime)
{
	if (MovementDirection.IsNearlyZero())
	{
		return;
	}

	const FRotator CurrentRotation = GetActorRotation();
	const FRotator TargetRotation = MovementDirection.ToOrientationRotator();
	const FRotator InterpolatedRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, RotationRate);
	SetActorRotation(InterpolatedRotation);
}

void ACBBoat::LeaveDebris(const FVector& Location)
{
	// @NOTE: Potentially randomize the rotation of the debris on an axis
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Instigator = this;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	GetWorld()->SpawnActor<AActor>(DebrisLeftAfterDestruction, DebrisSpawnLocation->GetComponentLocation(), GetActorRotation(), SpawnParameters);
}

void ACBBoat::DrawBoatDebugPathing()
{
	DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + (GetActorForwardVector() * 100.f), 1, FColor::Red, false, -1, 0, 3);
	
	for (const FVector& Point : ReturnToPathPoints)
	{
		DrawDebugSphere(GetWorld(), Point, 8, 8, FColor::Green);
	}
}

void ACBBoat::SetPath_Implementation(USplineComponent* NewPath)
{
	CurrentPath = NewPath;
	SetActorLocation(CurrentPath->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World));
	LastFrameLocation = GetActorLocation();
	
	CurrentPathingState = EBoatPathingState::FollowingPath;
}

FOnPathingActorLeftPath& ACBBoat::PathingActorLeftPathEvent()
{
	return OnPathingActorLeftPath;
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

	ReturnToPathPoints.Empty();
	FollowTarget = nullptr;
	CurrentPathingState = EvaluateStatePostFollowLight();
}

void ACBBoat::OnDestroyed_Implementation(AActor* InstigatorActor, EDestroyingObject DestroyingObject)
{
	if (DestroyingObject != EDestroyingObject::Debris)
	{
		LeaveDebris(GetActorLocation());
	}

	OnPathingActorLeftPath.Broadcast(this);
	Destroy();
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
	USplineComponent* SplineComponent = ICBPathProvider::Execute_GetClosestPath(GetWorld()->GetAuthGameMode(), this);
	if (SplineComponent != CurrentPath)
	{
		OnPathingActorLeftPath.Broadcast(this);
		CurrentPath = SplineComponent;
		ICBPathProvider::Execute_RegisterPathingActorWithPath(GetWorld()->GetAuthGameMode(), this, SplineComponent);
	}

	const FVector ClosestSplinePoint = CurrentPath->FindLocationClosestToWorldLocation(GetActorLocation(), ESplineCoordinateSpace::World);
	const float DistanceToClosestPathPoint = (ClosestSplinePoint - GetActorLocation()).Length();
	if (DistanceToClosestPathPoint > MaxDistanceToPathAllowed)
	{
		return EBoatPathingState::ReturningToPath;
	}
	
	return EBoatPathingState::FollowingPath;
}
