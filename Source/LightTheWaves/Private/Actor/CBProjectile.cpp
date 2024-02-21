// Made By Cubic Burrito


#include "Actor/CBProjectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ACBProjectile::ACBProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereCollision = CreateDefaultSubobject<USphereComponent>(FName("SphereCollision"));
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("StaticMesh"));
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(FName("ProjectileMovement"));

	SetRootComponent(SphereCollision);
	StaticMesh->SetupAttachment(SphereCollision);
	
	ProjectileMovement->Velocity = FVector(1200.f, 0.f, 0.f);
}

FVector ACBProjectile::GetLaunchVelocity() const
{
	return ProjectileMovement->Velocity;
}

float ACBProjectile::GetLifetime() const
{
	return LifeSpan;
}

float ACBProjectile::GetAreaOfEffect() const
{
	return AreaOfEffect;
}

// Called when the game starts or when spawned
void ACBProjectile::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(LifeSpan);
}

// Called every frame
void ACBProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

