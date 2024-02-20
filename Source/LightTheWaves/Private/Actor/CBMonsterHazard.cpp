// Made By Cubic Burrito


#include "Actor/CBMonsterHazard.h"

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
	}
	
	SetLifeSpan(5.f);
}

void ACBMonsterHazard::OnNewPathChosen(USplineComponent* NewPath)
{
	if (TimesFollowedBoat < ChancesToFollowBoat.Num())
	{
		const int32 Chance = ChancesToFollowBoat[TimesFollowedBoat];
		const int32 DiceRoll = FMath::RandRange(0, 99);

		if (DiceRoll < Chance)
		{
			const FVector BoatDestination = NewPath->GetWorldLocationAtSplinePoint(NewPath->GetNumberOfSplinePoints() - 1);
			const float InputKey = NewPath->FindInputKeyClosestToWorldLocation(Target->GetActorLocation());
			const float DistanceAlongSpline = NewPath->GetDistanceAlongSplineAtSplineInputKey(InputKey);
			const float OffsetDistance = DistanceAlongSpline + FMath::FRandRange(MinMaxFollowDistanceFromBoat.X, MinMaxFollowDistanceFromBoat.Y);
			const FVector NewLocation = NewPath->GetWorldLocationAtDistanceAlongSpline(OffsetDistance);

			if ((BoatDestination - NewLocation).Length() < MinDistanceToEnd)
			{
				// @NOTE: Maybe adjust location of monster here
				return;
			}
			
			SetActorLocation(NewLocation);
		}
		
		++TimesFollowedBoat;
	}
}

void ACBMonsterHazard::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	SetLifeSpan(LifeSpan);
	CapsuleTrigger->OnComponentBeginOverlap.AddDynamic(this, &ACBMonsterHazard::OnOverlap);
}

void ACBMonsterHazard::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor && IsValid(OtherActor) && OtherActor->Implements<UCBDestroyableObject>())
	{
		ICBDestroyableObject::Execute_TakeDamage(OtherActor, this, EDestroyingObject::Monster, ICBDestroyableObject::Execute_GetMaxHealth(OtherActor));
		Destroy();
	}
}


