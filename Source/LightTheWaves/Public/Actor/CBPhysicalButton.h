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

	/** The Root Component of the Actor */
	UPROPERTY(VisibleAnywhere, Category = "Component")
	TObjectPtr<USceneComponent> SceneRootComponent;

	/** The body of the button to be used */
	UPROPERTY(VisibleAnywhere, Category = "Component")
	TObjectPtr<UStaticMeshComponent> ButtonBody;

	/** The box collision that detects the activation of the button */
	UPROPERTY(VisibleAnywhere, Category = "Component")
	TObjectPtr<UBoxComponent> ActivationBox;
	
	UPROPERTY(VisibleAnywhere, Category = "Component")
	TObjectPtr<UPhysicsConstraintComponent> PhysicsConstraintComponent;

	/** The actual button mesh that will be pushed */
	UPROPERTY(VisibleAnywhere, Category = "Component")
	TObjectPtr<UStaticMeshComponent> ButtonTop;

	/** This will push the button back to it's original position */
	UPROPERTY(VisibleAnywhere, Category = "Component")
	TObjectPtr<UPhysicsThrusterComponent> PhysicsThrusterComponent;

	UPROPERTY(VisibleAnywhere, Category = "Button Properties")
	TObjectPtr<USoundBase> ClickSound;
	
	/** This Event will be called once the button has been activated(pushed far enough to where the top collides with the activation box) */
	UFUNCTION(BlueprintNativeEvent)
	void OnButtonActivated();

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
