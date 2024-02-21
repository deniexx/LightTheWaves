// Made By Cubic Burrito

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CBProjectile.generated.h"

class UProjectileMovementComponent;
class USphereComponent;

UCLASS()
class LIGHTTHEWAVES_API ACBProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACBProjectile();

	FVector GetLaunchVelocity() const;

	float GetLifetime() const;

	float GetAreaOfEffect() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> SphereCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> StaticMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	float LifeSpan = 5.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	float AreaOfEffect = 0.f;	
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
