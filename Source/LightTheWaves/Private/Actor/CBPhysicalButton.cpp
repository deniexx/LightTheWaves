// Made By Cubic Burrito


#include "Actor/CBPhysicalButton.h"

#include "Components/BoxComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "PhysicsEngine/PhysicsThrusterComponent.h"

static TAutoConsoleVariable<int32> CVarDrawDebugPhysicalButtons(
	TEXT("ShowDebugPhysicalButtons"),
	0,
	TEXT("Draws debug info on physical button activation, can be used to confirm if they are being activated")
	TEXT(" 0: Do not show debug info/n")
	TEXT(" 1: Show Debug info/n"),
	ECVF_Cheat
);

// Sets default values
ACBPhysicalButton::ACBPhysicalButton()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

#pragma region Component Subobject Creation
	SceneRootComponent = CreateDefaultSubobject<USceneComponent>(FName("Root"));
	ButtonBody = CreateDefaultSubobject<UStaticMeshComponent>(FName("ButtonBody"));
	ActivationBox = CreateDefaultSubobject<UBoxComponent>(FName("ActivationBox"));
	PhysicsConstraintComponent = CreateDefaultSubobject<UPhysicsConstraintComponent>(FName("PhysicsConstraintComponent"));
	ButtonTop = CreateDefaultSubobject<UStaticMeshComponent>(FName("ButtonTop"));
	PhysicsThrusterComponent = CreateDefaultSubobject<UPhysicsThrusterComponent>(FName("PhysicsThrusterComponent"));
#pragma endregion 

#pragma region Attachments
	SetRootComponent(SceneRootComponent);
	ButtonBody->SetupAttachment(GetRootComponent());
	ActivationBox->SetupAttachment(GetRootComponent());
	PhysicsConstraintComponent->SetupAttachment(GetRootComponent());
	ButtonTop->SetupAttachment(GetRootComponent());
	PhysicsThrusterComponent->SetupAttachment(ButtonTop);
#pragma endregion

#pragma region Default Values
	PhysicsThrusterComponent->ThrustStrength = 5000.f;
	PhysicsThrusterComponent->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	PhysicsThrusterComponent->SetAutoActivate(true);
	
	PhysicsConstraintComponent->SetConstrainedComponents(ButtonBody, FName(""), ButtonTop, FName(""));
	PhysicsConstraintComponent->SetDisableCollision(true);
	PhysicsConstraintComponent->SetLinearXLimit(LCM_Locked, 0.f);
	PhysicsConstraintComponent->SetLinearYLimit(LCM_Locked, 0.f);
	PhysicsConstraintComponent->SetLinearZLimit(LCM_Limited, 5.f);

	PhysicsConstraintComponent->SetAngularSwing1Limit(ACM_Locked, 0.f);
	PhysicsConstraintComponent->SetAngularSwing2Limit(ACM_Locked, 0.f);
	PhysicsConstraintComponent->SetAngularTwistLimit(ACM_Locked, 0.f);
#pragma endregion
}

// Called when the game starts or when spawned
void ACBPhysicalButton::BeginPlay()
{
	Super::BeginPlay();

	// @NOTE: This is here in order to make sure that the button physics body does not sleep when starting the game
	ButtonTop->SetSimulatePhysics(true);
	ActivationBox->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnOverlap);
}

void ACBPhysicalButton::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                  UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherComp == ButtonTop)
	{
		const bool bShowDebug = CVarDrawDebugPhysicalButtons.GetValueOnAnyThread() > 0;
		if (bShowDebug)
		{
			DrawDebugSphere(GetWorld(), GetActorLocation(), 12, 12, FColor::Blue, true, 3.f);
		}
		
		OnButtonActivated();
	}
}

void ACBPhysicalButton::OnButtonActivated_Implementation()
{

}
