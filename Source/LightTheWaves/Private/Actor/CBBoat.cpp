// Made By Cubic Burrito


#include "Actor/CBBoat.h"

#include "Components/SphereComponent.h"
#include "Components/SplineComponent.h"
#include "GameFramework/GameModeBase.h"
#include "Interface/CBPathProvider.h"
#include "LightTheWaves/LightTheWaves.h"
#include "NavigationSystem.h"
#include "Async/CBMoveActorToAction.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "GameFramework/PlayerState.h"
#include "Interface/CBPlayerInterface.h"
#include "Kismet/GameplayStatics.h"

static TAutoConsoleVariable<int32> CVarDrawDebugBoatPathing(
	TEXT("ShowDebugBoatPathing"),
	0,
	TEXT("Draws debug info about Boat Pathing")
	TEXT(" 0: Do not show debug info/n")
	TEXT(" 1: Show Debug info/n"),
	ECVF_Cheat
);

static TAutoConsoleVariable<int32> CVarCustomBoatPathing(
	TEXT("CB.CustomBoatPathing"),
	0,
	TEXT("Enables/Disables Custom Boat Pathing(our own solution)")
	TEXT(" 0: Custom Boat Pathing is NOT enabled/n")
	TEXT(" 1: Custom Boat Pathing is enabled/n"),
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
	BoatPathingVis = CreateDefaultSubobject<UInstancedStaticMeshComponent>(FName("BoatPathingVis"));
	BoatPathingVisSpline = CreateDefaultSubobject<USplineComponent>(FName("PathingVisSpline"));
	
	SetRootComponent(SphereTrigger);
	BoatMesh->SetupAttachment(GetRootComponent());
	DebrisSpawnLocation->SetupAttachment(GetRootComponent());
	BoatPathingVis->SetupAttachment(BoatPathingVisSpline);

	CurrentPathingState = EBoatPathingState::None;
}

void ACBBoat::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;
	NavAgentProperties = NavAgentProperties.DefaultProperties;
	NavAgentProperties.AgentRadius = SphereTrigger->GetUnscaledSphereRadius();
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

	OrientRotationToMovement(DeltaTime);
	const bool bDrawDebug = CVarDrawDebugBoatPathing.GetValueOnAnyThread() > 0;
	if (bDrawDebug)
	{
		DrawBoatDebugPathing();
	}
}

void ACBBoat::SetState(EBoatPathingState NewState)
{
	CurrentPathingState = NewState;
}

void ACBBoat::FollowLight(float DeltaTime)
{
	if (!FollowTarget)
	{
		Execute_OnLightFocusEnd(this);
	}
	
	MovementDirection = (FollowTarget->GetComponentLocation() - GetActorLocation()).GetSafeNormal();
	MovementDirection.Z = 0;
	AddActorWorldOffset(MovementDirection * LightFollowSpeed * DeltaTime);
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

void ACBBoat::VisualisePath()
{
	const float ActorZ = GetActorLocation().Z;
	BoatPathingVisSpline->ClearSplinePoints();
	for (const auto& Point : ReturnToPathPoints)
	{
		const FVector TargetPoint = FVector(Point.X, Point.Y, ActorZ);
		BoatPathingVisSpline->AddSplinePoint(TargetPoint, ESplineCoordinateSpace::World);
	}
	CurrentPath = BoatPathingVisSpline;
	AddInstancedMeshesForPathVis();
}

void ACBBoat::GenerateNewPath()
{
	const bool bDrawDebug = CVarDrawDebugBoatPathing.GetValueOnAnyThread() > 0;
	const bool bUseCustomBoatPathing = CVarCustomBoatPathing.GetValueOnAnyThread() > 0;
	if (!bUseCustomBoatPathing)
	{
		GeneratePathUsingNavSystem(bDrawDebug);
	}
	else
	{
		GeneratePathCustom(bDrawDebug);
	}
}

void ACBBoat::GeneratePathCustom(bool bDrawDebug)
{
	const FVector DesiredLocation = FindDesiredLocation(bDrawDebug);

	// Check for obstacles on path
	ReturnToPathPoints.Empty();
	ReturnToPathPoints.Add(GetActorLocation());
	TArray<FHitResult> HitResults;
	const TArray<AActor*> ActorsToIgnore = { this };
	const float Radius = SphereTrigger->GetScaledSphereRadius();
	if (UKismetSystemLibrary::SphereTraceMultiForObjects(GetWorld(), GetActorLocation(), DesiredLocation, Radius, ObjectTypesToQuery, false, ActorsToIgnore, EDrawDebugTrace::None, HitResults, true))
	{
		// If obstacles have been found, find an offset vector and add it to the path points array
		for (int32 Index = 0; Index < HitResults.Num(); ++Index)
		{
			// Determine if the vector is right leaning, if so offset to the right, else offset to the left
			// @TODO: Improve how this is determined as using the normal is not ideal, it can lead to many problems
			const bool bRightLeaning = FVector::DotProduct(HitResults[Index].ImpactNormal, FVector::RightVector) > 0;
			const FVector CorrectionDirection = bRightLeaning ? FVector::RightVector : FVector::LeftVector;
			const FBox Extend = HitResults[Index].GetActor()->GetComponentsBoundingBox(false, true);
			const FVector CorrectionLocation = HitResults[Index].ImpactPoint + (CorrectionDirection * (Radius + CorrectionDistanceOffset + Extend.GetExtent().Y / 2));
			ReturnToPathPoints.Add(CorrectionLocation);
			if (UKismetSystemLibrary::SphereTraceMultiForObjects(GetWorld(), CorrectionLocation, DesiredLocation, Radius, ObjectTypesToQuery, false, ActorsToIgnore, EDrawDebugTrace::None, HitResults, true))
			{
				Index = 0;
			}
		}
	}

	// Add Last location to the path point array
	ReturnToPathPoints.Add(DesiredLocation);
	VisualisePath();
}

void ACBBoat::GeneratePathUsingNavSystem(bool bDrawDebug)
{
	const FVector DesiredLocation = FindDesiredLocation(bDrawDebug);
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	
	if (CurrentQueryIndex != INDEX_NONE)
	{
		NavSys->AbortAsyncFindPathRequest(CurrentQueryIndex);
	}
	
	FPathFindingQuery Query;
	Query.StartLocation = GetActorLocation();
	Query.NavData = NavSys->GetNavDataForProps(GetNavAgentPropertiesRef(), GetActorLocation(), GetComponentsBoundingBox().GetExtent());
	Query.EndLocation = DesiredLocation;
	Query.SetNavAgentProperties(GetNavAgentPropertiesRef());
	Query.QueryFilter = UNavigationQueryFilter::GetQueryFilter(*Query.NavData, this, nullptr);
	FNavPathQueryDelegate Delegate;
	Delegate.BindUObject(this, &ThisClass::OnPathQueryFinished);
	CurrentQueryIndex = NavSys->FindPathAsync(GetNavAgentPropertiesRef(), Query, Delegate);
}

void ACBBoat::OnPathQueryFinished(uint32 QueryID, ENavigationQueryResult::Type QueryResult,
	TSharedPtr<FNavigationPath> NavigationPath)
{
	if (QueryResult == ENavigationQueryResult::Success)
	{
		ReturnToPathPoints.Empty();
		for (const auto& PathPoint : NavigationPath->GetPathPoints())
		{
			ReturnToPathPoints.Add(PathPoint.Location);
			PointIndex = 0;
		}

		VisualisePath();
	}

	UE_LOG(CBLog, Warning, TEXT("Boat failed to generate path! This might be very bad, someone should take a look!"));
	CurrentQueryIndex = INDEX_NONE;
}

FVector ACBBoat::FindDesiredLocation(const bool bDrawDebug)
{
	TargetSpline = ICBPathProvider::Execute_GetClosestPath(GetWorld()->GetAuthGameMode(), this);
	FVector DesiredLocation = TargetSpline->GetLocationAtSplinePoint(TargetSpline->GetNumberOfSplinePoints() - 1, ESplineCoordinateSpace::World);
	DesiredLocation.Z = GetActorLocation().Z;

	if (bDrawDebug)
	{
		DrawDebugSphere(GetWorld(), DesiredLocation, 8, 8, FColor::Yellow, false, 5);
	}
	
	CachedDestination = DesiredLocation;
	return DesiredLocation;
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
	
	SetState(EBoatPathingState::FollowingPath);
}

FOnPathingActorLeftPath& ACBBoat::PathingActorLeftPathEvent()
{
	return OnPathingActorLeftPath;
}

FOnNewPathChosen& ACBBoat::NewPathChosenEvent()
{
	return OnNewPathChosen;
}

USplineComponent* ACBBoat::GetPath_Implementation()
{
	return CurrentPath;
}

const FNavAgentProperties& ACBBoat::GetNavAgentPropertiesRef() const
{
	return NavAgentProperties;
}

FVector ACBBoat::GetNavAgentLocation() const
{
	return GetActorLocation();
}

void ACBBoat::OnLightFocused_Implementation(UPrimitiveComponent* TargetComponent)
{
	if (IsFollowingLight())
	{
		UE_LOG(CBLog, Warning, TEXT("Receiving a secondary light interaction, when boat is already following path."))
		return;
	}
	
	FollowTarget = TargetComponent;
	SetState(EBoatPathingState::FollowingLight);
	ReturnToPathPoints.Empty();
	OnPathingActorLeftPath.Broadcast(this);

	GetWorldTimerManager().ClearTimer(RedrawInstancedMeshTimerHandle);
	
	if (!RegeneratePathTimerHandle.IsValid())
	{
		GetWorldTimerManager().SetTimer(RegeneratePathTimerHandle, this, &ThisClass::RegeneratePath_Elapsed, RegeneratePathDelay, true);
	}
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
	SetState(EvaluateStatePostFollowLight());
	GetWorldTimerManager().ClearTimer(RegeneratePathTimerHandle);
}

void ACBBoat::TakeDamage_Implementation(AActor* InstigatorActor, EDestroyingObject DestroyingObject, float IncomingDamage)
{
	Health = FMath::Clamp(Health - IncomingDamage, 0.f, MaxHealth);
	
	if (Health <= 0.f)
	{
		Die(DestroyingObject);
	}

	// @NOTE: Can add some visual feedback here
}

float ACBBoat::GetMaxHealth_Implementation() const
{
	return MaxHealth;
}

float ACBBoat::GetCurrentHealth_Implementation() const
{
	return Health;
}

void ACBBoat::DestroyOnFinishedMoving(AActor* MovingActor)
{
	Destroy();
}

void ACBBoat::Die(EDestroyingObject DestroyingObject)
{
	if (TEST_BIT(DebrisLeavingObjects, DestroyingObject))
	{
		LeaveDebris(GetActorLocation());
	}

	SetActorEnableCollision(false);
	OnPathingActorLeftPath.Broadcast(this);
	
	if (DestroyingObject != EDestroyingObject::Port)
	{
		ICBPlayerInterface::Execute_ApplyChangeToPlayerReputation(UGameplayStatics::GetPlayerState(this, 0), -ReputationLoss);
		FMoveActorToActionData Data;
		Data.bUseActorLocationAsStart = true;
		Data.EndLocation = GetActorLocation() - (GetActorUpVector() * SinkingZOffset);
		Data.InterpDuration = 2.f;
		UCBMoveActorToAction* Action = UCBMoveActorToAction::Execute(this, this, Data);
		Action->OnActorFinishedMoving.AddDynamic(this, &ThisClass::DestroyOnFinishedMoving);
	}
	else
	{
		ICBPlayerInterface::Execute_ApplyChangeToPlayerReputation(UGameplayStatics::GetPlayerState(this, 0), ReputationRegain);
		Destroy();
	}
	

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
		BoatPathingVis->SetVisibility(false);
		CurrentPath = SplineComponent;
		ICBPathProvider::Execute_RegisterPathingActorWithPath(GetWorld()->GetAuthGameMode(), this, SplineComponent);
	}
	
	const FVector ClosestSplinePoint = CurrentPath->FindLocationClosestToWorldLocation(GetActorLocation(), ESplineCoordinateSpace::World);
	const float DistanceToClosestPathPoint = (ClosestSplinePoint - GetActorLocation()).Length();
	if (DistanceToClosestPathPoint > MaxDistanceToPathAllowed)
	{
		OnPathingActorLeftPath.Broadcast(this);
		TargetSpline = CurrentPath;
		GenerateNewPath();
		GetWorldTimerManager().SetTimer(RedrawInstancedMeshTimerHandle, this, &ACBBoat::AddInstancedMeshesForPathVis, RedrawInstancedMeshDelay, true);
	}
	
	OnNewPathChosen.Broadcast(CurrentPath);
	return EBoatPathingState::FollowingPath;
}

void ACBBoat::RegeneratePath_Elapsed()
{
	GenerateNewPath();
}

void ACBBoat::AddInstancedMeshesForPathVis()
{
	BoatPathingVis->ClearInstances();
	BoatPathingVis->SetVisibility(true);
	BoatPathingVis->SetStaticMesh(BoatPathingVisMesh);

	const FBox Bounds = BoatPathingVisMesh->GetBoundingBox();
	const float Spacing = Bounds.Max.X - Bounds.Min.X;

	const float Length = (GetActorLocation() - CachedDestination).Length();
	const int32 NumberOfInstances = FMath::FloorToInt32(Length / Spacing);
	const float DistanceToEnd = BoatPathingVisSpline->GetSplineLength();
	for (int i = 0; i < NumberOfInstances + 1; ++i)
	{
		const float Distance = DistanceToEnd - (Spacing * i);
		const FVector Location = BoatPathingVisSpline->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::Local);
		FRotator Rotation = BoatPathingVisSpline->GetRotationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::Local);
		Rotation.Yaw += 90.f;
		FTransform Transform(Rotation, Location);
		BoatPathingVis->AddInstance(Transform);
	}
}
