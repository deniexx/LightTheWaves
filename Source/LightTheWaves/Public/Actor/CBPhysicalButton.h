// Made By Cubic Burrito

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CBPhysicalButton.generated.h"

class UPhysicsThrusterComponent;
class UPhysicsConstraintComponent;
class UBoxComponent;

UCLASS()
class LIGHTTHEWAVES_API ACBPhysicalButton : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACBPhysicalButton();

protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	TObjectPtr<USceneComponent> SceneRootComponent;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	TObjectPtr<UStaticMeshComponent> ButtonBody;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	TObjectPtr<UBoxComponent> ActivationBox;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	TObjectPtr<UPhysicsConstraintComponent> PhysicsConstraintComponent;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	TObjectPtr<UStaticMeshComponent> ButtonTop;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	TObjectPtr<UPhysicsThrusterComponent> PhysicsThrusterComponent;

	UFUNCTION(BlueprintNativeEvent)
	void OnButtonActivated();

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
