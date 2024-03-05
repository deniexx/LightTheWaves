// Made By Cubic Burrito


#include "Actor/CBMonsterHazard.h"

#include "Async/CBMoveActorToAction.h"
#include "Components/SplineComponent.h"
#include "Interface/CBDestroyableObject.h"
#include "Interface/CBPathingActor.h"

// Sets default values
ACBMonsterHazard::ACBMonsterHazard()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CapsuleTrigger = CreateDefaultSubobject<UCapsuleComponent>(FName("CapsuleTrigger"));
	MonsterMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("MonsterMesh"));

	SetRootComponent(CapsuleTrigger);
	MonsterMesh->SetupAttachment(GetRootComponent());

	CapsuleTrigger->SetCollisionResponseToAllChannels(ECR_Overlap);
}

void ACBMonsterHazard::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	SetLifeSpan(LifeSpan);
	CapsuleTrigger->OnComponentBeginOverlap.AddDynamic(this, &ACBMonsterHazard::OnOverlap);
}

void ACBMonsterHazard::BeginPlay()
{
	Super::BeginPlay();
	SetActorLocation(GetActorLocation() - (GetActorUpVector() * 600.f));

	FMoveActorToActionData Data;
	Data.bUseActorLocationAsStart = true;
	Data.InterpDuration = 2.f;
	Data.EndLocation = GetActorLocation() + (GetActorUpVector() * 600.f);
	UCBMoveActorToAction* MoveActorTo = UCBMoveActorToAction::Execute(this, this, Data);
}

void ACBMonsterHazard::Destroyed()
{
	OnMonsterDead.Broadcast(this);
	Super::Destroyed();
}

FOnMonsterDead& ACBMonsterHazard::OnMonsterDeadEvent()
{
	return OnMonsterDead;
}

AActor* ACBMonsterHazard::GetTarget_Implementation()
{
	return Target;
}

bool ACBMonsterHazard::IsTargeting_Implementation() const
{
	return Target != nullptr;
}

void ACBMonsterHazard::SetTarget_Implementation(AActor* InTarget)
{
	if (InTarget)
	{
		Target = InTarget;
		Cast<ICBPathingActor>(Target)->NewPathChosenEvent().AddDynamic(this, &ThisClass::OnNewPathChosen);
		return;
	}
	
	SetLifeSpan(5.f);
}

bool ACBMonsterHazard::AttemptRelocation()
{
	const USplineComponent* Path = nullptr;
	if (Target)
	{
		Path = ICBPathingActor::Execute_GetPath(Target);
	}

	if (!Path) return false;
	
	const FVector BoatDestination = Path->GetWorldLocationAtSplinePoint(Path->GetNumberOfSplinePoints() - 1);
	const float InputKey = Path->FindInputKeyClosestToWorldLocation(Target->GetActorLocation());
	const float DistanceAlongSpline = Path->GetDistanceAlongSplineAtSplineInputKey(InputKey);
	const float OffsetDistance = DistanceAlongSpline + FMath::FRandRange(MinMaxFollowDistanceFromBoat.X, MinMaxFollowDistanceFromBoat.Y);
	FVector NewLocation = Path->GetWorldLocationAtDistanceAlongSpline(OffsetDistance);

	if ((BoatDestination - NewLocation).Length() < MinDistanceToEnd)
	{
		// @NOTE: Maybe adjust location of monster here
		return false;
	}

	// Force the height to be unchanged
	NewLocation.Z = GetActorLocation().Z;
	SetActorLocation(NewLocation);
	++TimesFollowedBoat;
	return true;
}

void ACBMonsterHazard::OnNewPathChosen(USplineComponent* NewPath)
{
	if (TimesFollowedBoat < ChancesToFollowBoat.Num())
	{
		const int32 Chance = ChancesToFollowBoat[TimesFollowedBoat];
		const int32 DiceRoll = FMath::RandRange(0, 99);

		if (DiceRoll < Chance)
		{
			FMoveActorToActionData Data;
			Data.bUseActorLocationAsStart = true;
			Data.InterpDuration = 2.f;
			Data.EndLocation = GetActorLocation() - (GetActorUpVector() * 600.f);
			UCBMoveActorToAction* MoveActorTo = UCBMoveActorToAction::Execute(this, this, Data);
			MoveActorTo->OnActorFinishedMoving.AddDynamic(this, &ThisClass::OnMonsterFinishedSubmerging);
		}
	}
}

void ACBMonsterHazard::OnMonsterFinishedSubmerging(AActor* MovingActor)
{
	if (AttemptRelocation())
	{
		FMoveActorToActionData Data;
		Data.bUseActorLocationAsStart = true;
		Data.InterpDuration = 2.f;
		Data.EndLocation = GetActorLocation() + (GetActorUpVector() * 600.f);
		UCBMoveActorToAction* MoveAction = UCBMoveActorToAction::Execute(this, this, Data);
	}

	Destroy();
}

void ACBMonsterHazard::DestroyAfterSubmerge(AActor* MovingActor)
{
	Destroy();
}

void ACBMonsterHazard::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor && IsValid(OtherActor) && OtherActor->Implements<UCBDestroyableObject>())
	{
		ICBDestroyableObject::Execute_TakeDamage(OtherActor, this, EDestroyingObject::Monster, ICBDestroyableObject::Execute_GetMaxHealth(OtherActor));
		FMoveActorToActionData Data;
		Data.bUseActorLocationAsStart = true;
		Data.InterpDuration = 2.f;
		Data.EndLocation = GetActorLocation() - (GetActorUpVector() * 600.f);
		UCBMoveActorToAction* MoveActorTo = UCBMoveActorToAction::Execute(this, this, Data);
		MoveActorTo->OnActorFinishedMoving.AddDynamic(this, &ThisClass::DestroyAfterSubmerge);
		SetActorEnableCollision(false);
	}
}


