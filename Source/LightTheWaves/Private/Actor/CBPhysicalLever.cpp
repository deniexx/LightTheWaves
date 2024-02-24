// Made By Cubic Burrito


#include "Actor/CBPhysicalLever.h"

#include "Components/BoxComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "PhysicsEngine/PhysicsThrusterComponent.h"

// Sets default values
ACBPhysicalLever::ACBPhysicalLever()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

#pragma region Component Subobject Creation
	SceneRootComponent = CreateDefaultSubobject<USceneComponent>(FName("RootComponent"));
	Lever = CreateDefaultSubobject<UStaticMeshComponent>(FName("Lever"));
	Body = CreateDefaultSubobject<UStaticMeshComponent>(FName("Body"));
	PhysicsConstraintComponent = CreateDefaultSubobject<UPhysicsConstraintComponent>(FName("PhysicsConstraintComponent"));
	PhysicsThrusterComponent = CreateDefaultSubobject<UPhysicsThrusterComponent>(FName("PhysicsThrusterComponent"));
	ActivationBox = CreateDefaultSubobject<UBoxComponent>(FName("ActivationBox"));
#pragma endregion

#pragma region Attachment
	SetRootComponent(SceneRootComponent);
	Lever->SetupAttachment(GetRootComponent());
	PhysicsThrusterComponent->SetupAttachment(Lever);
	Body->SetupAttachment(GetRootComponent());
	PhysicsConstraintComponent->SetupAttachment(GetRootComponent());
	ActivationBox->SetupAttachment(GetRootComponent());
#pragma endregion

#pragma region DefaultValues
	PhysicsConstraintComponent->SetConstrainedComponents(Body, FName(""), Lever, FName(""));
	PhysicsConstraintComponent->SetLinearXLimit(LCM_Locked, 0.f);
	PhysicsConstraintComponent->SetLinearYLimit(LCM_Locked, 0.f);
	PhysicsConstraintComponent->SetLinearZLimit(LCM_Locked, 0.f);
	PhysicsConstraintComponent->SetAngularSwing1Limit(ACM_Locked, 0.f);
	PhysicsConstraintComponent->SetAngularSwing2Limit(ACM_Limited, 45.f);
	PhysicsConstraintComponent->SetAngularTwistLimit(ACM_Locked, 0.f);
	PhysicsConstraintComponent->SetDisableCollision(true);
	Lever->SetCollisionProfileName(FName("BlockAllDynamic"));
	Body->SetCollisionProfileName(FName("BlockAllDynamic"));
	PhysicsThrusterComponent->ThrustStrength = -1500.f;
	PhysicsThrusterComponent->bAutoActivate = true;
#pragma endregion
	
}

void ACBPhysicalLever::OnActivationBeginOverlap(UPrimitiveComponent* PrimitiveComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, int OtherBodyIndex, bool bFromSweep,const FHitResult& HitResult)
{
	if (OtherComponent == Lever)
	{
		OnLeverActivated();
	}
}

void ACBPhysicalLever::OnActivationEndOverlap(UPrimitiveComponent* PrimitiveComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, int OtherBodyIndex, bool bFromSweep, const FHitResult& HitResult)
{
	if(OtherComponent == Lever)
	{
		OnLeverDeActivated();
	}
}

// Called when the game starts or when spawned
void ACBPhysicalLever::BeginPlay()
{
	Super::BeginPlay();

	Lever->SetSimulatePhysics(true);
	ActivationBox->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnActivationBeginOverlap);
}

void ACBPhysicalLever::OnLeverDeActivated_Implementation()
{
}

void ACBPhysicalLever::OnLeverActivated_Implementation()
{
	// @TODO: Add debug draw info based on console variable
}
