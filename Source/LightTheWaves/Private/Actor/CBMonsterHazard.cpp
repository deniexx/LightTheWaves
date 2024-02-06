// Made By Cubic Burrito


#include "Actor/CBMonsterHazard.h"

#include "Interface/CBDestroyableObject.h"
#include "LightTheWaves/LightTheWaves.h"

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
		ICBDestroyableObject::Execute_OnDestroyed(OtherActor, this);
		Destroy();
	}
}


