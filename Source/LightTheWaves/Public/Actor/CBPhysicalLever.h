// Made By Cubic Burrito

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CBPhysicalLever.generated.h"

class UPhysicsConstraintComponent;
class UPhysicsThrusterComponent;
class UBoxComponent;

UCLASS()
class LIGHTTHEWAVES_API ACBPhysicalLever : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACBPhysicalLever();

protected:
	UFUNCTION()
	void OnActivationBeginOverlap(UPrimitiveComponent* PrimitiveComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int OtherBodyIndex, bool bFromSweep,
	                              const FHitResult& HitResult);
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	UBoxComponent* ActivationBox;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	TObjectPtr<USceneComponent> SceneRootComponent;
	
	UPROPERTY(VisibleAnywhere, Category = "Component")
	TObjectPtr<UStaticMeshComponent> Lever;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	TObjectPtr<UStaticMeshComponent> Body;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	TObjectPtr<UPhysicsConstraintComponent> PhysicsConstraintComponent;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	TObjectPtr<UPhysicsThrusterComponent> PhysicsThrusterComponent;

	UFUNCTION(BlueprintNativeEvent)
	void OnLeverActivated();
};
